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
#include <libswscale/swscale.h>

#include <libavutil/imgutils.h>

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
extern uint32_t time_now();

class FrameBuffer
{
public:
	int frame_size;
	int cur_pos;
	int len;
	struct Frame
	{
		uint8_t* data;
		Frame(uint8_t* _data,int _size)
		{
			data = new uint8_t[_size];
			memcpy(data,_data,_size);
		}
		~Frame()
		{
			delete[] data;
		}
	};

	FrameBuffer(int f_size)
	{
		frame_size = f_size;
		cur_pos = 0;
		len = 0;
	}
	FrameBuffer()
	{
		frame_size = -1;
		cur_pos = 0;
		len = 0;
	}
	std::vector<std::shared_ptr<Frame> > v;
	void push_frame(uint8_t* _data, int size)
	{
		if(frame_size == -1)
			frame_size = size;
		assert(size == frame_size);
		auto pf = std::make_shared<Frame>(_data,size);

		v.push_back(pf);
		++len;
	}
	int read(uint8_t** data, int size)
	{	
		// if(frame_size!=size)
		// 	printf("frame_size:%d,size:%d\n", frame_size,size);
		assert(frame_size == size);
		if(cur_pos >= len)
			return 0;
		*data = v[cur_pos++]->data;
		return size;
	}
	int seek(uint32_t pos)
	{
		if(pos > len)
			pos = len;
		cur_pos = pos;
		return pos;
	}
	int readable_frame()
	{
		return len - cur_pos;
	}
};

class DataBuffer
{
public:
	std::vector<uint8_t> v;
	int len;
	int rpos;
	bool locked;
	DataBuffer(){
		v.resize(4096);
		len = 0;
		rpos = 0;
		locked = false;
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
	void ensure_size(int size)
	{
		if(len + size > v.size())
		{
			v.resize(len + size);
		}
	}
	void push(uint8_t* data,int size)
	{
		//fix using size() better than capacity()!!!

		ensure_size(size);
		memcpy(get_tail(),data,size);
		len += size;
	}
	//24bits
	//1024*768, linesize = 1024*3
	// void push_pic_buf(uint8_t* data,int linesize,int w,int h)
	// {
	// 	int size = linesize * h;
	// 	uint32_t cur_t = time_now();
	// 	uint32_t start_tick = cur_t;
		
	// 	ensure_size(size);

	// 	cur_t = time_now();
	// 	// for(int i=0;i<h;++i)
	// 	// {
	// 	// 	push(data + i*linesize, linesize);
	// 	// }
	// 	memcpy(get_tail(),data,size);
	// 	len+=size;
	// }

	int readable_size()
	{
		return len - rpos;
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
	int fill(uint8_t* data,int n)
	{
		if(rpos == len)
			return 0;
		int read_len = n;
		if(n > len -rpos)
			read_len = len - rpos;
		memcpy(data,get_data()+rpos, read_len);
		rpos += read_len;
		return read_len;
	}
	void seek(int pos)
	{
		if(pos>=len)
			rpos = len;
		else
			rpos = pos;
	}
	void* new_buffer(int bytes)
	{
		ensure_size(bytes);
		return get_tail();
	}
	void reset()
	{
		rpos = 0;
		len = 0;
	}
	void* lock(int bytes)
	{
		if(locked) return nullptr;
		locked = true;
		ensure_size(bytes);
		return get_tail();
	}
	void unlock()
	{
		if(!locked) return;
		locked = false;
	}
};

typedef std::shared_ptr<DataBuffer> tPtrDB;

struct TargetAudioParams
{
	int sample_rate;
	int channels;
	uint64_t 	channel_layout;
	AVSampleFormat 	sample_fmt;
};

typedef SafeQueue<AVPacket> tPacketQueue;
typedef	int(*tReadPacketCB)(void *opaque, uint8_t *buf, int buf_size);

// struct Frame
// {
// 	int f_idx;
// 	int nSamples;
// 	Frame(int _idx,int n)
// 	{
// 		f_idx = _idx;
// 		nSamples = n;
// 	}
// 	~Frame()
// 	{
// 	}
// };

// struct FrameQueue
// {
// 	int nSample;
// 	int SampleSize;
// 	std::vector<Frame> v;
// 	std::shared_ptr<DataBuffer> pdb;

// 	FrameQueue(int size,std::shared_ptr<DataBuffer>& pdb)
// 	{
// 		SampleSize = size;
// 	}
// 	void push_frame(int _idx,int n_sample,uint8_t* buf,int size)
// 	{
// 		Frame f(_idx,n_sample);
// 		pdb.push(buf,size);
// 	}

// };

class FFDecoder
{
public:
	AVCodecContext *aCodecCtx;
	SwrContext *swr;
	AVPacket pkt;
	
	uint8_t* audio_pkt_data;
	int audio_pkt_size;
	uint8_t** frame_buf;
	int max_samples;
	TargetAudioParams* target_params;

	tPtrDB pdb;

	int allocat_buffer(int n_sample)
	{
		int ret = 0,dst_linesize;
		if(!frame_buf)
		{
			ret = av_samples_alloc_array_and_samples(
				&frame_buf, &dst_linesize, 
				aCodecCtx->channels,max_samples,
				target_params->sample_fmt, 0);
			// printf("====allocat_buffer:%d\n", ret);
		}
		else if(n_sample > max_samples)
		{
			av_freep(&frame_buf[0]);
			max_samples = n_sample;
			ret = av_samples_alloc(frame_buf, &dst_linesize, aCodecCtx->channels,
                 max_samples, target_params->sample_fmt, 1);
			// printf("=====re allocat_buffer:%d\n", ret);
		}

		if(ret<0)
		{
			printf("swr_alloc fail!!!:max_samples:%d\n",max_samples);
		}
		return ret;
	}

	FFDecoder(tPtrDB& _p,TargetAudioParams* pfmt,AVCodecContext* ctx, SwrContext* _swr=NULL)
	{
		aCodecCtx = ctx;
		swr = _swr;
		audio_pkt_data = nullptr;
		audio_pkt_size = 0;
		frame_buf = NULL;
		pdb = _p;
		max_samples = 8192;
		target_params = pfmt;

	}
	~FFDecoder()
	{
		if (frame_buf)
			av_freep(&frame_buf[0]);
		av_freep(&frame_buf);
	}
	int decode_one_pkt(AVPacket& _pkt);
	int decode(uint8_t* audio_buf, int buf_size);

};

class FFVDecoder
{
public:
	AVCodecContext *pCodecCtx;
	SwsContext *sws_ctx;
	AVPacket pkt;
	
	uint8_t* video_pkt_data;
	int video_pkt_size;
	uint8_t** frame_buf;
	// tPtrDB pdb;
	std::shared_ptr<FrameBuffer> pfb;

	FFVDecoder(std::shared_ptr<FrameBuffer>& _p,AVCodecContext* ctx, SwsContext* _sws=NULL)
	{
		pCodecCtx = ctx;
		sws_ctx = _sws;
		video_pkt_data = nullptr;
		video_pkt_size = 0;
		frame_buf = NULL;
		pfb = _p;
		//
		printf("video timebase:%d/%d,tick:%d\n", pCodecCtx->time_base.num,
		pCodecCtx->time_base.den,pCodecCtx->ticks_per_frame
		);
	}
	~FFVDecoder()
	{
		if (frame_buf)
			av_freep(&frame_buf[0]);
		av_freep(&frame_buf);
	}
	int decode_one_pkt(AVPacket& _pkt);
	int decode(uint8_t* video_buf, int buf_size);



};


class ffStream
{
private:
	std::string fname;
	AVFormatContext *pFormatCtx;
	AVCodecContext *aCodecCtx;
	// AVCodecContext *aCodecCtxOrig;

	AVCodec* aCodec;
	SwrContext *swr;
	int audioStream;
	// TargetAudioParams target_params;
	SafeQueue<AVPacket> packet_q;
	bool has_open;

	// uint8_t* decode_data;
	// int decode_data_size;
	// DataBuffer db;
	std::shared_ptr<DataBuffer> pdb;

	//stream api
	uint8_t* avio_ctx_buffer;
	AVIOContext* p_avio_ctx;
	const uint32_t avio_ctx_buffer_size = 4096;

	bool is_decode_all;
	
	//decode
	std::shared_ptr<FFDecoder> pdecoder;

private:
	//video 
	int videoStream;
	AVCodecContext* vCodecCtx;
	AVCodec* vCodec;
	std::shared_ptr<FrameBuffer> pvbuf;
	std::shared_ptr<FFVDecoder> pvdecoder;
	SafeQueue<AVPacket> video_packet_q;




public:
	int freq;
	int channels;
	
	TargetAudioParams target_params;
	const int BitsPerSample = 16;
	const int BytesPerSample = 2;
	uint64_t total_time; //ms
	uint64_t n_src_samples;

	bool decoded_end;
	bool video_decoded_end;
	// uint32_t read_pos;
	// uint32_t decoded_len;

public:
	//video
	int video_decode_nframe(int nframe = 1);
	double get_fps()
	{
		if(vCodecCtx)
		{
			return 1.0 / av_q2d(vCodecCtx->time_base) / FFMAX(vCodecCtx->ticks_per_frame, 1);
		}
		return 0.0;
	}
	int get_width()
	{
		if(vCodecCtx)
		{
			return vCodecCtx->width;
		}
		return 0;
	}
	int get_height()
	{
		if(vCodecCtx)
			return vCodecCtx->height;
		return 0;
	}





public:
	ffStream(std::string name
	, bool _is_decode_all=false
	):is_decode_all(_is_decode_all)
	{
		init();
		fname = name;
		open_audio_file();
	}
	ffStream(bool _is_decode_all = false):
	is_decode_all(_is_decode_all)
	{
		init();
	}
	void init()
	{
		fname = "";
		pFormatCtx = NULL;
		aCodecCtx = NULL;
		// aCodecCtxOrig = NULL;
		aCodec = NULL;
		swr = NULL;
		audioStream = -1;

		memset(&target_params,0,sizeof(target_params));

		freq = -1;
		channels = 0;

		avio_ctx_buffer = NULL;
		p_avio_ctx = NULL;
		pdb = nullptr;
		pdecoder = nullptr;
		decoded_end = false;
		total_time = 0;
		n_src_samples = 0;
		has_open = false;

		//video
		videoStream = -1;
		vCodecCtx = nullptr;
		vCodec = nullptr;
		pvbuf = nullptr;
		pvdecoder = nullptr;
		video_decoded_end = false;
		
	}
	std::shared_ptr<DataBuffer> get_decode_buffer()
	{
		return pdb;
	}
	std::shared_ptr<FrameBuffer>get_video_buffer()
	{
		return pvbuf;
	}
	int open_audio_stream();
	int open_audio_stream_cb(void* stream,tReadPacketCB read_packet_cb);
	int open_audio_file();

	void read_all_packet();
	int audio_decode_frame(uint8_t* audio_buf, int buf_size);
	void decode_all();
	int decode_one();

	void on_packet(AVPacket& pkt);
//audio
	int Read(void* buf, uint32_t readsize)
	{
		int has_read = 0;
		while(!decoded_end && readsize > pdb->readable_size())
		{
			decode_one();
		}
		has_read = pdb->fill((uint8_t*)buf,readsize);
		return has_read;
	}
	void SetPos(uint32_t pos)
	{
		// read_pos = pos;
		while(!decoded_end && pos > pdb->readable_size())
			decode_one();
		pdb->seek(pos);
	}
//video
	int GetFrame(uint8_t** data, int size)
	{
		// int next_pos = cur_pos + 1;
		while(!video_decoded_end && pvbuf->readable_frame()<1)
		{
			video_decode_nframe(5);
		}
		int r = pvbuf->read(data,size);
		if(r > 0)
		{
			// cur_pos = next_pos;
			return r;
		}
		return 0;
	}
	void SetFrame(uint32_t pos)
	{
		while(!video_decoded_end && pos > pvbuf->len)
		{
			video_decode_nframe(5);
		}
		pvbuf->seek(pos);
	}
public:
	int open_audio();
	int open_video();

	~ffStream();

};




#endif