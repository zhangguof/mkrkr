

#include <cstring>
#include <vector>
#include <iostream>
#include <memory>
#include "alDevice.hpp"
#include "GLDevice.hpp"
#include "SDL.h"
#include "ffStream.hpp"

//#include "AL/alc.h"
//#include "AL/al.h"
//#include "AL/alext.h"
//#include "AL/alut.h"


//--------------AudioPlyaer------------------------

AudioPlayer::AudioPlayer(int nbuffer,bool _loop):BaseAudioPlayer(_loop)
{
	pdev = nullptr;
	p_src = std::make_shared<ALSources>(1);
	p_bufs = std::make_shared<ALBuffers>(nbuffer);
	n_buffer = nbuffer;
	n_queued_buffer = 0;
	ff_stream = nullptr;
	is_static_type = false;
	has_lock = false;

	buffer_status = new BufferStatus[nbuffer];
	reset_buffer_status();
}

int AudioPlayer::buffer_data_one(int buf_idx,int update_time_ms)
{
	int freq = ff_stream->freq;//ff_stream->target_params.sample_rate;
	int channles = ff_stream->channels;//ff_stream->target_params.channles;
	ALenum _format = AL_FORMAT_STEREO16;
	int size = get_buffer_data_size(channles,
		freq,16,
		update_time_ms);
	uint8_t* _data = nullptr;
	int len = pdb->read(&_data, size);
	if(len == 0)
		return 0;
	assert(len <= size);

	p_bufs->buffer_data(_format,_data,len,freq,buf_idx);
	return len;
}


int AudioPlayer::buffer_data_unit(int buf_idx,int bytes)
{
	// SDL_Log("buffer_data_unit:%d:%d,freq:%d,chs:%d,format:%d",
//	buf_idx,bytes,format.nSamplesPerSec,format.nChannels, format.format);
	assert(buf_idx >= 0);

	uint32_t freq = format.nSamplesPerSec;
	int channles = format.nChannels;
	ALenum _format = format.format;
	uint8_t* _data;
	// assert(bytes == nUnitSize);
	// SDL_Log("going to read L1 buffer:readable:%d,writeable:%d",
	// plb->readable_len,plb->writeable_len);
	bool ret  = plb->read(&_data,bytes);
	if(!ret)
	{
		return 0;
	}
	// SDL_Log("buffer_data_unit end:bytes:%d",bytes);
	p_bufs->buffer_data(_format,_data,bytes,freq,buf_idx);
	return bytes;
}


int AudioPlayer::buffer_data_all(int buf_idx)
{
	int freq = ff_stream->target_params.sample_rate;
    int channels = ff_stream->target_params.channels;
    assert(channels == 1 || channels ==2);
    ALenum format = AL_FORMAT_STEREO16;
    if(channels == 1)
    {
        format = AL_FORMAT_MONO16;
    }
    
//    ALenum format = ff_stream->target_params.sample_fmt;

	int len = pdb->len;
	uint8_t* _data = pdb->get_data();
	// int len = pdb->read(&_data, size);
	if(len == 0)
		return 0;

	p_bufs->buffer_data(format,_data,len,freq,buf_idx);
	p_src->set_buffer(p_bufs->get_obj(0));
	return len;
}

void AudioPlayer::unqueue_buffers()
{
	if(status != init)
	{
		int free_buffer = 0;
		uint32_t buffer_ids[n_buffer];

		p_src->get_buffer_processed(&free_buffer);
		if(free_buffer>0)
		{
			int r = p_src->unqueue_buffers(free_buffer,buffer_ids);
			if(r<0)
				return;
			change_buffers_status(buffer_ids,free_buffer,Unqueued);
			n_queued_buffer--;
		}
	}
	// n = 0;
	// int free_buffer = 0;
	// int r;
	// // if(n_free_buffer!=0)
	// // 	return;
	// if(status == init)
	// {
	// 	//all buffer free.
	// 	if(n_queued_buffer == 0)
	// 	{
	// 		free_buffer = n_buffer;
	// 		memcpy(buffer_ids, p_bufs->get_objs(), free_buffer*sizeof(ALuint));
	// 	}
	// }
	// else if(status == playing)
	// {
	// 	p_src->get_buffer_processed(&free_buffer);
	// 	if(free_buffer>0)
	// 	{
	// 		r = p_src->unqueue_buffers(free_buffer,buffer_ids);
	// 		if(r<0)
	// 			return;
	// 		n_queued_buffer--;
	// 	}
	// }
	// n = free_buffer;
}

void AudioPlayer::queue_buffers(ALuint* buffer_ids, int n)
{
	p_src->queue_buffers(n,buffer_ids);
	change_buffers_status(buffer_ids,n,Queued);
	n_queued_buffer += n;
}

void AudioPlayer::update_units()
{
	// SDL_Log("update uints");
	if(status == stopping && n_queued_buffer == 0)
	{
		stop();
		return;
	}
	uint32_t buffer_idx[n_buffer];
	uint32_t buffer_ids[n_buffer];
	int nfree = 0;
	unqueue_buffers();

	get_free_buffers_idx(buffer_idx,nfree);
	int num_to_queue = 0;
	if(nfree>0)
	{
		// SDL_Log("update_uint...:%d",nfree);	
		for(int i=0; i<nfree; ++i)
		{
			int idx = buffer_idx[i];
			int r = buffer_data_unit(idx,nUnitSize);

			if(r==0)
			{
				// status = stopping;
				break;
			}
			buffer_ids[i] = p_bufs->get_obj(idx);
			num_to_queue++;
		}
		queue_buffers(buffer_ids, num_to_queue);
		//dump 
		// int queued_buffer;
		// p_src->get_buffer_queued(&queued_buffer);
		// printf("queue buffers:%d/%d\n",num_to_queue,queued_buffer);

	}

}

void AudioPlayer::update_uint()
{
}
void AudioPlayer::update()
{
	if(plb)
		return update_units();

	
	// printf("AudioPlayer update!\n");

	// if(is_static_type)
		// return;
	// if(!ff_stream)
	// 	return;
	// int nfree = 0;
	// int queued_buffer = 0;
	// if(status == stopping && n_queued_buffer == 0)
	// {
	// 	stop();
	// 	return;
	// }

	// uint32_t buffer_ids[n_buffer];

	// unqueue_buffers(buffer_ids, nfree);

	// int num_to_queue = 0;
	// int r;
	// if(nfree>0)
	// {	
	// 	for(int i=0;i<nfree;++i)
	// 	{
	// 		r = buffer_data_one(
	// 			p_bufs->get_idx(buffer_ids[i]),
	// 			UPDATE_TIME);
	// 		if(r==0)
	// 		{
	// 			//no data;
	// 			//stop it.
	// 			status = stopping;
	// 			break;
	// 		}
	// 		num_to_queue++;
	// 	}
	// }
	// if(num_to_queue>0)
	// {
	// 	queue_buffers(buffer_ids, num_to_queue);
	// 	p_src->get_buffer_queued(&queued_buffer);
	// 	printf("queue buffers:%d/%d\n",num_to_queue,queued_buffer);
	// }
}

void AudioPlayer::read_from_ffstream(
	std::shared_ptr<ffStream>& ff,bool is_static)
{
	ff_stream = ff;
	//clear it
	p_src->rewind();
	int buf = 0;
	p_src->set_buffer(buf);

	int type;
	p_src->get_type(&type);
	assert(type == AL_UNDETERMINED);


	if(!pdb)
	{
		pdb = ff_stream->get_decode_buffer();
	}
	is_static_type = is_static;

	// update();
}

void AudioPlayer::play()
{
	printf("AudioPlayer do play!:status:%d\n",status);
	if(status == playing)
		return;
	// reset_buffer_status();
	
	if(status == init)
	{	
		//push buffer frist.
		if(is_static_type)
		{
			buffer_data_all(0);
			if(is_loop)
			{
				ALenum i = AL_TRUE;
				p_src->set_looping(i);
			}
		}
		else
		{
			// update();
			update_units();
		}
	}
	if(is_enable)
	{
		//pdev->play();
		if(status == stopping)
			stop();
		assert(status == stoped || status == paused || status == init);
		status = playing;
		p_src->play();
		p_src->dump_all_prop();
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
	if(status == stoped || status == init)
		return;
	// seek(0);
	status = stoped;
	reset();
	p_src->stop();
	reset_buffer_status();
	p_src->set_buffer(0);
	p_src->rewind();
	status = init;
	
	// if(!is_static_type)
	// 	pdev->on_stop(shared_from_this());
}
void AudioPlayer::do_stop()
{
	printf("do stopping: now status:%d\n",status);
	if(status == stoped || status == stopping)
		return;
	status = stopping;
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

//ready for fill buffer
bool AudioPlayer::lock(int pos,int bytes, void** p1,int* b1)
{
	if(has_lock)
	{
		*p1 = nullptr;
		*b1 = 0;
		return false;
	}
	// *p1 = pdb.get_dat
	has_lock = true;
	plb->lock(pos,p1,bytes);
	*b1 = bytes;
	
	return true;
}

//end for fill buffer,sync to openal obj
bool AudioPlayer::unlock(void* p1, int b1)
{
	if(has_lock)
	{
		has_lock = false;
		plb->unlock(p1,b1);
		return true;
	}
	return false;
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
		// p->play();
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
			// it->update();
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


int ALAuidoDevice::SetFormat(const AudioFormat* fm)
{
	format = *fm;
	// if(pDev)
	// {
	// 	int ret = pDev->init_spec(fm->nSamplesPerSec,fm->nChannels);
	// 	return ret;
	// }
	return 0;
}
int ALAuidoDevice::GetFormat(AudioFormat* fm)
{
	*fm = format;
	return 0;
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
	auto p_player1 = std::make_shared<AudioPlayer>();
	// auto p_player2 = std::make_shared<AudioPlayer>();

    std::string name1 = "a0001.ogg";
	// const char* name2 = "../../voice/a0001.ogg";

	auto p_ffs1 = std::make_shared<ffStream>(name1,true);
	// auto p_ffs2 = std::make_shared<ffStream>(name2);
    // ffStream ffs2("../../voice/a0001.ogg");
    // ffStream ffs3("../../data/井内啓二 - 英雄願望 〜アルゴイゥ卜~.mp3");
  
	p_dev->add_player(static_cast<tPtrBasePlayer>(p_player1));
	// p_dev->add_player(static_cast<tPtrBasePlayer>(p_player2));
     p_player1->enable();
     p_player1->set_loop(true);
	p_player1->read_from_ffstream(p_ffs1,true);//static type
	// p_player1->disable();

	// p_player2->read_from_ffstream(p_ffs2,true);
	// p_player2->set_loop(true);

	p_dev->play();

	regist_update(al_loop);

// SDL_Delay(3000);
	// sdl_loop();
//exit

}
