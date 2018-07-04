#include "SDL.h"
#include "SDL_audio.h"
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>
#include "baseAudio.hpp"

#define AVCODEC_MAX_AUDIO_FRAME_SIZE (4096*2)


// uint8_t *check_data = NULL;
// extern void check_buf(uint8_t* v,uint8_t* buf, int len,int idx,int pos);
// uint8_t test_data[4096*4027];


// class audioDevice;



//implent
void pcmBuffer::enable()
{
	is_enable = true;
	if(pDev)
	{
		pDev->on_enable(shared_from_this());
	}
}
void pcmBuffer::disable()
{
	is_enable = false;
	if(pDev)
	{
		pDev->on_disable(shared_from_this());
	}
}
int pcmBuffer::get_data(uint8_t** _data, int _len)
{
	int ret_len = db.read(_data,_len);
	return ret_len;
}
void pcmBuffer::set_dev(tDevPtr p_dev)
{
	pDev  = p_dev;
}


void pcmBuffer::read_from_ffstream(ffStream& fs)
{
	
	uint8_t buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
	// printf("start audio_decode_frame!!\n");
	int len = fs.audio_decode_frame(buf,AVCODEC_MAX_AUDIO_FRAME_SIZE);
	int count = 0;
	int total_size = 0;
	while(len > 0)
	{
		//printf("read frame size:%d\n",len);
		count++;
		
		db.push(buf,len);
		total_size += len;

		len = fs.audio_decode_frame(buf,AVCODEC_MAX_AUDIO_FRAME_SIZE);
	}
	this->len = total_size;
	// printf("databuffer pos:%d,size:%d,size2:%d,len:%d\n",
	// db.rpos,db.v.capacity(),db.v.size(),db.len);
	printf("frame count:%d,size:%d\n",count,total_size);

}


void pcmBuffer::play(bool _loop)
{
	if(pDev)
	{
		set_loop(_loop);
		if(!is_enable)
		{
			enable();
		}
		pDev->play();
	}
}
void pcmBuffer::stop()
{
	if(pDev)
	{
		if(is_enable)
		{
			disable();
		}
	}
}
int pcmBuffer::SetFormat(const AudioFormat* fm)
{
	format = *fm;
	if(pDev)
	{
		int ret = pDev->init_spec(fm->nSamplesPerSec,fm->nChannels);
		return ret;
	}
	return 0;
}
int pcmBuffer::GetFormat(AudioFormat* fm)
{
	*fm = format;
	return 0;
}

//implent audioDevice
void audioDevice::play()
{
	if(!is_playing)
	{
		printf("do playing!!!\n");
		// SDL_PauseAudioDevice(dev,0);
		SDL_PauseAudio(0);
		is_playing = true;
	}
}

void audioDevice::stop()
{
	if(is_playing)
	{
		// SDL_PauseAudioDevice(dev,1);
		SDL_PauseAudio(1);
		is_playing = false;
	}
}

void audioDevice::on_enable(tPcmPtr pcm)
{
	auto find_it = std::find(pcms.begin(),pcms.end(),pcm);
	if(find_it!=pcms.end() && pcm->is_enable)
	{
		nplaying++;
	}
}
void audioDevice::on_disable(tPcmPtr pcm)
{
	auto find_it = std::find(pcms.begin(),pcms.end(),pcm);
	if(find_it!=pcms.end() && !pcm->is_enable)
	{
		nplaying--;
		if(nplaying==0)
		{
			stop();
		}
	}
}
void audioDevice::disable_all()
{
	for(auto it:pcms)
	{
		it->disable();
	}
}

void audioDevice::add_pcm(tPcmPtr& pcm)
{
	pcms.push_back(pcm);
	pcm->set_dev(shared_from_this());
	if(pcm->is_enable)
	{
		nplaying++;
	}
}
void audioDevice::rm_pcm(tPcmPtr& pcm)
{
	auto find_it = std::find(pcms.begin(),pcms.end(),
	pcm);
	if(find_it!=pcms.end())
	{
		//pcms.remove(find_it);
		if(pcm->is_enable)
		{
			on_disable(pcm);
		}
		pcms.erase(find_it);
	}
}


int audioDevice::init_spec(int freq,uint8_t channels)
{
	if(is_init_spec)
		return 0;
	SDL_zero(want_spec);
	want_spec.freq = freq;
	want_spec.format = AUDIO_S16SYS; //16bit system byte order	// want_spec.format = AUDIO_F32SYS;
	want_spec.channels = channels;
	want_spec.samples = 4096;
	want_spec.callback = audioDevice::audio_cb;
	want_spec.userdata = this;

    /* Initialize call back variables */ 
    // dev = SDL_OpenAudioDevice(NULL,0,&want_spec, &get_spec,
    // 	  SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    // if (dev == 0) 
    if(SDL_OpenAudio(&want_spec, &get_spec) < 0)
    {  
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio: %s\n", SDL_GetError());  
        return -1;
    } 
    else
    {
    	printf("open dev id:%d\n",dev);
    	if(want_spec.format!=get_spec.format)
    	{
    		SDL_Log("din't get same format!!!\n");
    		printf("want_spec:%0x,get_spec:%0x\n",
    		want_spec.format,get_spec.format
    		);
    	}
    	// SDL_PauseAudioDevice(dev, 1);
    	// SDL_PauseAudio(1);
    }
    is_init_spec = true;
    return 0;
}

void SDLCALL
audioDevice::audio_cb(void* userdata, uint8_t* stream,int len)
{
	// printf("audio_cb:len:%d\n",len);
	audioDevice* _this = static_cast<audioDevice*>(userdata);
	SDL_memset(stream,0,len);

	if(_this->pcms.size()==0 || !_this->is_playing)
	{
		// printf("stop !!\n");
		_this->stop();
		return;
	}
	for(auto it:_this->pcms)
	{
		// printf("enable!!!:%d\n",!it.is_enable);
		if(!it->is_enable)
			continue;
		uint8_t* mixData = NULL;
		int _len = it->get_data(&mixData,len);
		// printf("cpy data!!!:%d,%d\n",_len,len);
		if(_len < len)
		{
			SDL_MixAudioFormat(stream, mixData, _this->get_spec.format,
		 		_len, SDL_MIX_MAXVOLUME);
			// _this->stop();
			if(!it->is_loop)
				it->disable();
			else
				it->reset();
			continue;
		}
		SDL_MixAudioFormat(stream, mixData, _this->get_spec.format,
		 len, SDL_MIX_MAXVOLUME);
		// SDL_memcpy(stream,mixData,_len);

		// break;
	}

}

void audioDevice::create_pcm_buffer(tPcmPtr& pcm)
{
	auto _p = std::make_shared<pcmBuffer>();
	pcm = _p;
}

void audioDevice::dump_info()
{
	// printf("filename:%s\n",filename.c_str());
	printf("freq:%d\n",get_spec.freq);
    printf("format:%d\n",get_spec.format);
    printf("channels:%d\n",get_spec.channels);
    printf("silence:%d\n",get_spec.silence);
    printf("samples:%d\n",get_spec.samples);
    printf("size:%d\n",get_spec.size);
}



class waveInfo
{
public:
	SDL_AudioSpec spec;
	uint8_t *data;
	uint32_t len;
	int pos;
	std::string filename;

public:
	// waveInfo();
	waveInfo(std::string fname);
	~waveInfo();
	//void init();
	void play();
	void stop();
	static void SDLCALL
	audio_cb(void* unused, Uint8* stream,int len);
	void dump_info();
};

waveInfo::waveInfo(std::string fname)
{
	filename = fname;
	data = NULL;
	len = 0;
	if(SDL_LoadWAV(fname.c_str(), &spec, &data, &len)==NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load: %s\n", SDL_GetError());  	
	}
	printf("wave info:size:%d\n",len);
	pos = 0;

	spec.callback = waveInfo::audio_cb;//设置回调函数
	spec.userdata = this;
	spec.channels = 1;
	// spec.format = AUDIO_F32SYS;


    /* Initialize call back variables */  
    if (SDL_OpenAudio(&spec, NULL) < 0) {  
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio: %s\n", SDL_GetError());  
        SDL_FreeWAV(data);  
    }  
}
waveInfo::~waveInfo()
{
	if(data)
		;//SDL_FreeWAV(data);
}

void SDLCALL 
waveInfo::audio_cb(void* userdata, Uint8* stream,int len)
{
	waveInfo* _this = static_cast<waveInfo*>(userdata);
    Uint8 *waveptr;  
    int waveleft;  
    // printf("callback len:%d\n",len);  
    /* Set up the pointers */  
    waveptr = _this->data + _this->pos;  
    waveleft = _this->len - _this->pos;  
  
    /* Go! */  
    if (waveleft <= len) {  
        SDL_memcpy(stream, waveptr, waveleft);  
        // SDL_PauseAudio(1);
        _this->stop(); 
        return;  
    }  
    SDL_memcpy(stream, waveptr, len);  
    _this->pos += len;  
}

void waveInfo::play()
{
 	SDL_PauseAudio(0);
}
void waveInfo::stop()
{
	 SDL_PauseAudio(1);
}

void waveInfo::dump_info()
{
	// printf("filename:%s\n",filename.c_str());
	printf("freq:%d\n",spec.freq);
    printf("format:%d\n",spec.format);
    printf("channels:%d\n",spec.channels);
    printf("silence:%d\n",spec.silence);
    printf("samples:%d\n",spec.samples);
    printf("size:%d\n",spec.size);
}

// tDevPtr g_audio_dev = nullptr;
// tPcmPtr main_buffer = nullptr;

tDevPtr create_sdl_auido_dev()
{
	tDevPtr dev = std::make_shared<audioDevice>();
	// int ret = dev->init_spec(DEFAUTL_FREQ, DEFAUTL_CHANNELS);
	return dev;
}

int play_wav()
{
    // char filename[4096]="a0001.wav";  
  
    /* Enable standard application logging */  
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);  
  
    /* Load the SDL library */  
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {  
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());  
        return -1;  
    }  
  
    /* Show the list of available drivers */  
    SDL_Log("Available audio drivers:");  
    for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i) {  
        SDL_Log("%i: %s", i, SDL_GetAudioDriver(i));  
    } 
    // waveInfo wav("a1.wav");
    // check_data = wav.data;

    //show list
    int count = SDL_GetNumAudioDevices(0);
    SDL_Log("Play Audio device:");
    for(int i=0;i<count;++i)
    {
    	SDL_Log("%i: %s",i,SDL_GetAudioDeviceName(i,0));
    }
  

    
    // ffStream ffs("../../voice/a0001.ogg");

    ffStream ffs("../../data/bgm/ＢＧＭ／通常１.ogg");
    ffStream ffs2("../../voice/a0001.ogg");
    ffStream ffs3("../../data/井内啓二 - 英雄願望 〜アルゴイゥ卜~.mp3");
    // ffStream ffs("a1.wav");
	// ffs.open_audio_stream();
	// ffs2.open_audio_stream();

	auto pbuf = std::make_shared<pcmBuffer>();
	auto pbuf2 = std::make_shared<pcmBuffer>();
	auto pbuf3 = std::make_shared<pcmBuffer>();

	// pcmBuffer pbuf;
	pbuf->read_from_ffstream(ffs);
	pbuf2->read_from_ffstream(ffs2);
	pbuf3->read_from_ffstream(ffs3);
	// pbuf3->read_from_ffstream(ffs3);

	auto audio_dev = std::make_shared<audioDevice>();
	// audio_dev->init_spec(ffs.freq, ffs.channels);


	audio_dev->init_spec(DEFAUTL_FREQ, DEFAUTL_CHANNELS);
	// g_audio_dev  = audio_dev;

	// pbuf2->enable();
	// pbuf2->set_loop(true);

	// pbuf3->enable();

	// pbuf->enable();


	audio_dev->add_pcm(pbuf);
	audio_dev->add_pcm(pbuf2);
	audio_dev->add_pcm(pbuf3);

	// main_buffer = pbuf;
	// main_buffer->enable();

	audio_dev->dump_info();

	// main_buffer->play(true);

	// audio_dev->play();



  
    SDL_Log("Using audio driver: %s\n", SDL_GetCurrentAudioDriver());  

    
    // printf("x:%d,len:%d,diff:%d\n",x,len,c);
    // wav.data = pbuf.db.get_data();
    // // wav.data = check_data;
    // wav.len = pbuf.db.len;
    // wav.dump_info();

    // wav.play();

    while ((SDL_GetAudioStatus() == SDL_AUDIO_PLAYING))//获取音频状态  
        SDL_Delay(1000);  
  
    /* Clean up on signal */  
    SDL_CloseAudio();//关掉音频进程以及音频设备  
    // SDL_FreeWAV(wave.sound);//释放数据由SDL_LoadWAV申请的  
    SDL_Quit();  
    // printf("=========over==========\n");  
    // system("pause");  
    return 0;
}
