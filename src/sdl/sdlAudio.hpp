#ifndef _SDL_AUDIO_H_
#define _SDL_AUDIO_H_
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include "ffStream.hpp"

class audioDevice;
class pcmBuffer;

typedef std::shared_ptr<ffStream> tffStreamPtr;
typedef std::shared_ptr<audioDevice> tDevPtr;
typedef std::shared_ptr<pcmBuffer> tPcmPtr;

struct AudioFormat
{
  SDL_AudioFormat format; //pcm is AUDIO_S16SYS
  uint16_t  nChannels;
  uint16_t wBitsPerSample; //8 or 16
  uint16_t wFormatTag;
  uint32_t nSamplesPerSec; //freq
  //If wFormatTag is WAVE_FORMAT_PCM, 
  //nBlockAlign must equal (nChannels × wBitsPerSample) / 8
  uint32_t nBlockAlign;
  uint32_t nAvgBytesPerSec; 

};


class DataBuffer
{
public:
	std::vector<uint8_t> v;
	int len;
	int rpos;
	DataBuffer(){
		v.resize(4096);
		len = 0;
		rpos = 0;
	}
	uint8_t* get_data()
	{
		return v.data();
		// return &(*(v.begin()));
	}
	uint8_t* get_tail()
	{
		return v.data()+len;
		// return &(*(v.begin()+len));
	}
	void push(uint8_t* data,int size)
	{
		//fix using size() better than capacity()!!!
		if(len + size > v.size()) 
		{
			v.resize(len + size);
		}
		memcpy(get_tail(),data,size);
		len += size;
	}
	int read(uint8_t** data,int n)
	{
		if(rpos == len)
		{
			return 0;
		}
		int read_len = n;
		if( n > len - rpos)
		{
			read_len = len - rpos;
		}
		*data = get_data() + rpos;
		rpos += read_len;
		return read_len;
	}

	//read all;
	int read(uint8_t** data)
	{
		int read_len = len - rpos;
		*data = get_data() + rpos;
		rpos = len;
		return len;
	}
	void seek(int pos)
	{
		if(pos>=len)
			rpos = len;
		else
			rpos = pos;
	}
};

class pcmBuffer:
public std::enable_shared_from_this<pcmBuffer>
{
public:
	DataBuffer db;
	uint32_t len;
	int pos;
	bool is_enable;
	bool is_loop;
	tDevPtr pDev;
	AudioFormat format;

	pcmBuffer(){
		_init();
	}
	
	pcmBuffer(uint8_t* _buf,uint32_t _len)
	{
		_init();
		db.push(_buf,_len);
		len = _len;
	}
	void _init()
	{
		len = 0;
		pos = 0;
		is_enable = false;
		is_loop = false;
		pDev = nullptr;
	}
	void read_from_ffstream(ffStream& fs);
	void enable();
	void disable();
	void set_loop(bool s){is_loop = s;}
	int get_data(uint8_t** _data, int _len);
	void set_dev(tDevPtr p_dev);
	void reset()
	{
		db.seek(0);
	}
	void play(bool _loop);
	void stop();
	int SetFormat(const AudioFormat* fm);
	int GetFormat(AudioFormat* fm);


};

class audioDevice:
public std::enable_shared_from_this<audioDevice>
{
public:
	

	SDL_AudioSpec want_spec;
	SDL_AudioSpec get_spec;
	bool is_playing;
	int nplaying;
	bool is_init_spec;
	// uint8_t* data;
	std::vector<tPcmPtr > pcms;
	SDL_AudioDeviceID dev;
	int init_spec(int freq,uint8_t channels);
	static void SDLCALL
	audio_cb(void* userdata, Uint8* stream,int len);
	

	audioDevice()
	{
		pcms.clear();
		nplaying = 0;
		is_playing = false;
		is_init_spec = false;
	}
	void dump_info();
	void play();
	void on_enable(tPcmPtr pcm);
	void on_disable(tPcmPtr pcm);
	void disable_all();
	void stop();
	void add_pcm(tPcmPtr& pcm);
	void rm_pcm(tPcmPtr& pcm);
	void create_pcm_buffer(tPcmPtr& pcm);

};





#endif