

#include <cstring>
#include <vector>
#include <iostream>
#include <memory>
#include "alDevice.hpp"
#include "GLDevice.hpp"
#include "SDL.h"
#include "ffStream.hpp"

#include "AL/alc.h"
#include "AL/al.h"
#include "AL/alext.h"
#include "AL/alut.h"


//--------------AudioPlyaer------------------------

AudioPlayer::AudioPlayer(int nbuffer):BaseAudioPlayer()
{
	pdev = nullptr;
	p_src = std::make_shared<ALSources>(1);
	p_bufs = std::make_shared<ALBuffers>(nbuffer);
	n_buffer = nbuffer;
	n_queued_buffer = 0;
	ff_stream = nullptr;
}

int AudioPlayer::buffer_data_one(int buf_idx,int update_time_ms)
{
	int freq = ff_stream->freq;//ff_stream->target_params.sample_rate;
	int channles = ff_stream->channels;//ff_stream->target_params.channles;
	ALenum format = AL_FORMAT_STEREO16;
	int size = get_buffer_data_size(channles,
		freq,16,
		update_time_ms);
	uint8_t* _data = nullptr;
	int len = pdb->read(&_data, size);
	if(len == 0)
		return 0;
	assert(len <= size);

	p_bufs->buffer_data(format,_data,len,freq,buf_idx);
	return len;
}

void AudioPlayer::update()
{
	printf("AudioPlayer update!\n");
	int buffer_processed = 0;
	int queued_buffer = 0;
	if(status == init)
		buffer_processed = n_buffer;
	else
	{
		
		p_src->get_buffer_processed(&buffer_processed);
		p_src->get_buffer_queued(&queued_buffer);
		printf("=====now status:%d,buffer_processed:%d,queued:%d\n",status,
			buffer_processed,queued_buffer
		);
		//processed all buffer.
		if(status == stopping
		   && n_queued_buffer == 0
		   //&& queued_buffer == 0
		   )
		{
			stop();
			return;
		}
	}
	uint32_t buffer_ids[n_buffer];
	uint32_t* p_buffer_ids = buffer_ids;
	int num_to_queue = 0;
	int r;
	if(buffer_processed>0)
	{	
		//get free buffer
		if(status != init)
		{
			r = p_src->unqueue_buffers(
				buffer_processed,buffer_ids);
			printf("unqueue buffers:%d\n",buffer_processed);
			if(r<0)
				return;
			n_queued_buffer -= buffer_processed;
		}
		else
		{
			p_buffer_ids = p_bufs->get_objs();
		}
		for(int i=0;i<buffer_processed;++i)
		{
			r = buffer_data_one(
				p_bufs->get_idx(p_buffer_ids[i]),
				UPDATE_TIME
				);
			if(r==0)
			{
				//no data;
				//stop it.
				status = stopping;
				break;
			}
			num_to_queue++;
		}
	}
	if(num_to_queue>0)
	{
		p_src->queue_buffers(num_to_queue,p_buffer_ids);
		n_queued_buffer += num_to_queue;
		p_src->get_buffer_queued(&queued_buffer);
		printf("queue buffers:%d/%d\n",num_to_queue,queued_buffer);
	}
}

void AudioPlayer::read_from_ffstream(
	std::shared_ptr<ffStream>& ff)
{
	ff_stream = ff;
	if(!pdb)
	{
		pdb = ff_stream->get_decode_buffer();
	}
	// update();
}

void AudioPlayer::play()
{
	printf("AudioPlayer do play!\n");
	if(status == playing)
		return;
	if(status == init)
	{
		update();//push buffer frist.
	}
	if(is_enable && p_src)
	{
		//pdev->play();
		assert(status == stoped || status == paused || status == init);
		status = playing;
		p_src->play();
	}
}

void AudioPlayer::pause()
{
	if(status == paused)
		return;
	status = paused;
	if(p_src)
		p_src->pause();
	
}

void AudioPlayer::stop()
{
	printf("do stop: now status:%d\n",status);
	if(status == stoped)
		return;
	reset();
	p_src->stop();
	status = stoped;
	pdev->on_stop(shared_from_this());
}
void AudioPlayer::enable()
{
	if(is_enable) return;
	is_enable = true;
	if(pdev)
	{
		pdev->on_enable(shared_from_this());
	}
}
void AudioPlayer::disable()
{
	if(!is_enable) return;
	is_enable = false;
	if(pdev)
	{
		pdev->on_disable(shared_from_this());
	}
}


//-------------ALAudioDevice---------------
tPtrAuDev ALAuidoDevice::p_inst = nullptr;

void ALAuidoDevice::play()
{
	printf("ALAuidoDevice:: do play!\n");
	for(auto& it:players)
	{
		if(it->is_enable)
		{
			it->play();
		}
	}
}

void ALAuidoDevice::stop()
{
	for(auto& it:players)
	{
		if(it->is_enable)
		{
			it->stop();
		}
	}
}

void ALAuidoDevice::on_stop(tPtrBasePlayer p)
{
	if(p->is_loop)
	{
		p->status = AudioPlayer::init;
		// p->update(); //queue buffers first
		p->play();
	}
}

void ALAuidoDevice::pause()
{
	for(auto& it:players)
	{
		if(it->is_enable)
		{
			it->pause();
		}
	}
}
//return next wake
uint32_t ALAuidoDevice::update()
{
	static uint32_t last_tick = 0;
	uint32_t cur_tick = SDL_GetTicks();
	if(cur_tick - last_tick < UPDATE_TIME)
		return 0;
	for(auto& it: players)
	{
		if(it->is_enable && it->status!= AudioPlayer::stoped)
		{
			it->update();
		}
	}
	last_tick = cur_tick;
	return UPDATE_TIME;
}
void ALAuidoDevice::add_player(tPtrBasePlayer p)
{
	players.push_back(p);
	auto _p = std::static_pointer_cast<AudioPlayer>(p);
	_p->set_dev(shared_from_this());
}
void ALAuidoDevice::rm_player(tPtrBasePlayer p)
{
	auto find_it = std::find(players.begin(),
		players.end(),p);
	if(find_it!=players.end())
	{
		players.erase(find_it);
	}
}

ALAuidoDevice::ALAuidoDevice():BaseAudioDevice()
{
	init_al(&p_al_dev);
}

ALAuidoDevice::~ALAuidoDevice()
{
	auto Context=alcGetCurrentContext();
	// auto Device=alcGetContextsDevice(Context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(Context);
	alcCloseDevice(p_al_dev);
}
//---------init al------------------------

ALCdevice* cur_alc_device = nullptr;
static bool is_init_al = false;
extern void regist_all_props();

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

tPtrAuDev get_al_dev()
{
	return ALAuidoDevice::get_inst();
}

void al_loop(uint32_t interval)
{
	static auto pd = get_al_dev();
	uint32_t t = pd->update();
}

// extern void sdl_loop();
// extern void regist_update(UpdateFunc f);

void al_test_play()
{
	auto p_dev = get_al_dev();
	auto p_player = std::make_shared<AudioPlayer>();
	// const char* name = "../../data/bgm/ＢＧＭ／通常１.ogg";
	const char* name = "../../voice/a0001.ogg";

	
	auto p_ffs = std::make_shared<ffStream>(name);
    // ffStream ffs2("../../voice/a0001.ogg");
    // ffStream ffs3("../../data/井内啓二 - 英雄願望 〜アルゴイゥ卜~.mp3");
  
	p_dev->add_player(static_cast<tPtrBasePlayer>(
		   p_player));
	p_player->enable();
	// p_player->set_loop(true);
	p_player->read_from_ffstream(p_ffs);
	p_dev->play();

	regist_update(al_loop);

// SDL_Delay(3000);
	sdl_loop();
//exit

}
