#include "alDevice.hpp"

int al_check_error(const char* filename,
const char* fun_name,int line)
{
	ALenum error = alGetError();
	if(error == AL_NO_ERROR)
		return 0;
	printf("file:%s:%d,func:%s,error(%d):%s\n",
		filename,line,fun_name,
		error,alGetString(error));
	assert(false);
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
	// SDL_Log("buffer_data:format:%d,id:%d",format,_objs[idx]);
	alBufferData(_objs[idx],format,data,size,freq);
	return CHECK_ERROR;
}

int ALBuffers::buffer_data(AudioFormat* a_fromat,const ALvoid* data,ALsizei size, int idx)
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

