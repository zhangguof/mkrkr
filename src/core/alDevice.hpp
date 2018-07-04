#ifndef _AL_DEVICE_H_
#define _AL_DEVICE_H_
#include "SDL.h"
#include "ffStream.hpp"
#include "sdlAudio.hpp"


#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <atomic>

#include "alObjects.hpp"

// extern uint32_t GetTickCount();



//wrap sources
//one palyer<--> one sources
//one source hold n buffers
#define UPDATE_TIME (200) //ms
#define SOURCE_BUFFER_NUM (4)

class LoopBuffer
{
public:
	//std::vector<uint8_t> v;
	uint8_t* _data;
	int r_pos;
	int w_pos;
	// int readable_len;
	std::atomic_int readable_len;

	// int writeable_len;
	int len;
	// op by unit size.
	LoopBuffer(int size)
	{
		len = size;
		_data = new uint8_t[size];
		reset();
	}
	void reset()
	{
		r_pos  = w_pos = 0;
		readable_len = 0;
		// writeable_len = len;;
	}
	bool is_writeable()
	{
		return len - readable_len > 0;
	}
	bool is_readable()
	{
		return readable_len > 0;
	}
	bool lock(int pos,void** p1, int bytes)
	{
		//get write data;
		if(pos!=w_pos)
		{
			SDL_Log("(=====)Error:pos:%d,w_pos:%d:len:%d,bytes:%d\n",pos,w_pos,len,bytes);
		}
		// assert(pos == w_pos);
		int n_pos = w_pos + bytes;
		if(len - readable_len < bytes)
			return false;
		
		if(n_pos< len)
		{
			*p1 = _data+w_pos;
		}
		else
		{
			assert(bytes == len - w_pos);
			*p1 = _data+w_pos;
		}
		return true;
	}
	bool unlock(void*p1, int bytes)
	{
		assert(p1 == _data + w_pos);
		int n_pos = w_pos + bytes;
		if(n_pos < len)
		{
			w_pos = n_pos;
		}
		else
		{
			w_pos = 0;
		}
		// writeable_len -= bytes;
		readable_len += bytes;
		return true;
	}
	bool write(void* p1, int bytes)
	{
		if(len - readable_len < bytes)
			return false;
		int n_pos = w_pos + bytes;
		if(n_pos<len)
		{
			memcpy(_data+w_pos,p1,bytes);
			w_pos = n_pos;

		}
		else
		{
			//bytes <= len - w_pos
			assert(bytes == len - w_pos);
			int remain_len = len - w_pos;
			memcpy(_data+w_pos,p1,remain_len);
			// if(bytes - remain_len > 0)
				// memcpy(_data,p1+remain_len,bytes - remain_len);
			// w_pos = bytes - remain_len;
			w_pos = 0;
		}
		// writeable_len -= bytes;
		readable_len += bytes;
		return true;
	}
	bool read(uint8_t** p1,int bytes)
	{
		if(readable_len<bytes)
			return false;
		int n_pos = r_pos + bytes;
		if(n_pos < len)
		{
			// memcpy(p1,_data+r_pos,bytes);
			*p1 = _data + r_pos;
			r_pos = n_pos;
		}
		else
		{
			// bytes<= len - r_pos;
			assert(bytes == len - r_pos);

			int remain_len = len - r_pos;
			// memcpy(p1,_data+r_pos,remain_len);
			*p1 = _data + r_pos;
			// if(bytes - remain_len > 0)
				// memcpy(p1 + remain_len, _data, bytes - remain_len);
			r_pos = bytes - remain_len;
		}
		// writeable_len += bytes;
		readable_len -= bytes;
		return true;
	}

	~LoopBuffer()
	{
		delete[] _data;
	}

};

inline int get_buffer_data_size(
	int channles,
	int freq,
	int bits,
	int update_time_ms)
{
	return bits/8 * channles * freq * update_time_ms / 1000;
}

class AudioPlayer: public BaseAudioPlayer,
public std::enable_shared_from_this<AudioPlayer>
{
public:
	enum BufferStatus
	{
		Queued,
		Unqueued
	};
	BufferStatus* buffer_status;


public:
//openAL interface
	std::shared_ptr<ALSources> p_src;
	std::shared_ptr<ALBuffers> p_bufs;
	std::shared_ptr<DataBuffer> pdb;

	
//simulate DirectSound Buffer API
	std::shared_ptr<LoopBuffer> plb;
	int nUnits; // n ALBuffers.
	int nSize;  //Loop buffer size.
	int nUnitSize;

	bool has_lock;
	AudioFormat format;

	//for test
	std::shared_ptr<ffStream> ff_stream;
	int freq;
	

//AL_SOURCE_TYPE == AL_STATIC?
	bool is_static_type;

	tPtrBaseDevice pdev;
	int n_buffer;
	int n_queued_buffer;


	AudioPlayer(int nbuffer=SOURCE_BUFFER_NUM,
		bool _loop = false);
	~AudioPlayer(){
		printf("====Release AudioPlayer!!!\n");
	}
	void Release(){}
	int buffer_data_one(int buf_idx,int update_time_ms);
	int buffer_data_all(int buf_idx);
	int buffer_data_unit(int buf_idx,int bytes);
	
	void unqueue_buffers();
	void queue_buffers(ALuint* buffer_ids, int n);
	void update_uint();
	void update_units();
	
	//read bytes from data
	void update();

	void read_from_ffstream(std::shared_ptr<ffStream>& ff,
		bool is_static=false);

	void play();
	void pause();
	void stop();
	void do_stop();
	void enable();
	void disable();

	void set_dev(tPtrBaseDevice p)
	{
		pdev = p;
	}
	int SetFormat(AudioFormat& _format)
	{
		format = _format;
		return 0;
	}
	int GetFormat(AudioFormat& _format)
	{
		_format = format;
		return 0;
	}
	void reset()
	{
		//do reset here?
		if(pdb)
			pdb->reset();
		if(plb)
			plb->reset();
		
	}
	void seek(int n)
	{
		assert(false);
		// pdb->seek(n);
		// if(plb)
		// 	plb->seek(n);
	}
	void new_loop_buffer(int n,int size)
	{
		plb = std::make_shared<LoopBuffer>(size);
		nUnits = n;
		nSize =  size;
		nUnitSize = size / n;
	}
	bool lock(int pos,int bytes, void** p1,int* b1);
	bool unlock(void* p1, int b1);
	bool GetCurrentPosition(int &play_pos, int &write_pos)
	{
		play_pos = plb->r_pos;
		write_pos = plb->w_pos;
		return true;	
	}
	void SetVolume(int v)
	{
		float vol = v/10000.0;
		if(vol > 1.0f)
		{
			vol = 1.0f;
		}
		if(vol < 0.0f) vol = 0.0f;
		p_src->set_gain(vol);
	}
	void SetFrequency(int freq)
	{
		// p_src->set_freq(freq);
		this->freq = freq;
	}
	bool writeable()
	{
		if(plb)
		{
			return plb->is_writeable();
		}
		return false;
	}
//buffer status
	void reset_buffer_status()
	{
		for(int i=0;i<n_buffer;++i)
		{
			buffer_status[i]=Unqueued;
		}	
	}
	void get_free_buffers_idx(uint32_t* buffer_idx,int& n)
	{
		n = 0;
		for(int i=0;i<n_buffer;++i)
		{
			if(buffer_status[i]==Unqueued)
			{
				buffer_idx[n++] = i;
			}
		}
	}
	void change_buffers_status(uint32_t* buffer_ids, int n, BufferStatus s)
	{
		for(int i=0;i<n;++i)
		{
			int idx = p_bufs->get_idx(buffer_ids[i]);
			buffer_status[idx] = s;
		}
	}
};

extern int init_al(ALCdevice** pdev);
class ALAuidoDevice;
typedef std::shared_ptr<ALAuidoDevice> tPtrAuDev;

class ALAuidoDevice: public BaseAudioDevice,
public std::enable_shared_from_this<ALAuidoDevice>
{
public:
	
	typedef void (BaseAudioPlayer::*PlayerOP)();
public:
	std::vector<tPtrBasePlayer> players;
	ALCdevice* p_al_dev;

	AudioFormat format;

	static tPtrAuDev p_inst;// = nullptr;

	static tPtrAuDev get_inst()
	{
		//c++0x
		static ALAuidoDevice _inst; //= std::make_shared<ALAuidoDevice>();
		if(p_inst == nullptr)
		{
			p_inst = std::shared_ptr<ALAuidoDevice>(&_inst);
		}
		return p_inst;
	}
	void foreach_objs(PlayerOP op)
	{
		for(auto& it:players)
		{
			if(it->is_enable)
			{
				((*it).*op)();
			}
		}
	}
	void play();
	void stop();
	void on_stop(tPtrBasePlayer p);

	void pause();
	//return next wake
	uint32_t update();

	void enable_player(tPtrBasePlayer p)
	{
		p->enable();
	}
	void disable_player(tPtrBasePlayer p)
	{
		p->disable();
	}

	void on_enable(tPtrBasePlayer p){}
	void on_disable(tPtrBasePlayer p){}

	void add_player(tPtrBasePlayer p);
	void rm_player(tPtrBasePlayer p);


	int SetFormat(const AudioFormat* fm);
	int GetFormat(AudioFormat* fm);

//use loop  buffer
	//simulate directsound buffer

	int CreateSoundBuffer(std::shared_ptr<AudioPlayer>& pbuf,int n,int size,AudioFormat& _format)
	{
		pbuf = std::make_shared<AudioPlayer>(n);
		pbuf->new_loop_buffer(n,size);
		pbuf->SetFormat(_format);
		// pbuf->set_loop(true);

		add_player(pbuf);
		return 0;
	}


private:
	ALAuidoDevice();
public:
	~ALAuidoDevice();

};


#endif
