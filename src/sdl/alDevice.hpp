#ifndef _AL_DEVICE_H_
#define _AL_DEVICE_H_
#include "SDL.h"
#include "ffStream.hpp"
#include "sdlAudio.hpp"

#include "AL/alc.h"
#include "AL/al.h"
#include "AL/alext.h"
#include "AL/alut.h"

#include <map>
#include <string>
#include <algorithm>
#include <vector>

extern int al_check_error(const char* filename,const char* fun_name,int line);

#define CHECK_ERROR al_check_error(__FILE__,__FUNCTION__,__LINE__)

#define AL_DELC(alname,name,type) \
int name(ALenum param, type a, int idx=0) \
{ \
	alname(param,a); \
	return CHECK_ERROR; \
}

#define AL_PROP(name,prop,op,type) \
int op##_##name(type a) \
{ \
op(prop,a); \
return CHECK_ERROR; \
}

#define AL_PROP2(name,prop,op1,type1,op2,type2) \
AL_PROP(name,prop,op1,type1) \
AL_PROP(name,prop,op2,type2)


//objects define
#define AL_DELC_OBJS(alname,name,type) \
int name(ALenum param,type a, int idx=0) \
{ \
alname(_objs[idx],param,a); \
return CHECK_ERROR; \
}

#define AL_PROP_OBJS(name,prop,op,type) \
int op##_##name(type a,int idx=0) \
{ \
op(prop,a,idx); \
return CHECK_ERROR; \
}

#define AL_PROP_OBJS2(name,prop,op1,type1,op2,type2) \
AL_PROP_OBJS(name,prop,op1,type1) \
AL_PROP_OBJS(name,prop,op2,type2)

#define AL_NEW_PROP(alname,t) \
g_al_props[alname] = Prop(alname,t);


#define AL_PROP_VAL_INT   (0x01)
#define AL_PROP_VAL_FLOAT (0x02)

#define AL_PROP_VAL_INTV  (0x11)
#define AL_PROP_VAL_FLOATV (0x12)

#define AL_OBJE_LISTENER (0x01)
#define AL_OBJE_SROUCES (0x02)
#define AL_OBJE_BUFFERS (0x04)



struct ALProp
{
	union Val{
		int i;
		float f;
		int iv[3];
		float fv[3];
	};
	uint16_t obj_type;
	std::string name;
	ALenum al_name;
	uint16_t prop_type;

	ALProp(ALenum al_n,uint16_t t,const char* _name,uint16_t _t)
	{
		// const char* p = alGetString(al_n);
		// CHECK_ERROR;
		name = _name;
		al_name = al_n;
		prop_type = t;
		obj_type = _t;
	}
	ALProp(ALProp& p)
	{
		name = p.name;
		al_name = p.al_name;
		prop_type = p.prop_type;
		obj_type = p.obj_type;
	}
};



extern std::map<ALenum,std::shared_ptr<ALProp> > g_al_props;
extern std::vector<ALenum> g_al_listener_props;
extern std::vector<ALenum> g_al_buffers_props;
extern std::vector<ALenum> g_al_sources_props;

extern void regist_al_prop(ALenum al_name,uint16_t t,const char* name,uint16_t _type);

#define REGIST_AL_PROP(al_name,t,_type) \
regist_al_prop(al_name,t,#al_name,_type);


class ALObjects
{

public:
	uint16_t obj_type;
	ALObjects(uint16_t _t):obj_type(_t){}

	virtual int get(ALenum a, int* p, int idx) 	  	  = 0;
	virtual int get(ALenum a, float* p, int idx ) 	  = 0;
	virtual int getv(ALenum a, int* p, int idx) 	  = 0;
	virtual int getv(ALenum a, float* p, int idx ) 	  = 0;

	virtual int set(ALenum a, int p, int idx) 	  	  = 0;
	virtual int set(ALenum a, float p, int idx ) 	  = 0;
	virtual int setv(ALenum a, int* p, int idx) 	  = 0;
	virtual int setv(ALenum a, float* p, int idx ) 	  = 0;

	void dump_prop(const ALenum al_name,int idx=0)
	{
		ALProp::Val val;
		auto it = g_al_props.find(al_name);
		if(it!=g_al_props.end())
		{
			auto p = it->second;
			switch(p->prop_type)
			{
				case AL_PROP_VAL_INT:
					this->get(al_name, &val.i, idx);
					printf("%s:%d\n",p->name.c_str(),val.i);
				break;

				case AL_PROP_VAL_FLOAT:
					this->get(al_name, &val.f, idx);
					printf("%s:%f\n",p->name.c_str(),val.f);
				break;
				case AL_PROP_VAL_INTV:
					this->getv(al_name,val.iv,idx);
					printf("%s:(%d,%d,%d)\n",p->name.c_str(),
						   val.iv[0],val.iv[1],val.iv[2]);
				break;
				case AL_PROP_VAL_FLOATV:
					this->getv(al_name,val.fv,idx);
					printf("%s:(%f,%f,%f)\n",p->name.c_str(),
							val.fv[0],val.fv[1],val.fv[2]);
				break;
			}
		}
	}
	void dump_all_prop(int idx=0)
	{
		printf("=====dump all props:\n");
		std::vector<ALenum>* props;
		if(obj_type == AL_OBJE_LISTENER)
		{
			props = &g_al_listener_props;

		}
		else if(obj_type == AL_OBJE_BUFFERS)
		{
			props = &g_al_buffers_props;
		}
		else if(obj_type == AL_OBJE_SROUCES)
		{
			props = &g_al_sources_props;
		}
		else
		{
			printf("unknow obj_type!\n");
			return;
		}

		for(auto it:*props)
		{
			dump_prop(it,idx);
		}
		printf("====end dump===\n");
	}
};

class ALListener:public ALObjects
{
public:
	static ALListener* GetInst()
	{
		static ALListener listener;//in c++ 0x
		return &listener;
	}
//--------------get set op---------------------
//set prop
	AL_DELC(alListeneri,set,int)
	AL_DELC(alListenerf,set,float);
	AL_DELC(alListeneriv,setv,int*);
	AL_DELC(alListenerfv,setv,float*);
//get prop
	AL_DELC(alGetListeneri,get,int*);
	AL_DELC(alGetListenerf,get,float*);
	AL_DELC(alGetListeneriv,getv,int*);
	AL_DELC(alGetListenerfv,getv,float*);

//------------props----------------------------
	// AL_PROP(get,gain,AL_GAIN,float*);
	// AL_PROP(set,gain,AL_GAIN,float);
	AL_PROP2(gain,AL_GAIN,get,float*,set,float);

	// AL_PROP(getv,pos,AL_POSITION,float*);
	// AL_PROP(setv,pos,AL_POSITION,float*);
	AL_PROP2(pos,AL_POSITION,getv,float*,setv,float*);

	// AL_PROP(getv,velocity,AL_VELOCITY,float*);
	// AL_PROP(setv,velocity,AL_VELOCITY,float*);
	AL_PROP2(velocity,AL_VELOCITY,getv,float*,setv,float*);


private:
	ALListener():ALObjects(AL_OBJE_LISTENER){}
};


class ALSources:public ALObjects
{
private:
	ALuint* _objs;
	int num;

public:
	ALSources(int n=1):ALObjects(AL_OBJE_SROUCES){
		_objs = new ALuint[n];
		num = n;
		alGenSources(n,_objs);
		CHECK_ERROR;
	}
	~ALSources(){
		alDeleteSources(num,_objs);
		delete[] _objs;
	}

	ALuint get_obj(int idx=0){return _objs[idx];}
	ALuint &operator[](int idx){return _objs[idx];}

// The playing source will have its state changed to AL_PLAYING. 
// When called on a source which is already playing, 
// the source will restart at the beginning. 
// When the attached buffer(s) are done playing, 
// the source will progress to the AL_STOPPED state.
	int play(int idx=0);
	int play_all();

	//The paused sources will have their state changed to AL_PAUSED.
	int pause(int idx=0);
	int pause_all();

//The stopped source will have its state changed to AL_STOPPED.
	int stop(int idx=0);
	int stop_all();
//alSourceRewind
	int rewind(int idx=0);
	int rewind_all();

// This function queues a set of buffers on a source. 
// All buffers attached to a source will be played in sequence, 
// and the number of processed buffers 
// can be detected using an alSourcei call to retrieve AL_BUFFERS_PROCESSED.
	int queue_buffers(int n,ALuint* buffers,int idx=0);

// This function unqueues a set of buffers attached to a source. 
// The number of processed buffers can be detected 
// using an alSourcei call to retrieve AL_BUFFERS_PROCESSED, 
// which is the maximum number of buffers that can be unqueued using this call.
	int unqueue_buffers(int n, ALuint* buffers, int idx=0);


//-----------------get set op------------------------------------
//set
	//int name(ALenum param, type a, int idx=0)

	AL_DELC_OBJS(alSourcei,set,int)
	AL_DELC_OBJS(alSourcef,set,float);
	AL_DELC_OBJS(alSourceiv,setv,int*);
	AL_DELC_OBJS(alSourcefv,setv,float*);

//get
	AL_DELC_OBJS(alGetSourcei,get,int*);
	AL_DELC_OBJS(alGetSourcef,get,float*);
	AL_DELC_OBJS(alGetSourceiv,getv,int*);
	AL_DELC_OBJS(alGetSourcefv,getv,float*);


//-----------------props--------------------------------
	AL_PROP_OBJS2(pitch,AL_PITCH,get,float*,set,float)

	AL_PROP_OBJS2(gain,AL_GAIN,get,float*,set,float)
	
	AL_PROP_OBJS2(pos,AL_POSITION,getv,float*,setv,float*)

	AL_PROP_OBJS2(type,AL_SOURCE_TYPE,get,int*,set,int)

	AL_PROP_OBJS2(looping,AL_LOOPING,get,int*,set,int)

	AL_PROP_OBJS2(buffer,AL_BUFFER,get,int*,set,int)
	
	AL_PROP_OBJS2(state,AL_SOURCE_STATE,get,int*,set,int)

//read only

	AL_PROP_OBJS(buffer_queued, AL_BUFFERS_QUEUED,get,int*);
	AL_PROP_OBJS(buffer_processed, AL_BUFFERS_PROCESSED, get, int*);



	AL_PROP_OBJS2(sec_offset,AL_SEC_OFFSET,get,float*,set,float)
	AL_PROP_OBJS2(sec_offset,AL_SEC_OFFSET,get,int*,set,int)

	AL_PROP_OBJS2(sample_offset,AL_SAMPLE_OFFSET,get,int*,set,int)

	AL_PROP_OBJS2(byte_offset,AL_BYTE_OFFSET,get,int*,set,int)

};

// struct ALAudioFormat
// {
// 		ALenum format; 
// 		ALsizei freq;
// };

class ALBuffers:public ALObjects
{
private:
	ALuint* _objs;
	int num;
public:
	ALBuffers(int n=1):ALObjects(AL_OBJE_BUFFERS)
	{
		num = n;
		_objs = new ALuint[n];
		alGenBuffers(n,_objs);
		CHECK_ERROR;
	}
	int Release();
	~ALBuffers(){
		// alDeleteBuffers(num,_objs);// may reuse
		delete[] _objs;
	}
	ALuint get_obj(int idx=0){return _objs[idx];}
	ALuint &operator[](int idx){return _objs[idx];}

// wrap alBufferData
// void alBufferData(ALuint buffer,ALenum format,
//       const ALvoid *data,
//       ALsizei size,
//       ALsizei freq
// );
// format: format type from among the following: 
// 		   AL_FORMAT_MONO8 
// 		   AL_FORMAT_MONO16
// 		   AL_FORMAT_STEREO8
// 		   AL_FORMAT_STEREO16
// data: pointer to the audio data
// size: the size of the audio data in bytes freq the frequency of the audio data
	int buffer_data(ALenum format,
		const ALvoid* data,
		ALsizei size, 
		ALsizei freq,int idx=0);

	int buffer_data(AudioFormat* a_fromat,const ALvoid* data,ALsizei size, int idx=0)
	{
		ALenum format = AL_FORMAT_STEREO16;
		int freq = a_fromat->nSamplesPerSec; //freq

		if(a_fromat->format == AUDIO_S16SYS)
		{
			if(a_fromat->nChannels==1)
			{
				format = AL_FORMAT_MONO16;
			}
			else
			{
				format = AL_FORMAT_STEREO16;
			}
		}
		else if(a_fromat->format == AUDIO_S8)
		{
			if(a_fromat->nChannels == 1)
			{
				format = AL_FORMAT_MONO8;
			}
			else
			{
				format = AL_FORMAT_STEREO8;
			}
		}
		else{
			printf("error:not support fomat!\n");
			return -1;
		}
		return buffer_data(format,data,size,freq,idx);
	}

//-----------------get set op ---------------------
//set prop
	AL_DELC_OBJS(alBufferi,set,int)
	AL_DELC_OBJS(alBufferf,set,float)
	AL_DELC_OBJS(alBufferiv,setv,int*)
	AL_DELC_OBJS(alBufferfv,setv,float*)
//get prop
	AL_DELC_OBJS(alGetBufferi,get,int*)
	AL_DELC_OBJS(alGetBufferf,get,float*)
	AL_DELC_OBJS(alGetBufferiv,getv,int*)
	AL_DELC_OBJS(alGetBufferfv,getv,float*)

//---------------props-----------------------------
	AL_PROP_OBJS2(freq,AL_FREQUENCY,get,int*,set,int)

	AL_PROP_OBJS2(bits,AL_BITS,get,int*,set,int)

	AL_PROP_OBJS2(channles,AL_CHANNELS,get,int*,set,int)

	AL_PROP_OBJS2(size,AL_SIZE,get,int*,set,int)

//no define??
	// AL_PROP_OBJS(get,data,AL_DATA,int*);
	// AL_PROP_OBJS(set,data,AL_DATA,int);
};

//wrap sources
//one palyer<--> one souces
class AudioPlayer: public BaseAudioPlayer,
public std::enable_shared_from_this<AudioPlayer>
{
public:
	// tPtrPcm pcm_buf;
	std::shared_ptr<ALSources> p_src;
	tPtrBaseDevice pdev;

	AudioPlayer():BaseAudioPlayer(){
		pdev = nullptr;
		p_src = std::make_shared<ALSources>(1);
	}
	void play()
	{
		if(status == playing)
			return;
		if(is_enable && p_src)
		{
			//pdev->play();
			status = playing;
			p_src->play();
		}
	}
	void pause()
	{
		if(status == paused)
			return;
		status = paused;
		if(p_src)
			p_src->pause();
	}
	void stop()
	{
		if(status == stoped)
			return;
		reset();
		p_src->stop();
	}
	void enable()
	{
		if(is_enable) return;
		is_enable = true;
		if(pdev)
		{
			pdev->on_enable(shared_from_this());
		}
	}
	void disable()
	{
		if(!is_enable) return;
		is_enable = false;
		if(pdev)
		{
			pdev->on_disable(shared_from_this());
		}
	}
	void set_dev(tPtrBaseDevice& p)
	{
		pdev = p;
	}
	void reset()
	{
		//do reset here?
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

	static tPtrAuDev p_inst;// = nullptr;

	tPtrAuDev get_inst()
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
	void play()
	{
		for(auto& it:players)
		{
			if(it->is_enable)
			{
				it->play();
			}
		}
	}
	void stop()
	{
		for(auto& it:players)
		{
			if(it->is_enable)
			{
				it->stop();
			}
		}
	}
	void pause()
	{
		for(auto& it:players)
		{
			if(it->is_enable)
			{
				it->pause();
			}
		}
	}
	void update()
	{
		for(auto& it: players)
		{
			if(it->is_enable)
			{
				it->update();
			}
		}
	}
	void enable_player(tPtrBasePlayer p)
	{
		p->enable();
	}
	void disable_player(tPtrBasePlayer p)
	{
		p->disable();
	}

	void on_enable(tPtrBasePlayer p)
	{
		;
	}
	void on_disable(tPtrBasePlayer p)
	{

	}

	void add_player(tPtrBasePlayer& p)
	{
		players.push_back(p);
	}
	void rm_player(tPtrBasePlayer& p)
	{
		auto find_it = std::find(players.begin(),
			players.end(),p);
		if(find_it!=players.end())
		{
			players.erase(find_it);
		}
	}

private:
	ALAuidoDevice()
	{
		init_al(&p_al_dev);
	}

};




#endif