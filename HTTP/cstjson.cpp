
#include "cstjson.hpp"

using namespace std;



void writeFileJson_maps(map<string,string> m)      //��Ϊ�в�������������ڻ�վ�Ϲ��죿
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


void writeFileJson_mission_callback()   //ͨ�������������ݣ������յ��������Ϣ
{
	std::string filePath = "F:/mission/group/res.json";
	std::ofstream fout;
	fout.open(filePath.c_str());
	assert(fout.is_open());
	 
	int code = 0;   //��Ҫ������Ϣ

	std::string reason ;   //��Ҫ������Ϣ


	Json::Value root;        // ���ڵ�
	root["code"] = code;
	root["reason"] = reason;

	std::cout << root.toStyledString() << std::endl;
	fout << root.toStyledString() << std::endl;
	fout.close();
}

//����json���ļ� e.g.

void writeFileJson_mission_allo(std::string str)
{
	string iok = "23";
	int tip = 23;
	std::string filePath = "F:/demo.json";
	std::ofstream fout;
	fout.open(filePath.c_str());
	assert(fout.is_open());



	Json::Value root;        // ���ڵ�
	root["isBankCardRecogOK"] = iok; // ���ڵ�
	root["tip"] = tip;        // ���ڵ�

	Json::Value array;   // ��������
	for (int i = 0; i < 3; i++)
	{
		Json::Value item;
		item["ok"] = " ";
		item["cancel"] = " ";
		item["loginsuccess"] = " ";
		item["loginfail"] = " ";
		array.append(item); // append()���������ʽ���
	}
	root["array"] = array;
	std::string out = root.toStyledString();
	std::cout << out << std::endl;
	fout << out << std::endl;

}