#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vector>
#include <stdlib.h>

#include "load_infos.h"

const int max_buf_size = 1024 * 4;
tPDataInfo load_infos(std::wstring json_file)
{
	FILE* f = NULL;
	auto err = ::_wfopen_s(&f, json_file.c_str(),L"rb");
	char buf[max_buf_size];
	if(!f)
	{
		strerror_s(buf,errno);
		printf("open file error:%ls:%d:==%s\n",json_file.c_str(),errno,buf);
		return nullptr;
	}
	int read_size = ::fread(buf,sizeof(char),max_buf_size,f);
	printf("read json size:%d\n",read_size);
	buf[read_size] = '\0';

	auto pinfo = std::make_shared<DataInfo>();

	namespace pt = boost::property_tree;
	std::string json_str(buf,read_size);
	std::istringstream is(json_str);
	pt::ptree root;
	pt::read_json(is,root);
	
	for(pt::ptree::value_type &xp3_file: root.get_child("xp3_files"))
	{
		auto item = xp3_file.second.get_child("");//["xx.xp3",0]
		auto p = item.begin(); 
		std::string v1 = (p++)->second.get<std::string>("");
		int v2 = (p++)->second.get<int>("");
		std::string v3 = "";
		if(p!=item.end())
		{
			v3 = (p++)->second.get<std::string>("");
		}
		pinfo->push(v1,v2,v3);
	}

	//pinfo->dump();
	return pinfo;
}
