
#include "ffStream.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libswresample/swresample.h>
//#include <libavutil/opt.h>
//#include <libavformat/avio.h>
//#include <libavutil/timestamp.h>
//#include <libavutil/mathematics.h>

#define LOGE printf
#define LOGI printf

int init_ffmpeg();

#ifdef __cplusplus
}
#endif
extern uint32_t time_now();

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

    // LOGI("len:%d,chs:%d,dst_bufsize:%d,fmt:%d\n",
    // 	len,targetAudioParams->channels,dst_bufsize,
    // 	targetAudioParams->sample_fmt
    // 	);
    return dst_bufsize;
}

void audio_swr_resampling_audio_destory(SwrContext **swr_ctx){
    if(*swr_ctx){
        swr_free(swr_ctx);
        *swr_ctx = NULL;
    }
}

int ffStream::open_audio()
{
	assert(audioStream!=-1);

	AVCodecContext *aCodecCtxOrig;
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
    printf("open auido stream orig:\nchannels:%d\nfreq:%d\nformat:%s\nlayout:%llu\n",
		channels,freq,av_get_sample_fmt_name(format),
		aCodecCtx->channel_layout
		);
	
	target_params = {
		aCodecCtx->sample_rate,
		aCodecCtx->channels,
		aCodecCtx->channel_layout,
		AV_SAMPLE_FMT_S16};

	audio_swr_resampling_audio_init(&swr,
	&target_params,aCodecCtx);

	
	if(pdb==nullptr)
	{
		pdb = std::make_shared<DataBuffer>();
	}
	if(!pdecoder)
	{
		pdecoder = std::make_shared<FFDecoder>(pdb,&target_params,aCodecCtx,swr);
	}
	return 0;
}

int ffStream::open_video()
{
	assert(videoStream != -1);
	AVCodecContext *vCodecCtxOrig;
	vCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
	vCodec = avcodec_find_decoder(vCodecCtxOrig->codec_id);
	if(vCodec == NULL)
	{
		fprintf(stderr, "Unsupported codec!\n");
    	return -1; // Codec not found
	}
	 // Copy context
	vCodecCtx = avcodec_alloc_context3(vCodec);
	if(avcodec_copy_context(vCodecCtx, vCodecCtxOrig) != 0)
	{
    	fprintf(stderr, "Couldn't copy codec context");
    	return -1; // Error copying codec context
    }

      // Open codec
	if(avcodec_open2(vCodecCtx, vCodec, NULL)<0)
		return -1; // Could not open codec

	if(!pvbuf)
		pvbuf = std::make_shared<FrameBuffer>();

	if(!pvdecoder)
		pvdecoder = std::make_shared<FFVDecoder>(pvbuf,vCodecCtx);


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
		auto _ct = pFormatCtx->streams[i]->codec->codec_type;
	 	if(audioStream==-1 && _ct == AVMEDIA_TYPE_AUDIO)
	 	{
	    	audioStream=i;
	    	// break;
	   	}
	   	if(videoStream==-1 && _ct ==  AVMEDIA_TYPE_VIDEO)
	   	{
	   		videoStream = i;
	   	}
	  }
  	if(audioStream==-1 && videoStream == -1)
	  	return -1; 
	if(audioStream!=-1)
	{
		int ret = open_audio();
		if(ret<0) return ret;
	}
	if(videoStream!=-1)
	{
		int ret = open_video();
		if(ret<0) return ret;
	}
	if(audioStream !=-1 || videoStream!=-1)
	{
		uint32_t start_tick = SDL_GetTicks();
		read_all_packet();
		printf("===%s:read packet cost:%u ms\n",fname.c_str(),SDL_GetTicks()-start_tick);

	}

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
	    printf("Couldn't open input stream file:%s\n",fname.c_str());  
	    return -1;  
	 }
	if(!pFormatCtx)
		return -1;

	int r = open_audio_stream();
	has_open = true;

	if(is_decode_all)
	{
		uint32_t start_tick = SDL_GetTicks();
		decode_all();
		printf("===%s:decode_all cost:%u ms\n",fname.c_str(),SDL_GetTicks()-start_tick);

	}

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
    	printf("error allock avio buffer fail!!!\n");
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
	assert(r == 0);
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
    		on_packet(packet);
    		// printf("packet size:%d\n",packet.size);
  		}
  		else if (packet.stream_index == videoStream)
  		{
  			video_packet_q.push(packet);
  			on_packet(packet);
  		}
  		else
  		{
    		av_free_packet(&packet);
    	}
    }
    printf("==read auido packets:%d\n",packet_q.size());
    printf("==read video packets:%d\n", video_packet_q.size());
}


static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
	// static int nb_samples = 0;
	// static int total_time = 0;
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    // printf("===time_base:%d/%d\n",time_base->num,time_base->den);
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
    // nb_samples += pkt->duration;
    // total_time += (pkt->duration * time_base->num * 1000) / (time_base->den);
    // printf("====nb_samples:%d,total_time:%dms\n",
    // nb_samples,total_time);
}
static uint32_t get_pkt_duration(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
	AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
	return (pkt->duration * time_base->num * 1000) / (time_base->den);
}
static uint32_t get_pkt_samples(const AVPacket* pkt)
{
	return pkt->duration;
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
    // printf("==pkt info:pts:%d,dts:%d,duration:%d,pos:%d\n",
    // pkt.pts,pkt.dts,pkt.duration,pkt.pos);
    // log_packet(pFormatCtx,&pkt);
    // printf("frame_size:%d,frame_number:%d\n", aCodecCtx->frame_size,
    // aCodecCtx->frame_number
    // );


    audio_pkt_data = pkt.data;
    audio_pkt_size = pkt.size;
  }
}

void ffStream::decode_all()
{
	int size = 0;
	size = decode_one();
	// printf("(decode_one:%d)\n", size);
	while(!decoded_end)
	{
		size = decode_one();
		// printf("(decode_one:%d)\n", size);
	}
	// if(!pdb)
	// {
	// 	pdb = std::make_shared<DataBuffer>();
	// }
	
	// uint8_t buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];

	// int len = audio_decode_frame(buf,MAX_AUDIO_FRAME_SIZE);
	// int count = 0;
	// int total_size = 0;
	// while(len > 0)
	// {
	// 	count++;
		
	// 	pdb->push(buf,len);
	// 	total_size += len;

	// 	len = audio_decode_frame(buf,MAX_AUDIO_FRAME_SIZE);
	// }
	// decode_data_size = total_size;
	// // printf("databuffer pos:%d,size:%d,size2:%d,len:%d\n",
	// // db.rpos,db.v.capacity(),db.v.size(),db.len);
	// decode_data = pdb->get_data();
	// printf("frame count:%d,size:%d\n",count,total_size);

}

int ffStream::decode_one()
{
	if(decoded_end) return 0;
	if(!pdecoder) return 0;
	if(!pdb) return 0;

	if(packet_q.empty())
	{
		decoded_end  = true;
		return 0;
	}


	AVPacket p;
	packet_q.wait_and_pop(p);
	// log_packet(pFormatCtx,&p);
	

	int len =  pdecoder->decode_one_pkt(p);
	if(len > 0){
//		decoded_size += len;
		// printf("===decode one :%d\n",len);
	} 
	return len;
}

int ffStream::video_decode_nframe(int nframe)
{
	if(video_decoded_end) return 0;
	if(!pvbuf) return 0;
	if(!pvdecoder) return 0;

	if(video_packet_q.empty())
	{
		video_decoded_end  = true;
		return 0;
	}
	int cnt = 0;
	AVPacket p;
	while(1)
	{
		video_packet_q.wait_and_pop(p);
		int r = pvdecoder->decode_one_pkt(p);
		if(r>0)
			cnt+=r;
		if(cnt >= nframe)
			break;
		if(video_packet_q.empty())
		{
			video_decoded_end = true;
			break;
		}
	}
	printf("video decode %d frame\n", cnt);
	printf("cur_frame:%d\n", vCodecCtx->frame_number);
	return cnt;
}


void ffStream::on_packet(AVPacket& pkt)
{
	if(pkt.stream_index == audioStream)
	{
		total_time += get_pkt_duration(pFormatCtx,&pkt);
		n_src_samples += get_pkt_samples(&pkt);
	}
	else if(pkt.stream_index == videoStream)
	{
		// log_packet(pFormatCtx,&pkt);
		// total_frame += 
		auto av_stream = pFormatCtx->streams[pkt.stream_index];
		auto frame_new_base = av_div_q(av_stream->time_base,vCodecCtx->time_base);
		total_frame += pkt.duration * frame_new_base.num / frame_new_base.den;
	}
	// log_packet(pFormatCtx,&pkt);

}

ffStream::~ffStream()
{
	if(pFormatCtx)
		avformat_close_input(&pFormatCtx);
	// if(aCodecCtxOrig)
	// 	avcodec_close(aCodecCtxOrig);
	if(aCodecCtx)
  		avcodec_close(aCodecCtx);

	if (p_avio_ctx){
        av_freep(&p_avio_ctx->buffer);
        av_freep(&p_avio_ctx);
    }
    if(swr)
    	swr_free(&swr);
}

//=========ff decoder==================================
int FFDecoder::decode(uint8_t* audio_buf, int buf_size)
{
	return 0;
}

int FFDecoder::decode_one_pkt(AVPacket& _pkt)
{
	pkt = _pkt;
	int len1;
	int data_size = 0;
	int got_frame = 0;
	audio_pkt_data = pkt.data;
	audio_pkt_size = pkt.size;
	AVFrame* frame = av_frame_alloc();
	int decoded_size = 0;

	while(audio_pkt_size  > 0)
	{
		av_frame_unref(frame);
		len1 = avcodec_decode_audio4(aCodecCtx, frame, 
			&got_frame, &pkt);
		if(len1 < 0)
		{
			// audio_pkt_size = 0;
			break;
		}
		audio_pkt_data += len1;
		audio_pkt_size -= len1;
		data_size = 0;
		if(got_frame)
		{
			data_size = av_samples_get_buffer_size(NULL, 
					       aCodecCtx->channels,
					       frame->nb_samples,
					       aCodecCtx->sample_fmt,
					       1);
			// printf("===got_frame:%d,nb_samples:%d\n", got_frame,frame->nb_samples);
			if(swr)
			{
				allocat_buffer(frame->nb_samples);
				data_size = audio_swr_resampling_audio(swr,
					target_params,frame, frame_buf);

				pdb->push(*frame_buf,data_size);
			}
			else
			{
				pdb->push(frame->data[0],data_size);

			}
			// printf("===data_size:%d\n",data_size);
			if(data_size <= 0)
			{
			/* No data yet, get more frames */
				continue;
			}
			// assert(data_size > 0);
			decoded_size += data_size;
		}

	}
	//process one packet end.
	if(pkt.data)
	{
		av_free_packet(&pkt);
	}
	return decoded_size;
}

//ff video decoder

int FFVDecoder::decode_one_pkt(AVPacket& _pkt)
{
	pkt = _pkt;
	video_pkt_data = pkt.data;
	video_pkt_size = pkt.size;


//use for scaling
	uint8_t *dst_data[4];
    int dst_linesize[4];
    // AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;
    // AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGBA;
    AVPixelFormat dst_pix_fmt = AV_PIX_FMT_BGRA;
    int dst_w, dst_h;
    int dst_bufsize;

	AVFrame* pFrame =av_frame_alloc();
	// AVFrame* pFrameRGB=av_frame_alloc();
	if(pFrame == NULL)
	{
		printf("frame alloc fail!\n");
		return -1;
	}

	// int numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
	// 		      pCodecCtx->height);
	// uint8_t* buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  	// avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
		 // pCodecCtx->width, pCodecCtx->height);
	dst_w = pCodecCtx->width;
	dst_h = pCodecCtx->height;
	// printf("width:%d,height:%d\n", dst_w,dst_h);
  
  // initialize SWS context for software scaling
  	if(sws_ctx==NULL)
  	{
  		sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,pCodecCtx->pix_fmt,
		   dst_w,dst_h,dst_pix_fmt,
		   SWS_BILINEAR,NULL,NULL,NULL);
  		if(!sws_ctx){
	        fprintf(stderr,
	                "Impossible to create scale context for the conversion "
	                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
	                av_get_pix_fmt_name(pCodecCtx->pix_fmt), pCodecCtx->width, pCodecCtx->height,
	                av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h);
	        return -1;
    	}
  	}
  	int decoded_frame = 0;
  	int ret;
  	if ((ret = av_image_alloc(dst_data, dst_linesize,
                              dst_w, dst_h, dst_pix_fmt, 1)) < 0)
  	{
        fprintf(stderr, "Could not allocate destination image\n");
        return -1;
    }
    dst_bufsize = ret;

    ret = avcodec_send_packet(pCodecCtx, &pkt);
    if(ret < 0)
    {
    	fprintf(stderr, "Error submitting the packet to the decoder\n");
        return -1;
    }
    while(ret >= 0)
    {
    	av_frame_unref(pFrame);
    	// Decode video frame

    	ret = avcodec_receive_frame(pCodecCtx, pFrame);
        if (ret == AVERROR_EOF)
        {
        	return -1;
        }
        else if(ret == AVERROR(EAGAIN))
        {
        	break;
        }
	    else if (ret < 0)
	    {
	    	fprintf(stderr, "Error during decoding\n");
            return -1;
	    }

	    // printf("decoded frame %3d\n", pCodecCtx->frame_number);	      


		// Convert the image from its native format to RGB
		
		//reversal image use of bmp.
        pFrame->data[0] += pFrame->linesize[0] * (pCodecCtx->height - 1);   
        pFrame->linesize[0] *= -1;   
        pFrame->data[1] += pFrame->linesize[1] * (pCodecCtx->height / 2 - 1);   
        pFrame->linesize[1] *= -1;   
        pFrame->data[2] += pFrame->linesize[2] * (pCodecCtx->height / 2 - 1);   
        pFrame->linesize[2] *= -1;   


		sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
				  pFrame->linesize, 0, pCodecCtx->height,
				  dst_data, dst_linesize);
		decoded_frame++;

		// printf("video decode one:%d,frame_linesize:%d,dst_linesize:%d\n",dst_bufsize, pFrame->linesize[0],dst_linesize[0]);
		pfb->push_frame(dst_data[0], dst_w * dst_h * 4);
		// char fname[1024];
		// sprintf(fname,"img%d.ppm",pCodecCtx->frame_number);
		// FILE *dst_file = fopen(fname, "wb");
	 //    if (!dst_file) {
	 //        fprintf(stderr, "Could not open destination file %s\n", fname);
	 //        return -1;
	 //    }
	 //      // Write header
  // 		fprintf(dst_file, "P6\n%d %d\n255\n", dst_w, dst_h);
	 //   	fwrite(dst_data[0], 1, dst_bufsize, dst_file);

	}
	// pdb->push(dst_data[0],dst_bufsize);
	        /* write scaled image to file */
	// std::string fname = "img";

	// Free the packet that was allocated by av_read_frame
	av_free_packet(&pkt);

	av_freep(&dst_data[0]);
	// sws_freeContext(sws_ctx);
  
  // Free the YUV frame
	av_frame_free(&pFrame);
  
  // Close the codecs
  // avcodec_close(pCodecCtx);
  // avcodec_close(pCodecCtxOrig);

  // // Close the video file
  // avformat_close_input(&pFormatCtx);
	return decoded_frame;
}
int FFVDecoder::decode(uint8_t* video_buf, int buf_size)
{
	return 0;
}


//---------------------
int init_ffmpeg()
{
	av_register_all();
	// avformat_network_init();
 
}
