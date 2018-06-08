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

public:
	int freq;
	int channels;


public:
	ffStream(std::string name):fname(name),
	pFormatCtx(NULL),aCodecCtx(NULL),aCodec(NULL),
	swr(NULL),
	audioStream(-1)
	{
		memset(&target_params,0,sizeof(target_params));
		freq = -1;
		channels = 0;
		has_open = false;
		open_audio_stream();
	}
	int open_audio_stream();
	void read_all_packet();
	int audio_decode_frame(uint8_t* audio_buf, int buf_size);

};




#endif