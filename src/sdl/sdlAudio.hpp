#ifndef _SDL_AUDIO_H_
#define _SDL_AUDIO_H_
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include "ffStream.hpp"
#include "AL/al.h"

class audioDevice;
class pcmBuffer;

typedef std::shared_ptr<ffStream> tffStreamPtr;
typedef std::shared_ptr<audioDevice> tDevPtr;
typedef tDevPtr tPtrDev;
typedef std::shared_ptr<pcmBuffer> tPcmPtr;
typedef tPcmPtr tPtrPcm;

struct AudioFormat
{
  // SDL_AudioFormat format; //pcm is AUDIO_S16SYS
  ALenum format; //al format,like AL_FORMAT_STEREO16
  uint16_t  nChannels;
  uint16_t wBitsPerSample; //8 or 16
  uint16_t wFormatTag;
  uint32_t nSamplesPerSec; //freq
  //If wFormatTag is WAVE_FORMAT_PCM, 
  //nBlockAlign must equal (nChannels × wBitsPerSample) / 8
  uint32_t nBlockAlign;
  uint32_t nAvgBytesPerSec; 

};

class BaseAudioPlayer
{
public:
	bool is_enable;
	bool is_loop;
	enum PLAYSTATUTS
	{
		playing,
		stoped,
		stopping,
		paused,
		init,
	} status;

	BaseAudioPlayer(bool _loop = false)
	{
		is_enable = true;
		is_loop = _loop;
		status = init;
	}
	virtual ~BaseAudioPlayer(){}
	void set_loop(bool s) {is_loop = s;}

	virtual void play() = 0;
	virtual void pause()= 0;
	virtual void stop()= 0;
	
	virtual void enable()= 0;
	virtual void disable()= 0;
	virtual void update()= 0;
};

typedef std::shared_ptr<BaseAudioPlayer> tPtrBasePlayer;

class BaseAudioDevice
{
public:

	std::vector<tPtrBasePlayer> players;
	BaseAudioDevice(){}
	virtual ~BaseAudioDevice(){}

	virtual void play() =0;
	virtual void pause() =0;
	virtual void stop() = 0;

	virtual void enable_player(tPtrBasePlayer p) = 0;
	virtual void disable_player(tPtrBasePlayer p) = 0;

	virtual void on_enable(tPtrBasePlayer p) = 0;
	virtual void on_disable(tPtrBasePlayer p) = 0;
	virtual void on_stop(tPtrBasePlayer p) = 0;

	virtual void add_player(tPtrBasePlayer p) = 0;
	virtual void rm_player(tPtrBasePlayer p) = 0;
};

typedef std::shared_ptr<BaseAudioDevice> tPtrBaseDevice;




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