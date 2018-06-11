#ifndef _FF_STREAM_H_
#define _FF_STREAM_H_
#include <string>

#include "SDL.h"
#include <string>
#include <safeQueue.hpp>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

#define LOGE printf
#define LOGI printf

const int DEFAUTL_CHANNELS = 2;
const int DEFAUTL_FREQ = 44100;

#define MAX_AUDIO_FRAME_SIZE (192000)

// extern int audio_swr_resampling_audio(SwrContext *swr_ctx,
// TargetAudioParams *targetAudioParams,
// AVFrame *audioFrame,
// uint8_t **targetData);

// extern void audio_swr_resampling_audio_init(SwrContext **swr_ctx,
// TargetAudioParams *targetAudioParams,
// AVCodecContext *codec);

// extern void 
// audio_swr_resampling_audio_destory(SwrContext **swr_ctx);

#ifdef __cplusplus
}
#endif

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

	//read all;
	int read(uint8_t** data)
	{
		int read_len = len - rpos;
		*data = get_data() + rpos;
		rpos = len;
		return len;
	}
	void seek(int pos)
	{
		if(pos>=len)
			rpos = len;
		else
			rpos = pos;
	}
};

struct TargetAudioParams
{
	int sample_rate;
	int channels;
	uint64_t 	channel_layout;
	AVSampleFormat 	sample_fmt;
};

typedef SafeQueue<AVPacket> tPacketQueue;

class ffStream
{
private:
	std::string fname;
	AVFormatContext *pFormatCtx;
	AVCodecContext *aCodecCtx;
	AVCodec* aCodec;
	SwrContext *swr;
	int audioStream;
	TargetAudioParams target_params;
	SafeQueue<AVPacket> packet_q;
	bool has_open;

	uint8_t* decode_data;
	int decode_data_size;
	// DataBuffer db;
	std::shared_ptr<DataBuffer> pdb;


public:
	int freq;
	int channels;


public:
	ffStream(std::string name
	, bool is_decode_all=true
	):fname(name),
	pFormatCtx(NULL),aCodecCtx(NULL),aCodec(NULL),
	swr(NULL),
	audioStream(-1)
	{
		memset(&target_params,0,sizeof(target_params));
		freq = -1;
		channels = 0;
		has_open = false;
		decode_data = nullptr;
		decode_data_size = 0;
		pdb = nullptr;

		open_audio_stream();
		if(is_decode_all)
		{
			decode_all();
		}
	}
	std::shared_ptr<DataBuffer> get_decode_buffer()
	{
		return pdb;
	}
	int open_audio_stream();
	void read_all_packet();
	int audio_decode_frame(uint8_t* audio_buf, int buf_size);
	void decode_all();

};




#endif