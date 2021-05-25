// tbwebsocket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket server, synchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include "dealdata.hpp"
#include <stdlib.h>
#include <fstream>

#include <sys\stat.h>
#include <windows.h>
#include <io.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;     // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

//std::ifstream myfile("F:\\jiesen.txt");
void* ptr;
// Echoes back all received WebSocket messages
void
do_session(tcp::socket socket)
{
    try
    {
        // Construct the stream by moving in the socket
        websocket::stream<tcp::socket> ws{ std::move(socket) };

        // Set a decorator to change the Server of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-server-sync");
            }));

        // Accept the websocket handshake
        ws.accept();

        for (;;)
        {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            //ws.read(buffer);
            //std::cout << beast::make_printable(buffer.data()) << std::endl;

            //std::ifstream file("F:\\jiesen.txt");/*只能在文件内io？*/
            //std::istreambuf_iterator<char> beg(file), end;
            std::string str;

                    
            ws.text(true/*ws.got_text()*/);       //text函数控制text还是bin二进制,设置成false无法回环消息
            str = dealdata();   
           
            if (str[0] == '{' && str.back() == '}')//筛选一下      
            {
                ws.write(net::buffer(str,str.length()+1));
                std::cout << str <<"   ----websocket message"<< std::endl; 
                std::cout << std::endl;
                //Sleep(500); //ws.write(net::buffer(ptr, 100)); 
            }
            if (str[0] == '[' && str.back() == ']')//这个说明要修改字符写入文件以供阅读
            {
                std::cout << str << "   ----http tofile" << std::endl;
                str[0] = '{'; str[str.length() - 1] = '}';
                std::ofstream os;
                os.open("F:\\json.txt");
                os << str;   
                os.close();
            }



        }
    }
    catch (beast::system_error const& se)
    {
        // This indicates that the session was closed
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try
    {
        //boost::asio::io_service iosev;
        //boost::asio::serial_port sp1(iosev, "com5");
        //sp1.set_option(boost::asio::serial_port::baud_rate(115200));
        //sp1.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
        //sp1.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
        //sp1.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
        //sp1.set_option(boost::asio::serial_port::character_size(8));
        // Check command line arguments.
      /*  if (argc != 3)
        {
            std::cerr <<
                "Usage: websocket-server-sync <address> <port>\n" <<
                "Example:\n" <<
                "    websocket-server-sync 0.0.0.0 8080\n";
            return EXIT_FAILURE;
        }*/
        auto const address = net::ip::make_address("0.0.0.0");
        auto const port = static_cast<unsigned short>(std::atoi("8888"));


        // The io_context is required for all I/O
        net::io_context ioc{ 1 };

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{ ioc, {address, port} };
        for (;;)
        {
            // This will receive the new connection
            tcp::socket socket{ ioc };

            // Block until we get a connection
            acceptor.accept(socket);

            // Launch the session, transferring ownership of the socket
            std::thread(
                &do_session,
                std::move(socket)).detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
