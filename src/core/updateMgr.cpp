//
//  updateMgr.cpp
//  mkrkr
//
//  Created by tony on 2018/7/4.
//

#include "updateMgr.hpp"

void CBMgr::update(unsigned int cur_tick)
{
    for (auto i = v.begin(); i != v.end(); ++i)
    {
        (*i)(cur_tick);
    }
    
    for (auto i = v2.begin(); i != v2.end(); ++i)
    {
        (*i)->update(cur_tick);
    }
    
}
