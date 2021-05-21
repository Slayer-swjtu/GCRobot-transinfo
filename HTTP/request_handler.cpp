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


std::string param ; //�洢�����param
std::string value ;



void printMap(std::map<std::string, std::string>& m)
{
    for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++)
    {
        std::cout << "key = " << it->first << " value = " << it->second << std::endl;
    }
    std::cout << std::endl;
}

std::map<std::string ,std::string>  record2map(std::string& str)    //��ͼ����url�����еļ�ֵ�ԣ����浽map
{
    std::string fake = str;     //��������
    std::vector<std::string> name;    
    std::vector<std::string> value;
    std::map<std::string, std::string> m;

    int flag = str.find('&');  //������ʽò�Ƹ����ף���ϧ��û��
    if (flag > 0) // �������
    {
        //��¼һ��
        name.push_back(fake.substr(str.find('?')+1  , str.find('=') - str.find('?')-1));  //vector<string> ��ֵ��ʽ��push_back,name[i]ֻ����print
        value.push_back(fake.substr(str.find('=')+1  , str.find('&') - str.find('=')-1)); 

        fake = fake.substr(str.find('&') + 1);

        //flag = fake.find('&');  //һ��Ҫת��Ϊint����Ȼfind���ص���һ��size_t�޷���������ѭ��
        while ((int)fake.find('&') > 0)   //url�Ӵ��ﻹ����&
        {
            name.push_back(fake.substr(0, fake.find('=')));
            value.push_back(fake.substr(fake.find('=')+1, fake.find('&') - fake.find('=')-1));
            fake = fake.substr(fake.find('&') + 1);
            
        }

        //url���ʷ��Ѿ���&�Ӵ�
        name.push_back(fake.substr(0, fake.find('=')));
        value.push_back(fake.substr(fake.find('=')+1, fake.size() - fake.find('=')-1)) ;    

        for (int c = 0; c < name.size(); c++) 
            m.insert(pair<string, string>(name[c], value[c]));  //һһ��Ӧ��map
        
        return m;
     }
    else if((int)str.find('?') > 0)  //ֻ��һ������
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
      //����������client�����ļ���д�÷����ļ�
      writeFileJson_mission_callback();

      request_path += "demo.json";    //Լ����·��

      rep.status = reply::ok;

      std::cout << "request body��" << req.body << std::endl;
      writeFileJson_mission_allo(req.body);   //�������Ѿ�����ַ���

      std::string full_path = doc_root_ + request_path;
      std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);    //ֻ��������,�ĳ�������Ӱ����    ������ʵ��ֻ��windows��Ӱ�컻�з�

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
                               //�����߿���ָ��,û�з����ļ�
      datatrans.remotecontrol();
      std::cout << "wow" << std::endl;
      rep.status = reply::ok;
      rep.headers.resize(1);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());    //���з���ͷ����������
      return;
  }



  //����Ϊ������

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

  // 5 ���ͷ�������      һ������postָ�Զ�̿��ƺ��·�����(return)
  //������Ӧͷ
  rep.headers.resize(1);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());

  //������Ӧ�壬������յ�����Ϊreq.body
  cout <<"request body��"<< req.body << endl;       
  //string sendStr = "huanhuncao";
  //rep.content.append(sendStr.c_str(), sendStr.length());
  // 
  //strתjson�������ļ������� 
 // writeFileJson_mission_allo(req.body);   //�������Ѿ�����ַ���


  
   

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

    // �д���
     std::map<std::string, std::string > pv_map = record2map(request_path);
     printMap(pv_map);

     //ȥ������
     request_path = request_path.substr(0, request_path.find('?'));

    //������в���  ,   ����json�Թ�����
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
     //��ղ�����
     pv_map.erase(pv_map.begin(), pv_map.end());
     pv_map.clear();


    if (request_path[request_path.size() - 1] == '/')
    {
        request_path += "demo.json";               //�ڲ�ͬ�ļ����µ�demo.json����
    }
    // ȷ���ļ���
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }

    // Open the file to send back.
    std::string full_path = doc_root_ + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);     //�����ƶ�����
    if (!is)
    {
        rep = reply::stock_reply(reply::not_found);
        return;
    }
    // Fill out the reply to be sent to the client.
    rep.status = reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)      //������������ȡ�ַ�
        rep.content.append(buf, is.gcount());        //����ַ�
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
