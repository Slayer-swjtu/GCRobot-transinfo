//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/*
boost asio创建http的用法如下：
1 在main函数中调用server类的构造函数 启动服务
2 在server类中调用了connection_manager类
3 在connection_manager类中调用了connection 这个类是重点要关注的一个类 包含了
  数据的读取do_read()函数以及数据的写入do_write()函数
  在数据的读取中内部调用两个类request_parser用于解析请求的数据
  当解析完后，调用request_handler中进行接收数据的分析，进而调用reply类进行响应数据的发送
*/

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
  try
  {

	const std::string ip = "0.0.0.0";      //允许一切链接，跟防火墙无关
	const std::string port = "8888";
	const std::string docroot = "F:/";
	 //1 第一步在main函数调用server类 下面这条语句就是初始化server中的构造函数 下一步到server.cpp中
	http::server::server s(ip, port, docroot);
     //Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
