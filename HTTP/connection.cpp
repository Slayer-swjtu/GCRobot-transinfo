//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"

#include<iostream>
#include<regex>
using namespace std;

namespace http {
	namespace server {

		connection::connection(boost::asio::ip::tcp::socket socket,
			connection_manager& manager, request_handler& handler)
			: socket_(std::move(socket)),
			connection_manager_(manager),
			request_handler_(handler)
		{
		}

		void connection::start()
		{
			do_read();
		}

		void connection::stop()
		{
			socket_.close();
		}

		//4 读取接收到数据，下一步传入到request_handler.cpp中
		void connection::do_read()
		{
			auto self(shared_from_this());
			socket_.async_read_some(boost::asio::buffer(buffer_),
				[this, self](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					request_parser::result_type result;
					std::tie(result, std::ignore) = request_parser_.parse(
						request_, buffer_.data(), buffer_.data() + bytes_transferred);
					/*
					如果是get请求，原代码就没问题
					如果是post请求，这个就只能接收第一次请求
					因为get请求是一次tcp 而post请求是两次tcp
					对于post请求，第二次tcp获取得到的result和request并满足发送数据的要求，
					应该使用post请求的第一条数据
					有的时候post请求也是一次就可以全部接收完 
					请求头和请求体中间以\r\n\r\n进行分隔
					*/
					string data = buffer_.data();

					buffer_ = { '\0' };//清空buffer
					if (v_data.size()==0)
					{
						//v_data.push_back("hhc");
						request_first = request_;
						result_first = result;   //枚举，good/bad等
						if (request_first.method == "POST")
						{
							//有的时候一次post请求就可以接收所有的数据
							smatch result1;
							regex r1("\r\n\r\n.*");
							string s1 = data;
							string::const_iterator it_start = s1.begin();
							string::const_iterator it_end = s1.end();
							string body;
							while (regex_search(it_start, it_end, result1, r1))
							{
								body = result1[0];
								it_start = result1[0].second;

							}
							if (!body.empty())
							{
								data = body.substr(4, body.length());
								if (!data.empty())
								{
									//一次数据接收就完成了
								}
								else {
									//需要两次数据才接收完
									do_read();
									return;
								}
							}
							
						}						
					}

					if (result_first== request_parser::good)
					{
						v_data.clear();
						//保存请求体数据 只有post请求才需要保存，放到handle中处理
						if (request_first.method == "POST")
						{
							cout << request_first.uri << endl;
							cout << request_first.body << endl;
							request_first.body = data;
							request_handler_.handle_request_POST(request_first, reply_);
							do_write();
						}
						if (request_first.method == "GET")
						{
							request_handler_.handle_request_GET(request_first, reply_);
							do_write();
						}
						
					}
					else if (result_first== request_parser::bad)
					{
						v_data.clear();
						reply_ = reply::stock_reply(reply::bad_request);
						do_write();
					}
					else
					{
						v_data.clear();
						do_read();
					}
				}
				else if (ec != boost::asio::error::operation_aborted)
				{
					v_data.clear();
					connection_manager_.stop(shared_from_this());
				}
			});
		}

		void connection::do_write()
		{
			auto self(shared_from_this());
			boost::asio::async_write(socket_, reply_.to_buffers(),
				[this, self](boost::system::error_code ec, std::size_t)
			{
				if (!ec)
				{
					// Initiate graceful connection closure.
					boost::system::error_code ignored_ec;
					socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
						ignored_ec);
				}

				if (ec != boost::asio::error::operation_aborted)
				{
					connection_manager_.stop(shared_from_this());
				}
			});
		}

	} // namespace server
} // namespace http