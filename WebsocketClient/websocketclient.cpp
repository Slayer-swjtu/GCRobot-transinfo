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
// Example: WebSocket client, synchronous
//
//------------------------------------------------------------------------------

//[example_websocket_client

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Sends a WebSocket message and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        /*if (argc != 4)
        {
            std::cerr <<
                "Usage: websocket-client-sync <host> <port> <text>\n" <<
                "Example:\n" <<
                "    websocket-client-sync echo.websocket.org 80 \"Hello, world!\"\n";
            return EXIT_FAILURE;
        }*/
        std::string host = "127.0.0.1";
        auto const  port = "8888";
        auto const  text = "{\"a\"=[\"b\"]}";


        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        auto ep = net::connect(ws.next_layer(), results);

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host += ':' + std::to_string(ep.port());

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-client-coro");
            }));

        // Perform the websocket handshake
        ws.handshake(host, "/");

        /*---------------------------------------------------------------*/

        for (;;)
        {
            // Send the message
            //ws.write(net::buffer(std::string(text)));

            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message into our buffer                             // 客户端收到第一个不合格的串口-websocket数据后就宕机了，如何处理
            ws.read(buffer);
            // Close the WebSocket connection
            //ws.close(websocket::close_code::normal)
            // If we get here then the connection is closed gracefully        
            // The make_printable() function helps print a ConstBufferSequence
            /*ostream左移重载的友元函数,make_printable返回m.p._adaptor类，<<函数重载，返回ostream的引用(不是ofstream)流二进制数据*/
            /*最后还是没能解析ostream&，用重定位绕开这个问题,但貌似会复写*/
            /*ver1日志会复写*/
            //// 保存cout流缓冲区指针
            //std::streambuf* coutbuf = std::cout.rdbuf();
            //std::ofstream file("F:\\out.log");
            //// 获取文件out.log流缓冲区指针
            //std::streambuf* filebuf = file.rdbuf();
            //// 设置cout流缓冲区指针为out.txt的流缓冲区指针
            //std::cout.rdbuf(filebuf);
            void* ptr = buffer.data().data();
            std::string str((char*)ptr,(char*)ptr+buffer.data().size());
            std::cout << str << std::endl;
           
            
            std::cout << beast::make_printable(buffer.data()) << "  -- webclient read from server"<<std::endl;
            //file.flush();
            //file.close();
            //// 恢复cout原来的流缓冲区指针
            //std::cout.rdbuf(coutbuf);
            
           /* freopen("F:\\out.json", "a", stdout);
            std::cout << beast::make_printable(buffer.data());
            fclose(stdout);*/

            
            /*ver2日志可续写但断点测试发现转回cmd会fatal*/                //遇到乱数据就不会往log里写了，准确地说是print都出不来了，是解决乱数据本身还是？？？
            
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//]