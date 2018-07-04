//
//  updateMgr.hpp
//  mkrkr
//
//  Created by tony on 2018/7/4.
//

#ifndef updateMgr_hpp
#define updateMgr_hpp

#include <stdio.h>
#include <vector>

class UpdateObj
{
public:
    virtual void update(unsigned int value)=0;
};

typedef void (*UpdateFunc)(unsigned int interval);
typedef void (UpdateObj::*ObjUpdateFunc)(unsigned int interval);


class CBMgr
{
    std::vector<UpdateFunc> v;
    std::vector<UpdateObj*> v2;
    
public:
    void add_callback(UpdateFunc f)
    {
        v.push_back(f);
    }
    void add_callback2(UpdateObj* p)
    {
        v2.push_back(p);
    }
    void update(unsigned int cur_tick);
};


void regist_update(UpdateFunc f);
void regist_objupdate(UpdateObj* p);

#endif /* updateMgr_hpp */
