
//各类json的构造函数

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


void writeFileJson_robots(std::map<std::string,std::string> m);

void writeFileJson_maps(std::map<std::string, std::string> m);

void writeFileJson_missions(std::map<std::string, std::string> m);

void writeFileJson_control(std::string str);

void writeFileJson_mission_allo(std::string str);

void writeFileJson_mission_callback();