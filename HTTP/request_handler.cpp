//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <json/json.h>
#include <map>
#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <string>
#include <iostream>
#include "cstjson.hpp"
#include "remote.hpp"


std::string param ; //存储？后的param
std::string value ;



void printMap(std::map<std::string, std::string>& m)
{
    for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++)
    {
        std::cout << "key = " << it->first << " value = " << it->second << std::endl;
    }
    std::cout << std::endl;
}

std::map<std::string ,std::string>  record2map(std::string& str)    //试图解析url中所有的键值对，保存到map
{
    std::string fake = str;     //被操作串
    std::vector<std::string> name;    
    std::vector<std::string> value;
    std::map<std::string, std::string> m;

    int flag = str.find('&');  //正则表达式貌似更容易，可惜我没用
    if (flag > 0) // 多个参数
    {
        //先录一个
        name.push_back(fake.substr(str.find('?')+1  , str.find('=') - str.find('?')-1));  //vector<string> 赋值方式用push_back,name[i]只用来print
        value.push_back(fake.substr(str.find('=')+1  , str.find('&') - str.find('=')-1)); 

        fake = fake.substr(str.find('&') + 1);

        //flag = fake.find('&');  //一定要转换为int，不然find返回的是一个size_t无符号数，死循环
        while ((int)fake.find('&') > 0)   //url子串里还是有&
        {
            name.push_back(fake.substr(0, fake.find('=')));
            value.push_back(fake.substr(fake.find('=')+1, fake.find('&') - fake.find('=')-1));
            fake = fake.substr(fake.find('&') + 1);
            
        }

        //url访问符已经无&子串
        name.push_back(fake.substr(0, fake.find('=')));
        value.push_back(fake.substr(fake.find('=')+1, fake.size() - fake.find('=')-1)) ;    

        for (int c = 0; c < name.size(); c++) 
            m.insert(pair<string, string>(name[c], value[c]));  //一一对应入map
        
        return m;
     }
    else if((int)str.find('?') > 0)  //只有一个参数
    {
      
        name.push_back(fake.substr(fake.find('?')+1, fake.find('=') - fake.find('?')-1));
        value.push_back(fake.substr(fake.find('=')+1, fake.size() - fake.find('=')-1));

        m.insert(pair<std::string, std::string>(name[0], value[0]));  

        return m;
    }
    else  
    {
        return m;
    } 
}

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root)
{
}

void request_handler::handle_request_POST(const request& req, reply& rep)
{
    std::cout << "post +1" << std::endl;
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  if (request_path == "/mission/group/")
  {
      //发布任务会给client返回文件，写好反馈文件
      writeFileJson_mission_callback();

      request_path += "demo.json";    //约定好路径

      rep.status = reply::ok;

      std::cout << "request body：" << req.body << std::endl;
      writeFileJson_mission_allo(req.body);   //请求体已经变成字符串

      std::string full_path = doc_root_ + request_path;
      std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);    //只读二进制,改成其他的影响吗？    二进制实测只在windows上影响换行符

      if (!is)
      {
          rep = reply::stock_reply(reply::not_found);
          return;
      }

      char buf[512];
      while (is.read(buf, sizeof(buf)).gcount() > 0)
          rep.content.append(buf, is.gcount());
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = mime_types::extension_to_type(" ");

      return;
  }



  if (request_path == "/api/robots/control/")
  {
      Datatrans datatrans;
                               //数传走控制指令,没有反馈文件
      datatrans.remotecontrol();
      std::cout << "wow" << std::endl;
      rep.status = reply::ok;
      rep.headers.resize(1);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());    //仅有返回头，正常运行
      return;
  }



  //以下为测试用

  // If path ends in slash (i.e. is a directory) then add 
  if (request_path[request_path.size() - 1] == '/')
  {
      request_path += "json.json";
  }

  // Determine the file extension.
  /*std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
      extension = request_path.substr(last_dot_pos + 1);
  }*/


   //Fill out the reply to be sent to the client.
   rep.status = reply::ok;

  // 5 发送返回数据      一共两个post指令，远程控制和下发任务(return)
  //返回响应头
  rep.headers.resize(1);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());

  //返回响应体，处理接收的数据为req.body
  cout <<"request body："<< req.body << endl;       
  //string sendStr = "huanhuncao";
  //rep.content.append(sendStr.c_str(), sendStr.length());
  // 
  //str转json并保存文件至本地 
 // writeFileJson_mission_allo(req.body);   //请求体已经变成字符串


  
   

  // Open the file to send back.
  std::string full_path = doc_root_ + request_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
      rep = reply::stock_reply(reply::not_found);
      return;
  }
  //
  rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
      rep.content.append(buf, is.gcount());
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = mime_types::extension_to_type("json");   

}

void request_handler::handle_request_GET(const request& req, reply& rep)
{
    std::cout << "get +1" << std::endl;
    // Decode url to path.
    std::string request_path;
    if (!url_decode(req.uri, request_path))
    {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/'
        || request_path.find("..") != std::string::npos)
    {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // 有传参
     std::map<std::string, std::string > pv_map = record2map(request_path);
     printMap(pv_map);

     //去除参数
     request_path = request_path.substr(0, request_path.find('?'));

    //处理表中参数  ,   构造json以供发送
     if (request_path == "/api/robots/")
     {
         writeFileJson_robots(pv_map);
         
     }
    
     if (request_path == "/api/missions/")
     {
         writeFileJson_missions(pv_map);
        
     }

     if (request_path == "/api/maps/")
     {
         writeFileJson_maps(pv_map);
         
     }
     //清空参数表
     pv_map.erase(pv_map.begin(), pv_map.end());
     pv_map.clear();


    if (request_path[request_path.size() - 1] == '/')
    {
        request_path += "demo.json";               //在不同文件夹下的demo.json即可
    }
    // 确定文件名
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }

    // Open the file to send back.
    std::string full_path = doc_root_ + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);     //二进制读入流
    if (!is)
    {
        rep = reply::stock_reply(reply::not_found);
        return;
    }
    // Fill out the reply to be sent to the client.
    rep.status = reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)      //从输入流中提取字符
        rep.content.append(buf, is.gcount());        //添加字符
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = mime_types::extension_to_type(extension);
}


bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // namespace server
} // namespace http
