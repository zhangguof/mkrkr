#ifndef _LOAD_INFOS_H_
#define  _LOAD_INFOS_H_
#include <string>

class DataInfo
{
public:
	struct DataItem
	{
		std::string name;
		int idx;
		std::string out_path;
		DataItem(std::string _n,int _i,std::string _o):name(_n),idx(_i),out_path(_o){}
	};
	typedef std::shared_ptr<DataItem> tPDataItem;
public:

	int count;
	std::vector<tPDataItem> infos;
	DataInfo(){count = 0;infos.clear();}
	void push(std::string name, int idx,std::string out_path="")
	{
		auto item = std::make_shared<DataItem>(name,idx,out_path);
		infos.push_back(item);
		count++;
	}
	int get_count(){return count;}
	tPDataItem get(int idx)
	{
		assert(idx < count);
		return infos[idx];
	}

	void dump()
	{
		for(auto &p:infos)
		{
			printf("==%s:%d:%s\n",p->name.c_str(),p->idx,p->out_path.c_str());
		}
	}
};
typedef std::shared_ptr<DataInfo> tPDataInfo;

extern tPDataInfo load_infos(std::wstring json_file);


#endif