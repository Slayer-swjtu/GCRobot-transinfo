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

		//4 ��ȡ���յ����ݣ���һ�����뵽request_handler.cpp��
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
					�����get����ԭ�����û����
					�����post���������ֻ�ܽ��յ�һ������
					��Ϊget������һ��tcp ��post����������tcp
					����post���󣬵ڶ���tcp��ȡ�õ���result��request�����㷢�����ݵ�Ҫ��
					Ӧ��ʹ��post����ĵ�һ������
					�е�ʱ��post����Ҳ��һ�ξͿ���ȫ�������� 
					����ͷ���������м���\r\n\r\n���зָ�
					*/
					string data = buffer_.data();

					buffer_ = { '\0' };//���buffer
					if (v_data.size()==0)
					{
						//v_data.push_back("hhc");
						request_first = request_;
						result_first = result;   //ö�٣�good/bad��
						if (request_first.method == "POST")
						{
							//�е�ʱ��һ��post����Ϳ��Խ������е�����
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
									//һ�����ݽ��վ������
								}
								else {
									//��Ҫ�������ݲŽ�����
									do_read();
									return;
								}
							}
							
						}						
					}

					if (result_first== request_parser::good)
					{
						v_data.clear();
						//�������������� ֻ��post�������Ҫ���棬�ŵ�handle�д���
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