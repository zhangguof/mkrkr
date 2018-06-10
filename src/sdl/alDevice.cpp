
#include <algorithm>
#include <cstring>
#include <vector>
#include <iostream>

#include "SDL.h"
#include "ffStream.hpp"

#include "AL/alc.h"
#include "AL/al.h"
#include "AL/alext.h"
#include "AL/alut.h"

#define CHECK_ERROR al_check_error(__FILE__,__FUNCTION__,__LINE__)

int al_check_error(const char* filename,
const char* fun_name,int line)
{
	ALenum error = alGetError();
	if(error == AL_NO_ERROR)
		return 0;
	printf("file:%s:%d,func:%s,error(%d):%s\n",
		filename,line,fun_name,
		error,alGetString(error));
	return -1;
}

#define AL_DELC(alname,name,type) \
int name(ALenum param, type a) \
{ \
	alname(param,a); \
	return CHECK_ERROR; \
}

#define AL_PROP(op,name,prop,type) \
int op##_##name(type a) \
{ \
op(prop,a); \
return CHECK_ERROR; \
}


#define AL_DELC_OBJS(alname,name,type) \
int name(ALenum param,type a, int idx=0) \
{ \
alname(_objs[idx],param,a); \
return CHECK_ERROR; \
}

#define AL_PROP_OBJS(op,name,prop,type) \
int op##_##name(type a,int idx=0) \
{ \
op(prop,a,idx); \
return CHECK_ERROR; \
}


class ALObjects
{
public:
	enum ObjType
	{
		Source,Buffer
	} Type;
};


class ALListener
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
	AL_PROP(get,gain,AL_GAIN,float*);
	AL_PROP(set,gain,AL_GAIN,float);

	AL_PROP(getv,pos,AL_POSITION,float*);
	AL_PROP(setv,pos,AL_POSITION,float*);

	AL_PROP(getv,velocity,AL_VELOCITY,float*);
	AL_PROP(setv,velocity,AL_VELOCITY,float*);

private:
	ALListener(){
	}
};

class ALSources:public ALObjects
{
private:
	ALuint* _objs;
	int num;

public:
	ALSources(int n=1){
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
	int play(int idx=0)
	{
		alSourcePlay(_objs[idx]);
		return CHECK_ERROR;
	}
	int play_all()
	{
		alSourcePlayv(num,_objs);
		return CHECK_ERROR;
	}
	//The paused sources will have their state changed to AL_PAUSED.
	int pause(int idx=0)
	{
		alSourcePause(_objs[idx]);
		return CHECK_ERROR;
	}
	int pause_all()
	{
		alSourcePausev(num,_objs);
		return CHECK_ERROR;
	}
//The stopped source will have its state changed to AL_STOPPED.
	int stop(int idx=0)
	{
		alSourceStop(_objs[idx]);
		return CHECK_ERROR;
	}
	int stop_all()
	{
		alSourceStopv(num,_objs);
		return CHECK_ERROR;
	}
//alSourceRewind
	int rewind(int idx=0)
	{
		alSourceRewind(_objs[idx]);
		return CHECK_ERROR;
	}
	int rewind_all()
	{
		alSourceRewindv(num,_objs);
		return CHECK_ERROR;
	}

// This function queues a set of buffers on a source. 
// All buffers attached to a source will be played in sequence, 
// and the number of processed buffers 
// can be detected using an alSourcei call to retrieve AL_BUFFERS_PROCESSED.
	int queue_buffers(int n,ALuint* buffers,int idx=0)
	{
		alSourceQueueBuffers(_objs[idx],n,buffers);
		return CHECK_ERROR;
	}

// This function unqueues a set of buffers attached to a source. 
// The number of processed buffers can be detected 
// using an alSourcei call to retrieve AL_BUFFERS_PROCESSED, 
// which is the maximum number of buffers that can be unqueued using this call.
	int unqueue_buffers(int n, ALuint* buffers, int idx=0)
	{
		alSourceUnqueueBuffers(_objs[idx],n,buffers);
		return CHECK_ERROR;
	}
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
	AL_PROP_OBJS(get,pitch,AL_PITCH,float*)
	AL_PROP_OBJS(set,pitch,AL_PITCH,float)

	AL_PROP_OBJS(get,gain,AL_GAIN,float*)
	AL_PROP_OBJS(set,gain,AL_GAIN,float)

	AL_PROP_OBJS(getv,pos,AL_POSITION,float*)
	AL_PROP_OBJS(setv,pos,AL_POSITION,float*)

	AL_PROP_OBJS(get,type,AL_SOURCE_TYPE,int*)
	AL_PROP_OBJS(set,type,AL_SOURCE_TYPE,int);

	AL_PROP_OBJS(get,looping,AL_LOOPING,int*);
	AL_PROP_OBJS(set,looping,AL_LOOPING,int);

	AL_PROP_OBJS(get,buffer,AL_BUFFER,int*);
	AL_PROP_OBJS(set,buffer,AL_BUFFER,int);

	AL_PROP_OBJS(get,state,AL_SOURCE_STATE,int*);
	AL_PROP_OBJS(set,state,AL_SOURCE_STATE,int);

	AL_PROP_OBJS(get,buffer_queued,AL_BUFFERS_QUEUED,int*);
	AL_PROP_OBJS(set,buffer_queued,AL_BUFFERS_QUEUED,int);

	AL_PROP_OBJS(get,buffer_processed,AL_BUFFERS_PROCESSED,int*);
	AL_PROP_OBJS(set,buffer_processed,AL_BUFFERS_PROCESSED,int);

	AL_PROP_OBJS(get,sec_offset,AL_SEC_OFFSET,float*);
	AL_PROP_OBJS(set,sec_offset,AL_SEC_OFFSET,float);
	AL_PROP_OBJS(get,sec_offset,AL_SEC_OFFSET,int*);
	AL_PROP_OBJS(set,sec_offset,AL_SEC_OFFSET,int);

	AL_PROP_OBJS(get,sample_offset,AL_SAMPLE_OFFSET,int*);
	AL_PROP_OBJS(set,sample_offset,AL_SAMPLE_OFFSET,int);

	AL_PROP_OBJS(get,byte_offset,AL_BYTE_OFFSET,int*);
	AL_PROP_OBJS(set,byte_offset,AL_BYTE_OFFSET,int);


};

class ALBuffers:public ALObjects
{
private:
	ALuint* _objs;
	int num;
public:
	ALBuffers(int n=1){
		num = n;
		_objs = new ALuint[n];
		alGenBuffers(n,_objs);
		CHECK_ERROR;
	}
	int Release()
	{
		//release buffer data.
		alDeleteBuffers(num,_objs);
		return CHECK_ERROR;
	}
	~ALBuffers(){
		// alDeleteBuffers(num,_objs);// may reuse
		delete[] _objs;
	}
	ALuint get_obj(int idx=0){return _objs[idx];}
	ALuint &operator[](int idx){return _objs[idx];}

	int buffer_data(ALenum format,
		const ALvoid* data,ALsizei size, 
		ALsizei freq,int idx=0);
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
	AL_PROP_OBJS(get,freq,AL_FREQUENCY,int*);
	AL_PROP_OBJS(set,freq,AL_FREQUENCY,int);

	AL_PROP_OBJS(get,bits,AL_BITS,int*);
	AL_PROP_OBJS(set,bits,AL_BITS,int);

	AL_PROP_OBJS(get,channles,AL_CHANNELS,int*);
	AL_PROP_OBJS(set,channles,AL_CHANNELS,int);

	AL_PROP_OBJS(get,size,AL_SIZE,int*);
	AL_PROP_OBJS(set,size,AL_SIZE,int);

//no define??
	// AL_PROP_OBJS(get,data,AL_DATA,int*);
	// AL_PROP_OBJS(set,data,AL_DATA,int);
};


// wrap alBufferData
// void alBufferData(ALuint buffer,ALenum format,
//       const ALvoid *data,
//       ALsizei size,
//       ALsizei freq
// );
// format: format type from among the following: 
// 		   AL_FORMAT_MONO8 AL_FORMAT_MONO16 AL_FORMAT_STEREO8 AL_FORMAT_STEREO16
// data: pointer to the audio data
// size: the size of the audio data in bytes freq the frequency of the audio data
int ALBuffers::buffer_data(ALenum format,
const ALvoid* data,ALsizei size, ALsizei freq,int idx)
{
	alBufferData(_objs[idx],format,data,size,freq);
	return CHECK_ERROR;
}



// ALCdevice* g_alc_device = nullptr;
int init_al()
{
	alGetError();

	ALCdevice* device = alcOpenDevice(nullptr); 
	ALCcontext *context = alcCreateContext(device, nullptr);
	if(!context || alcMakeContextCurrent(context) == ALC_FALSE)
	{
	    std::cerr<< "Failed to set up audio device" <<std::endl;
	    if(context)
	        alcDestroyContext(context);
	    return -1;
	}
    const ALCchar *name = nullptr;
    if(alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
    if(!name || alcGetError(device) != AL_NO_ERROR)
        name = alcGetString(device, ALC_DEVICE_SPECIFIER);
    std::cout<< "Opened \""<<name<<"\"" <<std::endl;

}

#define NUM_BUFFERS 4
#define BUFFER_TIME_MS 200

ALuint g_Buffers[NUM_BUFFERS];
ALuint source[1];

// alutLoadWAVFile (ALbyte *fileName, ALenum *format, void **data, ALsizei *size, ALsizei *frequency);
// void ALUT_APIENTRY alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei frequency);

void al_test_play()
{
	alGetError(); // clear error code
	// alGenBuffers(NUM_BUFFERS, g_Buffers);
	ALBuffers bufs(NUM_BUFFERS);

	int error;

    ALenum format;
    ALvoid *data;
    ALsizei size;
    ALsizei freq;

	// Load test.wav
	const char* name= "a0001.wav";
	alutLoadWAVFile((ALbyte*)name,&format,&data,&size,&freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
	      printf("alutLoadWAVFile test.wav : %d\n",error);
	      // alDeleteBuffers(NUM_BUFFERS, g_Buffers);
	      bufs.Release();
	      return;
	}

	// Copy test.wav data into AL Buffer 0
	// alBufferData(g_Buffers[0],format,data,size,freq);
	bufs.buffer_data(format,data,size,freq,0);

	if ((error = alGetError()) != AL_NO_ERROR)
	{
	      printf("alBufferData buffer 0 : %d\n", error);
	      // alDeleteBuffers(NUM_BUFFERS, g_Buffers);
	      bufs.Release();
	      return;
	}

	// Unload test.wav
	alutUnloadWAV(format,data,size,freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
	      printf("alutUnloadWAV : %d\n", error);
	      // alDeleteBuffers(NUM_BUFFERS, g_Buffers);
	      bufs.Release();
	      return;
	}

	// Generate Sources
	// alGenSources(1,source);
	ALSources src;

	// alSourceRewind(source[0]);
	// Attach buffer 0 to source
	ALfloat gain;
	// gain = 0.5f;

	// alSourcef(source[0],AL_GAIN,gin);
	// alGetSourcef(source[0],AL_GAIN,&gin);
	// src.set_gain(gain);
	src.get_gain(&gain);
	printf("source gain:%f\n",gain);

	// alGetSourcef(source[0],AL_MAX_GAIN,&gin);
	// printf("source max gain:%f\n", gin);
	// alGetSourcef(source[0],AL_MIN_GAIN,&gin);
	// printf("source min gain:%f\n", gin);

	ALfloat pos[3]={0.0f,0.0f,0.0f}
	// alGetSourcefv(source[0],AL_POSITION,pos);
	src.getv_pos(pos);
	printf("source pos:%f,%f,%f\n",pos[0],pos[1],pos[2]);

	// alSourcei(source[0], AL_BUFFER, g_Buffers[0]);
	src.set_buffer(bufs[0]);

	ALListener* listener = ALListener::GetInst();

	listener->getv_pos(pos);
	printf("listener pos:%f,%f,%f\n",pos[0],pos[1],pos[2]);

    // alSourceQueueBuffers(source[0], 1, g_Buffers);
    // alSourcePlay(source[0]);
    src.play(0);
    // sdl_loop();
    SDL_Delay(2000);
// Exit
	auto Context=alcGetCurrentContext();
	auto Device=alcGetContextsDevice(Context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(Device);

}