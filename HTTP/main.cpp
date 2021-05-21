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
boost asio����http���÷����£�
1 ��main�����е���server��Ĺ��캯�� ��������
2 ��server���е�����connection_manager��
3 ��connection_manager���е�����connection ��������ص�Ҫ��ע��һ���� ������
  ���ݵĶ�ȡdo_read()�����Լ����ݵ�д��do_write()����
  �����ݵĶ�ȡ���ڲ�����������request_parser���ڽ������������
  ��������󣬵���request_handler�н��н������ݵķ�������������reply�������Ӧ���ݵķ���
*/

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
  try
  {

	const std::string ip = "0.0.0.0";      //����һ�����ӣ�������ǽ�޹�
	const std::string port = "8888";
	const std::string docroot = "F:/";
	 //1 ��һ����main��������server�� �������������ǳ�ʼ��server�еĹ��캯�� ��һ����server.cpp��
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
