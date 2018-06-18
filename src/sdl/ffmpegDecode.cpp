
#include "ffStream.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavformat/avio.h>

#define LOGE printf
#define LOGI printf

int init_ffmpeg();

#ifdef __cplusplus
}
#endif

//stream --> Packets --> decode -->frame

void audio_swr_resampling_audio_init(SwrContext **swr_ctx,
TargetAudioParams *targetAudioParams,
AVCodecContext *codec)
{
    if(codec->sample_fmt == AV_SAMPLE_FMT_S16 || 
    codec->sample_fmt == AV_SAMPLE_FMT_S32 ||
    codec->sample_fmt == AV_SAMPLE_FMT_U8)
    {
        LOGE("codec->sample_fmt:%d\n",codec->sample_fmt);
        if(*swr_ctx)
        {
            swr_free(swr_ctx);
            *swr_ctx = NULL;
        }
        return;
    }
    if(*swr_ctx)
    {
        swr_free(swr_ctx);
    }
    *swr_ctx = swr_alloc();
    if(!*swr_ctx)
    {
        LOGE("swr_alloc failed");
        return;
    }

    /* set options */
    av_opt_set_int(*swr_ctx, "in_channel_layout",    codec->channel_layout, 0);
    av_opt_set_int(*swr_ctx, "in_sample_rate",       codec->sample_rate, 0);
    av_opt_set_sample_fmt(*swr_ctx, "in_sample_fmt", codec->sample_fmt, 0);
    av_opt_set_int(*swr_ctx, "out_channel_layout",    targetAudioParams->channel_layout, 0);
    av_opt_set_int(*swr_ctx, "out_sample_rate",       targetAudioParams->sample_rate, 0);
    av_opt_set_sample_fmt(*swr_ctx, "out_sample_fmt", targetAudioParams->sample_fmt, 0);// AV_SAMPLE_FMT_S16
    /* initialize the resampling context */
    int ret = 0;

    if ((ret = swr_init(*swr_ctx)) < 0) {

        LOGE("Failed to initialize the resampling context\n");

        if(*swr_ctx){
            swr_free(swr_ctx);
            *swr_ctx = NULL;
        }
        return;
    }
}

//转换(AV_SAMPLE_FMT_FLTP->AV_SAMPLE_FMT_S16) 

int audio_swr_resampling_audio(SwrContext *swr_ctx,
TargetAudioParams *targetAudioParams,
AVFrame *audioFrame,
uint8_t **targetData)
{
    int len = swr_convert(swr_ctx,targetData,audioFrame->nb_samples,
    (const uint8_t **)(audioFrame->extended_data),
    audioFrame->nb_samples);

    if(len < 0){
        LOGE("error swr_convert");
        return -1;
    }
	int dst_bufsize = len * targetAudioParams->channels * 
	av_get_bytes_per_sample(targetAudioParams->sample_fmt);

    // LOGI(" dst_bufsize:%d\n",dst_bufsize);
    return dst_bufsize;
}

void audio_swr_resampling_audio_destory(SwrContext **swr_ctx){
    if(*swr_ctx){
        swr_free(swr_ctx);
        *swr_ctx = NULL;
    }
}

//class ffStream
int ffStream::open_audio_stream()
{
	if(!pFormatCtx)
		return -1;

	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
	  return -1; // Couldn't find stream information
	}
	av_dump_format(pFormatCtx, 0, fname.c_str(), 0);
	// AVCodecContext *aCodecCtxOrig;
	for(int i=0; i<pFormatCtx->nb_streams; i++)
	{
	 	if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
	 	{
	    	audioStream=i;
	    	break;
	   	}
	  }
  	if(audioStream==-1)
	  	return -1; 
	aCodecCtxOrig = pFormatCtx->streams[audioStream]->codec;
	aCodec = avcodec_find_decoder(aCodecCtxOrig->codec_id);
	aCodecCtx = avcodec_alloc_context3(aCodec);
	if(avcodec_copy_context(aCodecCtx, aCodecCtxOrig) != 0) {
	  fprintf(stderr, "Couldn't copy codec context");
	  return -1; // Error copying codec context
	}
	avcodec_open2(aCodecCtx, aCodec, NULL);

	this->channels = aCodecCtx->channels;
	this->freq = aCodecCtx->sample_rate;

	AVSampleFormat format = aCodecCtx->sample_fmt;
	printf("open stream orig:\nchannels:%d\nfreq:%d\nformat:%s\nlayout:%d\n",
		channels,freq,av_get_sample_fmt_name(format),
		aCodecCtx->channel_layout
		);
		//resample.
	
	target_params = {
		aCodecCtx->sample_rate,
		// DEFAUTL_FREQ,
		// DEFAUTL_CHANNELS,
		aCodecCtx->channels,
		aCodecCtx->channel_layout,
		// (uint64_t)av_get_default_channel_layout(DEFAUTL_CHANNELS),
		AV_SAMPLE_FMT_S16
	};
	audio_swr_resampling_audio_init(&swr,
	&target_params,aCodecCtx);

	uint32_t start_tick = SDL_GetTicks();
	read_all_packet();
	printf("===%s:read packet cost:%u ms\n",fname.c_str(),SDL_GetTicks()-start_tick);

	has_open = true;

	return 0;
}
int ffStream::open_audio_file()
{
	if(has_open)
		return 0;
	pFormatCtx = avformat_alloc_context();

	if(avformat_open_input(&pFormatCtx,fname.c_str(),NULL,NULL)!=0)
	{  
	    printf("Couldn't open input stream.\n");  
	    return -1;  
	 }
	if(!pFormatCtx)
		return -1;

	int r = open_audio_stream();
	has_open = true;

	return r;
}


// static int read_packet_cb(void *stream, uint8_t *buf, int buf_size)
// {
//     struct buffer_data *bd = (struct buffer_data *)opaque;
//     buf_size = buf_size < bd->size?buf_size:bd->size;
//     if (!buf_size)
//         return AVERROR_EOF;
//     printf("ptr:%p size:%zu\n", bd->ptr, bd->size);
//     /* copy internal buffer data to buf */
//     memcpy(buf, bd->ptr, buf_size);
//     // bd->ptr  += buf_size;
//     // bd->size -= buf_size;
//     return buf_size;
// }


int ffStream::open_audio_stream_cb(void* stream,tReadPacketCB read_packet_cb)
{
	if(has_open)
		return 0;

	pFormatCtx = avformat_alloc_context();

	avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size);
	p_avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
               0, stream, read_packet_cb, NULL, NULL);
    if (!p_avio_ctx)
    {
    	return -1;
    }

    pFormatCtx->pb = p_avio_ctx;
   	if(avformat_open_input(&pFormatCtx,NULL,NULL,NULL)!=0)
	{  
	    printf("Couldn't open input stream.\n");  
	    return -1;  
	 }
	if(!pFormatCtx)
		return -1;

	int r = open_audio_stream();
	has_open = true;
	if(is_decode_all)
	{
		uint32_t now_tick = SDL_GetTicks();
		decode_all();
		SDL_Log("decode_all cost:%dms\n",SDL_GetTicks()-now_tick);
	}
	return r;
}
void ffStream::read_all_packet()
{
	//read all data
	AVPacket packet;
	while(av_read_frame(pFormatCtx, &packet)>=0)
	{
		if(packet.stream_index == audioStream) {
    		packet_q.push(packet);
    		// printf("packet size:%d\n",packet.size);
  		}
  		else
  		{
    		av_free_packet(&packet);
    	}
    }
    printf("==read packets:%d\n",packet_q.size());
}

//one frame decode.
static uint8_t** frame_buf = NULL;
const int max_samples = 8192;
int ffStream::audio_decode_frame(uint8_t *audio_buf,int buf_size)
{
  static AVPacket pkt;
  static uint8_t *audio_pkt_data = NULL;
  static int audio_pkt_size = 0;
  static AVFrame frame;

  int len1, data_size = 0;

  for(;;)
  {
    while(audio_pkt_size > 0)
    {
      int got_frame = 0;
      len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
      if(len1 < 0)
      {
		/* if error, skip frame */
		audio_pkt_size = 0;
		break;
      }
      audio_pkt_data += len1;
      audio_pkt_size -= len1;
      data_size = 0;
      if(got_frame)
      {
		data_size = av_samples_get_buffer_size(NULL, 
						       aCodecCtx->channels,
						       frame.nb_samples,
						       aCodecCtx->sample_fmt,
						       1);
		
		int dst_linesize;
		if(!frame_buf)
		{
			int ret = av_samples_alloc_array_and_samples(
			&frame_buf, &dst_linesize, 
			aCodecCtx->channels,max_samples,target_params.sample_fmt, 0);
			if(ret<0)
			{
				printf("swr_alloc fail!!!\n");
			}
		}
		if(swr)
		{
			data_size = audio_swr_resampling_audio(swr,
						&target_params,&frame, frame_buf);
			// assert(data_size <= buf_size);
			memcpy(audio_buf, *frame_buf, data_size);

		}
		else
		{
			// assert(data_size == frame.linesize[0]);
			memcpy(audio_buf, frame.data[0], data_size);
		}
		assert(data_size <= buf_size);
		
		// memcpy(audio_buf, *frame_buf, data_size);
      }
      if(data_size <= 0)
      {
		/* No data yet, get more frames */
		continue;
      }
      /* We have data, return it and come back for more later */
      return data_size;
    }

    if(pkt.data)
    {
    	av_free_packet(&pkt);
    }


    if(packet_q.empty())
    {
    	audio_swr_resampling_audio_destory(&swr);
    	return -1;
    }
    packet_q.wait_and_pop(pkt);


    audio_pkt_data = pkt.data;
    audio_pkt_size = pkt.size;
  }
}

void ffStream::decode_all()
{
	if(!pdb)
	{
		pdb = std::make_shared<DataBuffer>();
	}
	
	uint8_t buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];

	int len = audio_decode_frame(buf,MAX_AUDIO_FRAME_SIZE);
	int count = 0;
	int total_size = 0;
	while(len > 0)
	{
		count++;
		
		pdb->push(buf,len);
		total_size += len;

		len = audio_decode_frame(buf,MAX_AUDIO_FRAME_SIZE);
	}
	decode_data_size = total_size;
	// printf("databuffer pos:%d,size:%d,size2:%d,len:%d\n",
	// db.rpos,db.v.capacity(),db.v.size(),db.len);
	decode_data = pdb->get_data();
	printf("frame count:%d,size:%d\n",count,total_size);
}

ffStream::~ffStream()
{
	if(pFormatCtx)
		avformat_close_input(&pFormatCtx);
	if(aCodecCtxOrig)
		avcodec_close(aCodecCtxOrig);
	if(aCodecCtx)
  		avcodec_close(aCodecCtx);

	if (p_avio_ctx){
        av_freep(&p_avio_ctx->buffer);
        av_freep(&p_avio_ctx);
    }
    if(swr)
    	swr_free(&swr);
}



//---------------------
int init_ffmpeg()
{
	av_register_all();
	// avformat_network_init();
 
}
