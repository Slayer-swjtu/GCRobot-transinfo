#pragma once
//数传相关的

class Datatrans 
{
	public:

		void openfile();   //获取机器人上传的信息

		void remotecontrol();  //控制指令

		void rtopenfile();  //高速数传给websocket写好文件

	

};