#include "shift_jis_tables.h"

bool get_shiftjis_code(const char* &in,wchar_t* out)
{
	uint8_t b1 = *in++;
	//0x20~0x7E 0xA1~0xDF is one byte
	if(b1 < 0x80 || (b1>0xA0 && b1<=0xDF))
	{
		auto find_it = shift_jis_tables.find(b1);
		if(find_it!=shift_jis_tables.end())
		{
			if(out)
				*out = find_it->second;
			return true;
		}
	}
	uint8_t b2 = *in++;
	uint16_t b = (b1<<8)|b2;
	auto find_it = shift_jis_tables.find(b);
	if(find_it!=shift_jis_tables.end())
	{
		if(out)
			*out = find_it->second;
		return true;
	}
	
	return false;
}

// uint16_t get_shiftjis_code(uint8_t* &in)
// {
//     int idx = 0;
//     uint8_t b1 = *in++;
//     uint8_t b2 = 0;
//     const uint8_t* g1_r = shift_jis_g1_range;
//     const uint8_t* g2_r = shift_jis_g2_range;
//     if(b1>=g1_r[0] && b1<=g1_r[1])
//     {
//     	// in++;
//         return shift_jis_tables[b1 - g1_r[0]];
//     }
//     if(b1>=g1_r[2] && b1<=g1_r[3])
//     {
//         b2 = *in++;
//         int offest = (b1-g1_r[2]) * shift_jis_g2_size + (b2-g2_r[0]);
//         if(b2>=g2_r[0] && b2<=g2_r[1])
//         {
//             return shift_jis_tables[jis_p2_start + offest];
//         }
//         else if(b2>=g2_r[2] && b2<=g2_r[3])
//         {
//             return shift_jis_tables[jis_p2_start + offest - 1];
//         }
//         return 0;//error
//     }
//     if(b1>=g1_r[4] && b1<=g1_r[5])
//     {
//         return shift_jis_tables[jis_p3_start + b1 - g1_r[4]];
//     }
//     if(b1>=g1_r[6] && b1<=g1_r[7])
//     {
//         b2 = *in++;
//         int offest = (b1-g1_r[6]) * shift_jis_g2_size + (b2-g2_r[0]);
//         if(b2>=g2_r[0] && b2<=g2_r[1])
//         {
//             return shift_jis_tables[jis_p4_start + offest];
//         }
//         else if(b2>=g2_r[2] && b2<=g2_r[3])
//         {
//             return shift_jis_tables[jis_p4_start + offest - 1];
//         }
//         return 0;//error
//     }
//     return 0;
// }

