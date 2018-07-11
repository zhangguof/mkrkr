#ifndef _SHITF_JIS_TABLE_H_
#define _SHITF_JIS_TABLE_H_
#include <unordered_map>
extern std::unordered_map<uint16_t,uint16_t> shift_jis_tables;
// extern const uint16_t shift_jis_tables[];
// extern const uint8_t shift_jis_g1_range[];
// extern const uint8_t shift_jis_g2_range[];
// extern const int shift_jis_g2_size;
// extern const int jis_p1_start;
// extern const int jis_p2_start;
// extern const int jis_p3_start;
// extern const int jis_p4_start;
extern bool get_shiftjis_code(const char* &in,wchar_t* out=NULL);


#endif