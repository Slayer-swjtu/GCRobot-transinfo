
#include "cstjson.hpp"

using namespace std;



void writeFileJson_maps(map<string,string> m)      //因为有参数，所以最好在基站上构造？
{
	map<string, string>::iterator pos = m.find("used");
	if (pos != m.end())  
	{
		if ((*pos).second == "true")
		{
			//
		}
	}
	else
	{
		//
	}
}





void writeFileJson_missions(map<string, string> m)
{
	//
}

void writeFileJson_robots(map<string, string> m)
{
	//
}


void writeFileJson_mission_callback()   //通过数传返回数据，带有收到任务的信息
{
	std::string filePath = "F:/mission/group/res.json";
	std::ofstream fout;
	fout.open(filePath.c_str());
	assert(fout.is_open());
	 
	int code = 0;   //需要机体信息

	std::string reason ;   //需要机体信息


	Json::Value root;        // 根节点
	root["code"] = code;
	root["reason"] = reason;

	std::cout << root.toStyledString() << std::endl;
	fout << root.toStyledString() << std::endl;
	fout.close();
}

//保存json的文件 e.g.

void writeFileJson_mission_allo(std::string str)
{
	string iok = "23";
	int tip = 23;
	std::string filePath = "F:/demo.json";
	std::ofstream fout;
	fout.open(filePath.c_str());
	assert(fout.is_open());



	Json::Value root;        // 根节点
	root["isBankCardRecogOK"] = iok; // 根节点
	root["tip"] = tip;        // 根节点

	Json::Value array;   // 创建数组
	for (int i = 0; i < 3; i++)
	{
		Json::Value item;
		item["ok"] = " ";
		item["cancel"] = " ";
		item["loginsuccess"] = " ";
		item["loginfail"] = " ";
		array.append(item); // append()以数组的形式添加
	}
	root["array"] = array;
	std::string out = root.toStyledString();
	std::cout << out << std::endl;
	fout << out << std::endl;

}