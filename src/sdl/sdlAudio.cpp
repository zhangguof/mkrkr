#include "SDL.h"
#include "SDL_audio.h"
#include <string>
#include <cstdint>
#include <vector>
#include "ffStream.hpp"
#include <algorithm>
#include <memory>

#define AVCODEC_MAX_AUDIO_FRAME_SIZE (4096*2)

uint8_t *check_data = NULL;
extern void check_buf(uint8_t* v,uint8_t* buf, int len,int idx,int pos);
uint8_t test_data[4096*4027];

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
	// int read(uint8_t* data)
	// {
	// 	if(len==0)
	// 		return 0;
	// 	memcpy(data,get_data(),len);
	// 	rpos = len;
	// 	return len;
	// }
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
	void seek(int pos)
	{
		if(pos>=len)
		{
			rpos = len;
		}
		else
		{
			rpos = pos;
		}
	}
	//read all;
	// int read(uint8_t** data)
	// {
	// 	int read_len = len - rpos;
	// 	*data = get_data() + rpos;
	// 	rpos = len;
	// 	return len;
	// }

};
class audioDevice;
typedef std::shared_ptr<audioDevice> tDevPtr;

class pcmBuffer:
public std::enable_shared_from_this<pcmBuffer>
{
public:
	uint8_t* data;
	DataBuffer db;
	uint32_t len;
	int pos;
	bool is_enable;
	bool is_loop;
	tDevPtr pDev;

	pcmBuffer():data(NULL),len(0),
	pos(0),is_enable(false),is_loop(false),
	pDev(nullptr){}
	
	pcmBuffer(uint8_t* _buf,uint32_t _len):data(_buf),len(_len),
	pos(0),is_enable(false),is_loop(false),
	pDev(nullptr)
	{
		db.push(_buf,_len);
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
};

class audioDevice:
public std::enable_shared_from_this<audioDevice>
{
public:
	typedef std::shared_ptr<pcmBuffer> tPcmPtr;

	SDL_AudioSpec want_spec;
	SDL_AudioSpec get_spec;
	bool is_playing;
	int nplaying;
	// uint8_t* data;
	std::vector<tPcmPtr > pcms;
	SDL_AudioDeviceID dev;
	void init_spec(int freq,uint8_t channels);
	static void SDLCALL
	audio_cb(void* userdata, Uint8* stream,int len);
	

	audioDevice()
	{
		pcms.clear();
		nplaying = 0;
		is_playing = false;
	}
	void dump_info();
	void play();
	void on_enable(tPcmPtr pcm);
	void on_disable(tPcmPtr pcm);
	void disable_all();
	void stop();
	void add_pcm(tPcmPtr& pcm);
	void rm_pcm(tPcmPtr& pcm);

};


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





// void check_buf(uint8_t* v,uint8_t* buf, int len,int idx,int pos)
// {
// 	int cnt=0;
// 	if(!check_data)
// 		check_data = new uint8_t[4096*1024];
// 	printf("check_buf:%d,%d,%d\n",len,idx,pos);
// 	// memcpy(check_data+pos,buf,len);
// 	for(int i=0;i<len;++i)
// 	{
// 		if(v[pos+i] != buf[i])
// 		{
// 			cnt++;
// 		}
// 	}
// 	if(cnt>0)
// 	{
// 		printf("diff %d:cnt:%d\n",idx,cnt);
// 	}
// }

void pcmBuffer::read_from_ffstream(ffStream& fs)
{
	
	uint8_t buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	printf("start audio_decode_frame!!\n");
	int len = fs.audio_decode_frame(buf,AVCODEC_MAX_AUDIO_FRAME_SIZE);
	int count = 0;
	int total_size = 0;
	while(len > 0)
	{
		//printf("read frame size:%d\n",len);
		count++;
		
		db.push(buf,len);
		// check_buf(db.v.data(),buf,len,count-1,total_size);
		// memcpy(check_data+total_size,buf,len);
		total_size += len;
		len = fs.audio_decode_frame(buf,AVCODEC_MAX_AUDIO_FRAME_SIZE);
	}
	//diff here??????
	// printf("last check!!!======\n");
	// check_buf(db.v.data(),check_data,total_size,0,0);
	printf("databuffer pos:%d,size:%d,size2:%d,len:%d\n",
	db.rpos,db.v.capacity(),db.v.size(),db.len);
	printf("frame count:%d,size:%d\n",count,total_size);

}




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
	if(pcm->is_enable)
	{
		nplaying++;
		pcm->set_dev(shared_from_this());
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


void audioDevice::init_spec(int freq,uint8_t channels)
{
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
}

void SDLCALL
audioDevice::audio_cb(void* userdata, uint8_t* stream,int len)
{
	printf("audio_cb:len:%d\n",len);
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
    // ffStream ffs("a1.wav");
	ffs.open_audio_stream();
	ffs2.open_audio_stream();

	auto pbuf = std::make_shared<pcmBuffer>();
	auto pbuf2 = std::make_shared<pcmBuffer>();
	// pcmBuffer pbuf;
	pbuf->read_from_ffstream(ffs);
	pbuf2->read_from_ffstream(ffs2);

	auto audio_dev = std::make_shared<audioDevice>();
	// audio_dev->init_spec(ffs.freq, ffs.channels);
	audio_dev->init_spec(44100,2);

	pbuf2->enable();
	pbuf2->set_loop(true);

	pbuf->enable();

	audio_dev->add_pcm(pbuf);
	audio_dev->add_pcm(pbuf2);

	audio_dev->dump_info();

	audio_dev->play();



  
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