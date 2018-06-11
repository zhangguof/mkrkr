

#include <cstring>
#include <vector>
#include <iostream>
#include <memory>
#include "alDevice.hpp"
#include "SDL.h"
#include "ffStream.hpp"

#include "AL/alc.h"
#include "AL/al.h"
#include "AL/alext.h"
#include "AL/alut.h"


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

std::map<ALenum, std::shared_ptr<ALProp> > g_al_props;
std::vector<ALenum> g_al_listener_props;
std::vector<ALenum> g_al_buffers_props;
std::vector<ALenum> g_al_sources_props;

void regist_al_prop(const ALenum al_name,uint16_t t,const char* name, uint16_t _type)
{

	auto it = g_al_props.find(al_name);
	if(it!=g_al_props.end())
		return;
	g_al_props[al_name] = std::make_shared<ALProp>(al_name,t,name,_type);

	if(_type & AL_OBJE_LISTENER)
		g_al_listener_props.push_back(al_name);
	if(_type & AL_OBJE_SROUCES)
		g_al_sources_props.push_back(al_name);
	if(_type & AL_OBJE_BUFFERS)
		g_al_buffers_props.push_back(al_name);
	// g_al_props.insert(std::make_pair(al_name, p));
}

void regist_all_props()
{
	//source properties
	REGIST_AL_PROP(AL_PITCH,AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);

	REGIST_AL_PROP(AL_GAIN,AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES|AL_OBJE_LISTENER);

	REGIST_AL_PROP(AL_MAX_DISTANCE,AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_ROLLOFF_FACTOR,AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_REFERENCE_DISTANCE,AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_MIN_GAIN, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_MAX_GAIN, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_CONE_OUTER_GAIN, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_CONE_INNER_ANGLE, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_CONE_OUTER_ANGLE, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);

	REGIST_AL_PROP(AL_POSITION, AL_PROP_VAL_FLOATV,AL_OBJE_SROUCES | AL_OBJE_LISTENER);
	REGIST_AL_PROP(AL_VELOCITY, AL_PROP_VAL_FLOATV,AL_OBJE_SROUCES | AL_OBJE_LISTENER);
	REGIST_AL_PROP(AL_DIRECTION, AL_PROP_VAL_FLOATV,AL_OBJE_SROUCES);

	REGIST_AL_PROP(AL_SOURCE_RELATIVE, AL_PROP_VAL_INT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_SOURCE_TYPE, AL_PROP_VAL_INT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_LOOPING, AL_PROP_VAL_INT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_BUFFER, AL_PROP_VAL_INT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_SOURCE_STATE, AL_PROP_VAL_INT,AL_OBJE_SROUCES);
	//read only
	REGIST_AL_PROP(AL_BUFFERS_QUEUED, AL_PROP_VAL_INT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_BUFFERS_PROCESSED, AL_PROP_VAL_INT,AL_OBJE_SROUCES);

	REGIST_AL_PROP(AL_SEC_OFFSET, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_SAMPLE_OFFSET, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);
	REGIST_AL_PROP(AL_BYTE_OFFSET, AL_PROP_VAL_FLOAT,AL_OBJE_SROUCES);

	//listener
	REGIST_AL_PROP(AL_GAIN, AL_PROP_VAL_FLOAT,AL_OBJE_LISTENER);
	REGIST_AL_PROP(AL_ORIENTATION, AL_PROP_VAL_FLOATV,AL_OBJE_LISTENER);

	//buffers
	REGIST_AL_PROP(AL_FREQUENCY, AL_PROP_VAL_INT,AL_OBJE_BUFFERS);
	REGIST_AL_PROP(AL_BITS, 	 AL_PROP_VAL_INT,AL_OBJE_BUFFERS);
	REGIST_AL_PROP(AL_CHANNELS,  AL_PROP_VAL_INT,AL_OBJE_BUFFERS);
	REGIST_AL_PROP(AL_SIZE, 	 AL_PROP_VAL_INT,AL_OBJE_BUFFERS);

}


//-----ALSource----------
int ALSources::play(int idx)
{
	alSourcePlay(_objs[idx]);
	return CHECK_ERROR;
}

int ALSources::play_all()
{
	alSourcePlayv(num,_objs);
	return CHECK_ERROR;
}
//The paused sources will have their state changed to AL_PAUSED.
int ALSources::pause(int idx)
{
	alSourcePause(_objs[idx]);
	return CHECK_ERROR;
}
int ALSources::pause_all()
{
	alSourcePausev(num,_objs);
	return CHECK_ERROR;
}

int ALSources::stop(int idx)
{
	alSourceStop(_objs[idx]);
	return CHECK_ERROR;
}
int ALSources::stop_all()
{
	alSourceStopv(num,_objs);
	return CHECK_ERROR;
}
int ALSources::rewind(int idx)
{
	alSourceRewind(_objs[idx]);
	return CHECK_ERROR;
}
int ALSources::rewind_all()
{
	alSourceRewindv(num,_objs);
	return CHECK_ERROR;
}
int ALSources::queue_buffers(int n,ALuint* buffers,int idx)
{
	alSourceQueueBuffers(_objs[idx],n,buffers);
	return CHECK_ERROR;
}
int ALSources::unqueue_buffers(int n, ALuint* buffers, int idx)
{
	alSourceUnqueueBuffers(_objs[idx],n,buffers);
	return CHECK_ERROR;
}

int ALBuffers::Release()
{
	//release buffer data.
	alDeleteBuffers(num,_objs);
	return CHECK_ERROR;
}

////-----ALBuffers----------

int ALBuffers::buffer_data(ALenum format,
const ALvoid* data,ALsizei size,
ALsizei freq,int idx)
{
	alBufferData(_objs[idx],format,data,size,freq);
	return CHECK_ERROR;
}

//-------------ALAudioDevice---------------
tPtrAuDev ALAuidoDevice::p_inst = nullptr;

//---------init al------------------------

ALCdevice* cur_alc_device = nullptr;
static bool is_init_al = false;

int init_al(ALCdevice** pdev=nullptr)
{
	if(is_init_al)
		return 0;
	regist_all_props();
	// alGetError();

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
    cur_alc_device = device;
    if(pdev)
    	*pdev = device;
    return 0;
}


// alutLoadWAVFile (ALbyte *fileName, ALenum *format, void **data, ALsizei *size, ALsizei *frequency);
// void ALUT_APIENTRY alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei frequency);
#define NUM_BUFFERS (4)
void al_test_play()
{
	alGetError(); // clear error code
	ALBuffers bufs(NUM_BUFFERS);

	int error;

    ALenum format;
    ALvoid *data;
    ALsizei size;
    ALsizei freq;

	// Load test.wav
	const char* name= "a1.wav";
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
	bufs.dump_all_prop();

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

	ALfloat pos[3]={0.0f,0.0f,1.0f};

	src.dump_prop(AL_POSITION);


	// alSourcei(source[0], AL_BUFFER, g_Buffers[0]);
	src.set_buffer(bufs[0]);

	ALListener* listener = ALListener::GetInst();

	listener->setv_pos(pos);

	listener->getv_pos(pos);
	printf("=====%.2f,%.2f,%.2f\n",pos[0],pos[1],pos[2]);
	// src.dump_all_prop();

	listener->dump_all_prop();


	// listener->getv_pos(pos);
	// printf("listener pos:%f,%f,%f\n",pos[0],pos[1],pos[2]);

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