// serialtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <windows.h>
#include <boost/json.hpp>
#include <stdlib.h>
using namespace std;
using namespace boost::asio;
using namespace boost::json;


int main(int argc, char* argv[])
{
    io_service iosev;
    // 串口COM1, Linux下为“/dev/ttyS0”

    serial_port sp2(iosev, "COM9");
    sp2.set_option(serial_port::baud_rate(115200));
    sp2.set_option(serial_port::flow_control(serial_port::flow_control::none));
    sp2.set_option(serial_port::parity(serial_port::parity::none));
    sp2.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
    sp2.set_option(serial_port::character_size(8));

    /*-------------------------------------------------------------------------*/
    double pi = 3.1415926;
    bool sa = false;
    value jv = {
    { "pi", pi },
    { "happy", sa },
    { "name", "Boost" },
    { "nothing", nullptr },
    { "answer", {
        { "everything", 42 } } },
    {"list", {1, 0, 2}},
    {"object", {
        { "currency", "USD" },
        { "value", 42.99 }
            } }
    };



    std::string s = serialize(jv);
    std::string popo(s) ;

    /*-------------------------------------------------------------------------*/
    // 向串口写数据
    //std::string  str = "{\"a\":\"b\",\"c\":\"d\"}";
    const void* ptr;    //指向常量的指针，不是指针指向不能变，而是指向一个对于此指针不能修改的常量
   
   // ptr = str;
    
    //char buf[100];

    ptr = (char*)s.data();

    for (int j = 0;;j++)
    {
        if (j%2==0)
        { 
            
            write(sp2,buffer(ptr,s.length()+1));
            std::cout << s<<std::endl;
        }  
        else
        {
            popo[0] = '['; 
            popo[popo.size()-1]= ']';
            write(sp2,buffer((char*)popo.data(),popo.length()+1));
            std::cout << popo << std::endl;
        }
        //async_read(sp2, buffer(buf), boost::bind(handle_read, buf, _1, _2));
             //buffer返回的是const_buffers_1类构造，char*强转为void*,两个buffer长度一致即可正确传递
        //向串口读数据
        //read(sp2, buffer(buf)); 
        //mutable_buffers_1类构造     
        //cout << buf << endl;
       // ptr = (char*)popo.data();    //可以重新赋值指针，但不可通过此指针修改
        
        Sleep(1000);//system("pause");          //两个数传数据质量堪忧，如何解决？？
        
    }
    
    iosev.run();
    return 0;
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
