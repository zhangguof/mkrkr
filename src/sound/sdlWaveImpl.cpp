
#include "tjsCommHead.h"

// #include <mmsystem.h>
// #include <mmreg.h>
#include <math.h>
#include <algorithm>
// #include "SystemControl.h"
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "StorageIntf.h"
#include "WaveImpl.h"
// #include "PluginImpl.h"
#include "StaticPluginImpl.h"
#include "SysInitIntf.h"
#include "ThreadIntf.h"
#include "Random.h"
#include "UtilStreams.h"
// #include "TickCount.h"
#include "utils.h"


#include "TVPTimer.h"
#include "Application.h"
// #include "UserEvent.h"
#include "NativeEventQueue.h"
#include "ffStream.hpp"


//---------------------------------------------------------------------------
// Options management
//---------------------------------------------------------------------------
static tTVPThreadPriority TVPDecodeThreadHighPriority = ttpHigher;
static tTVPThreadPriority TVPDecodeThreadLowPriority = ttpLowest;
static bool TVPSoundOptionsInit = false;
static bool TVPControlPrimaryBufferRun = true;
static bool TVPUseSoftwareBuffer = true;
static bool TVPAlwaysRecreateSoundBuffer = false;
static bool TVPPrimaryFloat = false;
static tjs_int TVPPriamrySBFrequency = 44100;
static tjs_int TVPPrimarySBBits = 16;
static tTVPSoundGlobalFocusMode TVPSoundGlobalFocusModeByOption = sgfmNeverMute;
static tjs_int TVPSoundGlobalFocusMuteVolume = 0;
static enum tTVPForceConvertMode { fcmNone, fcm16bit, fcm16bitMono }
	TVPForceConvertMode = fcmNone;
static tjs_int TVPPrimarySBCreateTryLevel = -1;
static bool TVPExpandToQuad = false;
static tjs_int TVPL1BufferLength = 1000; // in ms
static tjs_int TVPL2BufferLength = 1000; // in ms
static bool TVPDirectSoundUse3D = false;
static tjs_int TVPVolumeLogFactor = 3322;
//---------------------------------------------------------------------------
static void TVPInitSoundOptions()
{
	if(TVPSoundOptionsInit) return;

	// retrieve options from commandline
/*
 ttpIdle = 0
 ttpLowest = 1
 ttpLower = 2
 ttpNormal = 3
 ttpHigher = 4
 ttpHighest = 5
 ttpTimeCritical = 6
*/

	tTJSVariant val;
	if(TVPGetCommandLine(TJS_W("-wsdecpri"), &val))
	{
		tjs_int v = val;
		if(v < 0) v = 0;
		if(v > 5) v = 5; // tpTimeCritical is dangerous...
		TVPDecodeThreadLowPriority = (tTVPThreadPriority)v;
		if(TVPDecodeThreadHighPriority<TVPDecodeThreadLowPriority)
			TVPDecodeThreadHighPriority = TVPDecodeThreadLowPriority;
	}

	if(TVPGetCommandLine(TJS_W("-wscontrolpri"), &val))
	{
		if(ttstr(val) == TJS_W("yes"))
			TVPControlPrimaryBufferRun = true;
		else
			TVPControlPrimaryBufferRun = false;
	}

	if(TVPGetCommandLine(TJS_W("-wssoft"), &val))
	{
		if(ttstr(val) == TJS_W("no"))
			TVPUseSoftwareBuffer = false;
		else
			TVPUseSoftwareBuffer = true;
	}

	if(TVPGetCommandLine(TJS_W("-wsrecreate"), &val))
	{
		if(ttstr(val) == TJS_W("yes"))
			TVPAlwaysRecreateSoundBuffer = true;
		else
			TVPAlwaysRecreateSoundBuffer = false;
	}

	if(TVPGetCommandLine(TJS_W("-wsfreq"), &val))
	{
		TVPPriamrySBFrequency = val;
	}

	if(TVPGetCommandLine(TJS_W("-wsbits"), &val))
	{
		ttstr sval(val);
		if(sval == TJS_W("f32"))
		{
			TVPPrimaryFloat = true;
			TVPPrimarySBBits = 32;
		}
		else if(sval[0] == TJS_W('i'))
		{
			TVPPrimaryFloat = false;
			TVPPrimarySBBits = TJS_atoi(sval.c_str() + 1);
		}
	}

	if(TVPGetCommandLine(TJS_W("-wspritry"), &val))
	{
		ttstr sval(val);
		if(sval == TJS_W("all"))
			TVPPrimarySBCreateTryLevel = -1;
		else
			TVPPrimarySBCreateTryLevel = val;
	}

	if(TVPGetCommandLine(TJS_W("-wsuse3d"), &val))
	{
		ttstr sval(val);
		if(sval == TJS_W("no"))
			TVPDirectSoundUse3D = false;
		else
			TVPDirectSoundUse3D = true;
	}

	if(TVPGetCommandLine(TJS_W("-wsforcecnv"), &val))
	{
		ttstr sval(val);
		if(sval == TJS_W("i16"))
			TVPForceConvertMode = fcm16bit;
		else if(sval == TJS_W("i16m"))
			TVPForceConvertMode = fcm16bitMono;
		else
			TVPForceConvertMode = fcmNone;
	}

	if(TVPGetCommandLine(TJS_W("-wsexpandquad"), &val))
	{
		if(ttstr(val) == TJS_W("yes"))
			TVPExpandToQuad = true;
		else
			TVPExpandToQuad = false;
	}
	if(TVPDirectSoundUse3D) TVPExpandToQuad = false;
		// quad expansion is disabled when using 3D sounds

	if(TVPGetCommandLine(TJS_W("-wsmute"), &val))
	{
		ttstr str(val);
		if(str == TJS_W("no") || str == TJS_W("never"))
			TVPSoundGlobalFocusModeByOption = sgfmNeverMute;
		else if(str == TJS_W("minimize"))
			TVPSoundGlobalFocusModeByOption = sgfmMuteOnMinimize;
		else if(str == TJS_W("yes") || str == TJS_W("deactive"))
			TVPSoundGlobalFocusModeByOption = sgfmMuteOnDeactivate;
	}

	if(TVPGetCommandLine(TJS_W("-wsmutevol"), &val))
	{
		tjs_int n = (tjs_int)val;
		if(n >= 0 && n <= 100) TVPSoundGlobalFocusMuteVolume = n * 1000;
	}

	if(TVPGetCommandLine(TJS_W("-wsl1len"), &val))
	{
		tjs_int n = (tjs_int)val;
		if(n > 0 && n < 600000) TVPL1BufferLength = n;
	}

	if(TVPGetCommandLine(TJS_W("-wsl2len"), &val))
	{
		tjs_int n = (tjs_int)val;
		if(n > 0 && n < 600000) TVPL2BufferLength = n;
	}

	if(TVPGetCommandLine(TJS_W("-wsvolfactor"), &val))
	{
		tjs_int n = (tjs_int)val;
		if(n > 0 && n < 200000) TVPVolumeLogFactor = n;
	}

	TVPSoundOptionsInit = true;
}
//---------------------------------------------------------------------------


// //---------------------------------------------------------------------------
static bool TVPTSSWaveDecoderCreatorRegistered = false;
class tTVPFFMPEGWaveDecoder_C;
extern tTVPFFMPEGWaveDecoder_C TVPTSSWaveDecoderCreator;

void TVPRegisterTSSWaveDecoderCreator()
{
	if(!TVPTSSWaveDecoderCreatorRegistered)
	{
		TVPRegisterWaveDecoderCreator((tTVPWaveDecoderCreator*)&TVPTSSWaveDecoderCreator);
		TVPTSSWaveDecoderCreatorRegistered = true;
	}
}
// //---------------------------------------------------------------------------



//-----ffmpeg decode -------------------------------
//---------------------------------------------------------------------------
// tTVPFFMPEGWaveDecoder
//---------------------------------------------------------------------------
class tTVPFFMPEGWaveDecoder : public tTVPWaveDecoder
{
	ffStream* p_ffstream;
	// uint8_t* mem_stream;
	// int mem_size;

	tTJSBinaryStream *Stream;
	tTVPWaveFormat Format;
	tjs_uint64 DataStart;
	tjs_uint64 CurrentPos;
	tjs_uint SampleSize;

public:
	tTVPFFMPEGWaveDecoder(ffStream* ps,tTJSBinaryStream *stream, tjs_uint64 datastart,
		const tTVPWaveFormat & format);
	~tTVPFFMPEGWaveDecoder();
	void GetFormat(tTVPWaveFormat & format);
	bool Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered);
	bool SetPosition(tjs_uint64 samplepos);

static int read_packet_cb(void* opaque, uint8_t* buf, int buf_size);
};
//---------------------------------------------------------------------------
tTVPFFMPEGWaveDecoder::tTVPFFMPEGWaveDecoder(ffStream* ps,tTJSBinaryStream *stream, tjs_uint64 datastart,
	const tTVPWaveFormat & format)
{
	p_ffstream = ps;
	Stream = stream;

	p_ffstream->open_audio_stream_cb((void*)this, 
	tTVPFFMPEGWaveDecoder::read_packet_cb);

	printf("===go to get pdb!!!\n");
	auto pdb = p_ffstream->get_decode_buffer();

	assert(pdb!=nullptr);

	// mem_stream = pdb->get_data();
	// mem_size = pdb->len;

	

	DataStart = datastart;

	//format init.
	Format = format;
	Format.Channels = p_ffstream->channels;
	Format.SamplesPerSec = p_ffstream->freq; //use src freq
	Format.BitsPerSample = p_ffstream->BitsPerSample;
	Format.BytesPerSample = p_ffstream->BytesPerSample;

	SampleSize = Format.BytesPerSample * Format.Channels;


	// Format.TotalSamples = mem_size/SampleSize;
	Format.TotalSamples = p_ffstream->n_src_samples;

	// assert(mem_size % SampleSize == 0);
	// printf("=====ffmpeg nSamples:%lu,%lu,%lu,44100Hz:%lu,freq:%u\n", 
	// 	Format.TotalSamples,p_ffstream->n_src_samples,
	// 	p_ffstream->total_time * p_ffstream->freq / 1000,
	// 	p_ffstream->n_src_samples * 44100 / p_ffstream->freq,
	// 	p_ffstream->freq
	// 	);

	// printf("=====ffmpeg deocde:size:%d,samples:%d,SampleSize:%d\n",
	// mem_size,Format.TotalSamples,SampleSize);
	// if(mem_size%SampleSize != 0)
	// {
	// 	Format.TotalSamples++;
	// }
	//time * freq = nSamples
	// Format.TotalTime = Format.TotalSamples * 1000 /(Format.SamplesPerSec);
	Format.TotalTime = p_ffstream->total_time;
	// printf("=====ffmpeg:total_time:%ums,%ums",
	// Format.TotalSamples * 1000 /(Format.SamplesPerSec),
	// Format.TotalTime);


	Format.SpeakerConfig = 0;
	Format.IsFloat = false;

	printf("dump format:\n");
	Format.dump();


	
	CurrentPos = 0;
	// Stream->SetPosition(DataStart);
	p_ffstream->SetPos(DataStart);
}
//---------------------------------------------------------------------------
tTVPFFMPEGWaveDecoder::~tTVPFFMPEGWaveDecoder()
{
	delete Stream;
	delete p_ffstream;
}
//---------------------------------------------------------------------------
void tTVPFFMPEGWaveDecoder::GetFormat(tTVPWaveFormat & format)
{
	format = Format;
}
//---------------------------------------------------------------------------
bool tTVPFFMPEGWaveDecoder::Render(void *buf, tjs_uint bufsamplelen, tjs_uint& rendered)
{
	tjs_uint64 remain = Format.TotalSamples - CurrentPos;
	tjs_uint writesamples = bufsamplelen < remain ? bufsamplelen : (tjs_uint)remain;
	if(writesamples == 0)
	{
		// already finished stream or bufsamplelen is zero
		rendered = 0;
		return false;
	}


	tjs_uint readsize = writesamples * SampleSize;
	// tjs_uint mem_pos = CurrentPos * SampleSize;

	// if(mem_pos + read >= mem_size)
	// {
	// 	read = mem_size - CurrentPos*SampleSize;
	// }
	// tjs_uint read = Stream->Read(buf, readsize);
	tjs_uint read = p_ffstream->Read(buf, readsize);
	// if(read>0)
	// 	memcpy(buf,mem_stream + mem_pos,read);


#if TJS_HOST_IS_BIG_ENDIAN
	// endian-ness conversion
	if(Format.BytesPerSample == 2)
	{
		tjs_uint16 *p = (tjs_uint16 *)buf;
		tjs_uint16 *plim = (tjs_uint16 *)( (tjs_uint8*)buf + read);
		while(p < plim)
		{
			*p = (*p>>8) + (*p<<8);
			p++;
		}
	}
	else if(Format.BytesPerSample == 3)
	{
		tjs_uint8 *p = (tjs_uint8 *)buf;
		tjs_uint8 *plim = (tjs_uint8 *)( (tjs_uint8*)buf + read);
		while(p < plim)
		{
			tjs_uint8 tmp = p[0];
			p[0] = p[2];
			p[2] = tmp;
			p += 3;
		}
	}
	else if(Format.BytesPerSample == 4)
	{
		tjs_uint32 *p = (tjs_uint32 *)buf;
		tjs_uint32 *plim = (tjs_uint32 *)( (tjs_uint8*)buf + read);
		while(p < plim)
		{
			*p =
				(*p &0xff000000) >> 24 +
				(*p &0x00ff0000) >> 8 +
				(*p &0x0000ff00) << 8 +
				(*p &0x000000ff) << 24;
			p ++;
		}
	}
#endif


	rendered = read / SampleSize;
	CurrentPos += rendered;

	if(read < readsize || writesamples < bufsamplelen)
		return false; // read error or end of stream

	return true;
}
//---------------------------------------------------------------------------
bool tTVPFFMPEGWaveDecoder::SetPosition(tjs_uint64 samplepos)
{
	if(Format.TotalSamples <= samplepos) return false;

	tjs_uint64 streampos = DataStart + samplepos * SampleSize;
	p_ffstream->SetPos(streampos);
	// tjs_uint64 possave = Stream->GetPosition();

	// if(streampos != Stream->Seek(streampos, TJS_BS_SEEK_SET))
	// {
	// 	// seek failed
	// 	Stream->Seek(possave, TJS_BS_SEEK_SET);
	// 	return false;
	// }

	CurrentPos = samplepos;
	return true;
}

int tTVPFFMPEGWaveDecoder::read_packet_cb(void* opaque, uint8_t* buf, int buf_size)
{
	tTVPFFMPEGWaveDecoder* _this = (tTVPFFMPEGWaveDecoder*) opaque;
	tTJSBinaryStream* p_stream = _this->Stream;
	buf_size = p_stream->Read(buf, buf_size);
	// printf("read cb:%d\n",buf_size);
	if(!buf_size)
		return AVERROR_EOF;
	return buf_size;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// default Wave Decoder creator
//---------------------------------------------------------------------------
class tTVPFFMPEGWaveDecoder_C : public tTVPWaveDecoderCreator
{
public:
	tTVPWaveDecoder * Create(const ttstr & storagename, const ttstr & extension);
} TVPTSSWaveDecoderCreator;

//---------------------------------------------------------------------------
tTVPWaveDecoder * tTVPFFMPEGWaveDecoder_C::Create(const ttstr & storagename,
	const ttstr &extension)
{
	// if(extension == TJS_W(".wav")) return NULL;

	tTVPStreamHolder stream(storagename);

	// return NULL;
	ffStream* p_ffstream;
	try
	{

		// read format
		tTVPWaveFormat format;
		tjs_int64 datastart = 0;
		// p_ffstream = new ffStream(true); //decode all
		p_ffstream = new ffStream();

		// create tTVPWD_RIFFWave instance
		format.Seekable = true;
		tTVPFFMPEGWaveDecoder * decoder = new tTVPFFMPEGWaveDecoder(p_ffstream,stream.Get(), datastart,
			format);
		stream.Disown();

		return decoder;
	}
	catch(...)
	{
		delete p_ffstream;
		// this function must be a silent one unless file open error...
		return NULL;
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Log Table for DirectSound volume
//---------------------------------------------------------------------------
static bool TVPLogTableInit = false;
static tjs_int TVPLogTable[101];
static void TVPInitLogTable()
{
	if(TVPLogTableInit) return;
	TVPLogTableInit = true;
	tjs_int i;
	TVPLogTable[0] = -10000;
	for(i = 1; i <= 100; i++)
	{
		TVPLogTable[i] = static_cast<tjs_int>( log10((double)i/100.0)*TVPVolumeLogFactor );
	}
}
//---------------------------------------------------------------------------
tjs_int TVPVolumeToDSAttenuate(tjs_int volume)
{
	TVPInitLogTable();
	volume = volume / 1000;
	if(volume > 100) volume = 100;
	if(volume < 0 ) volume = 0;
	return TVPLogTable[volume];
}
//---------------------------------------------------------------------------
tjs_int TVPDSAttenuateToVolume(tjs_int att)
{
	if(att <= -10000) return 0;
	return (tjs_int)(pow(10, (double)att / TVPVolumeLogFactor) * 100.0) * 1000;
}
//---------------------------------------------------------------------------
tjs_int TVPPanToDSAttenuate(tjs_int volume)
{
	TVPInitLogTable();
	volume = volume / 1000;
	if(volume > 100) volume = 100;
	if(volume < -100 ) volume = -100;
	if(volume < 0)
		return TVPLogTable[100 - (-volume)];
	else
		return -TVPLogTable[100 - volume];
}
//---------------------------------------------------------------------------
tjs_int TVPDSAttenuateToPan(tjs_int att)
{
	if(att <= -10000) return -100000;
	if(att >=  10000) return  100000;
	if(att < 0)
		return (100 - (tjs_int)(pow(10, (double)att /  TVPVolumeLogFactor) * 100.0)) * -1000;
	else
		return (100 - (tjs_int)(pow(10, (double)att / -TVPVolumeLogFactor) * 100.0)) *  1000;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// SDL Voice management
//---------------------------------------------------------------------------
// static LPDIRECTSOUND TVPDirectSound = NULL;
static std::shared_ptr<ALAuidoDevice> TVPAudioDevice = nullptr;
// static pcmBuffer* TVPPrimaryBuffer = NULL;
// static std::shared_ptr<pcmBuffer> TVPPrimaryBuffer = nullptr;
// static LPDIRECTSOUND3DLISTENER TVPDirectSound3DListener = NULL;
static bool TVPPrimaryBufferPlayingByProgram = false;
// static HMODULE TVPDirectSoundDLL = NULL;
static TVPTimer *TVPPrimaryDelayedStopperTimer = NULL;
static bool TVPDirectSoundShutdown = false;
static bool TVPDeferedSettingAvailable = false;
//---------------------------------------------------------------------------
static void TVPEnsurePrimaryBufferPlay()
{
	if(!TVPControlPrimaryBufferRun) return;

	if(TVPAudioDevice)
	{
		if(TVPPrimaryDelayedStopperTimer)
			TVPPrimaryDelayedStopperTimer->SetEnabled( false );
		if(!TVPPrimaryBufferPlayingByProgram)
		{
			TVPAudioDevice->play(); 
			TVPPrimaryBufferPlayingByProgram = true;
		}
	}
}
//---------------------------------------------------------------------------
static void TVPStopPrimaryBuffer()
{
	// this will not immediately stop the buffer
	if(!TVPControlPrimaryBufferRun) return;

	if(TVPAudioDevice)
	{
		if(TVPPrimaryDelayedStopperTimer)
		{
			TVPPrimaryDelayedStopperTimer->SetEnabled( false ); // once disable the timer
			TVPPrimaryDelayedStopperTimer->SetEnabled( true );
		}
	}
}
//---------------------------------------------------------------------------
class tTVPPrimaryDelayedStopper
{
public:
	void OnTimer()
	{
		if(TVPPrimaryDelayedStopperTimer)
			TVPPrimaryDelayedStopperTimer->SetEnabled( false );
		if(TVPAudioDevice)
		{
			if(TVPPrimaryBufferPlayingByProgram)
			{
				TVPAudioDevice->stop();
				TVPPrimaryBufferPlayingByProgram = false;
			}
		}
	}
} TVPPrimaryDelayedStopper;
//---------------------------------------------------------------------------
// struct AudioFormat
// {
//   SDL_AudioFormat format; //pcm is AUDIO_S16SYS
//   uint16_t  nChannels;
//   uint16_t wBitsPerSample; //8 or 16
//   uint16_t wFormatTag;
//   uint32_t nSamplesPerSec; //freq

// };

static ttstr TVPGetSoundBufferFormatString(const AudioFormat &wfx)
{
	ttstr debuglog(TJS_W("format container = "));

	if(wfx.wFormatTag == WAVE_FORMAT_PCM)
		debuglog += TJS_W("WAVE_FORMAT_PCM, ");
	else if(wfx.wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
		debuglog += TJS_W("WAVE_FORMAT_IEEE_FLOAT, ");
	else if(wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		debuglog += TJS_W("WAVE_FORMAT_EXTENSIBLE, ");
	else
		debuglog += TJS_W("unknown format tag 0x") +
			TJSInt32ToHex(wfx.wFormatTag) + TJS_W(", ");

	debuglog +=
		TJS_W("frequency = ") + ttstr((tjs_int)wfx.nSamplesPerSec) + TJS_W("Hz, ") +
		TJS_W("bits = ") + ttstr((tjs_int)wfx.wBitsPerSample) + TJS_W("bits, ") +
		TJS_W("channels = ") + ttstr((tjs_int)wfx.nChannels);

	return debuglog;
}
//---------------------------------------------------------------------------
// static BOOL CALLBACK DSoundEnumCallback( GUID* pGUID, const wchar_t * strDesc,
// 	const wchar_t * strDrvName,  VOID* pContext ){}

// extern tDevPtr create_sdl_auido_dev();
extern tPtrAuDev get_al_dev();
//---------------------------------------------------------------------------
static void TVPInitDirectSound()
{
	TVPInitSoundOptions();

	if(TVPAudioDevice) return;
	if(TVPDirectSoundShutdown) return;



	try
	{
		if(TVPControlPrimaryBufferRun)
		{
			if(!TVPPrimaryDelayedStopperTimer)
			{
				// create timer to stop primary buffer playing at delayed timing
				TVPPrimaryDelayedStopperTimer = new TVPTimer();
				TVPPrimaryDelayedStopperTimer->SetInterval( 4000 );
				TVPPrimaryDelayedStopperTimer->SetEnabled( false );
				TVPPrimaryDelayedStopperTimer->SetOnTimerHandler( &TVPPrimaryDelayedStopper, &tTVPPrimaryDelayedStopper::OnTimer );
			}
		}



		// create DirectSound Object
		int hr;
		// hr = procDirectSoundCreate(NULL, &TVPDirectSound, NULL);
		TVPAudioDevice = get_al_dev();

		if(TVPAudioDevice == nullptr)
		{
			TVPThrowExceptionMessage(TVPCannotInitDirectSound,
				ttstr(TJS_W("SDL Audio Create failed./HR=")));
		}

		// create primary buffer
		TVPPrimaryBufferPlayingByProgram = false;



		if(TVPUseSoftwareBuffer)
			;
			// dsbd.dwFlags |= DSBCAPS_LOCSOFTWARE;
		if(TVPDirectSoundUse3D)
			;// dsbd.dwFlags |= DSBCAPS_CTRL3D;

		// hr = TVPDirectSound->CreateSoundBuffer(&dsbd, &TVPPrimaryBuffer, NULL);
		// TVPDirectSound->create_pcm_buffer(&TVPPrimaryBuffer);

		bool pri_normal = false;
		// if((FAILED(hr) || TVPPrimaryBuffer == NULL) && TVPDirectSoundUse3D)
		// {
		// 	// cannot create DirectSound primary buffer.
		// 	// try to not set 3D mode
		// 	TVPAddLog(TJS_W("Warning: Cannot create DirectSound primary buffer with 3D sound mode. ")
		// 		TJS_W("Force not to use it."));
		// 	dsbd.dwFlags &= ~DSBCAPS_CTRL3D;
		// 	hr = TVPDirectSound->CreateSoundBuffer(&dsbd, &TVPPrimaryBuffer, NULL);
		// 	if(SUCCEEDED(hr)) TVPDirectSoundUse3D = false;
		// }

		// if(FAILED(hr) || TVPPrimaryBuffer == NULL)
		// {
		// 	// cannot create DirectSound primary buffer.
		// 	// try to set cooperative level to DSSCL_NORMAL.
		// 	TVPPrimaryBuffer = NULL;
		// 	hr = TVPDirectSound->SetCooperativeLevel(Application->GetHandle(),
		// 		DSSCL_NORMAL);
		// 	if(FAILED(hr))
		// 	{
		// 		// failed... here assumes this a failure.
		// 		TVPThrowExceptionMessage(TVPCannotInitDirectSound,
		// 			ttstr(TJS_W("IDirectSound::SetCooperativeLevel failed. ")
		// 				TJS_W("(after creation of primary buffer failed)/HR="))+
		// 			TJSInt32ToHex((tjs_uint32)hr));
		// 	}
		// 	TVPAddLog(TJS_W("Warning: Cannot create DirectSound primary buffer. ")
		// 		TJS_W("Force not to use it."));
		// 	pri_normal = true;
		// }

		// if(TVPDirectSoundUse3D)
		// {
		// 	if(!TVPPrimaryBuffer)
		// 		TVPThrowExceptionMessage(TVPCannotInitDirectSound,
		// 			TJS_W("Cannot use 3D sounds because the primary buffer is not available."));

		// 	hr = TVPPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener,
  //                                         (void **)&TVPDirectSound3DListener);
		// 	if(FAILED(hr))
		// 	{
		// 		TVPThrowExceptionMessage(TVPCannotInitDirectSound,
		// 			ttstr(TJS_W("Querying interface IDirectSound3DListener failed./HR=") +
		// 				TJSInt32ToHex((tjs_uint32)hr)));
		// 	}
		// }

		// set primary buffer 's sound format
		if(!pri_normal && TVPAudioDevice)
		{
			AudioFormat wfx;

			// number of channels is decided using GetSpeakerConfig
			// DWORD sp_config;
			// wfx.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
			// wfx.Format.nChannels = 2;
			wfx.nChannels = 2;


			// if(SUCCEEDED(TVPDirectSound->GetSpeakerConfig(&sp_config)))
			// {
			// 	switch(DSSPEAKER_CONFIG(sp_config))
			// 	{
			// 	case DSSPEAKER_HEADPHONE:
			// 		wfx.Format.nChannels = 2;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
			// 		break;
			// 	case DSSPEAKER_MONO:
			// 		wfx.Format.nChannels = 1;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_MONO;
			// 		break;
			// 	case DSSPEAKER_QUAD:
			// 		wfx.Format.nChannels = 4;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
			// 		break;
			// 	case DSSPEAKER_STEREO:
			// 		wfx.Format.nChannels = 2;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
			// 		break;
			// 	case DSSPEAKER_SURROUND:
			// 		wfx.Format.nChannels = 4;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_SURROUND;
			// 		break;
			// 	case DSSPEAKER_5POINT1:
			// 		wfx.Format.nChannels = 6;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			// 		break;
			// 	case 0x00000007:  // DSSPEAKER_7POINT1
			// 		wfx.Format.nChannels = 8;
			// 		wfx.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
			// 		break;
			// 	}
			// }

			// wfx.Format.cbSize = 22;
			// wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			wfx.nSamplesPerSec = TVPPriamrySBFrequency; //44100
			wfx.wBitsPerSample = TVPPrimarySBBits; //16
			// wfx.wValidBitsPerSample = wfx.Format.wBitsPerSample;
			wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample/8 * wfx.nChannels);
			wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

			// memcpy(&wfx.SubFormat, TVPPrimaryFloat ?
			// 	TVP_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT:
			// 	TVP_GUID_KSDATAFORMAT_SUBTYPE_PCM, 16);

			// switch(TVPPrimarySBCreateTryLevel)
			// {
			// case 2: goto level2;
			// case 1: goto level1;
			// case 0: goto level0;
			// }

			// first try using WAVEFORMATEXTENSIBLE
			hr = TVPAudioDevice->SetFormat((const AudioFormat*)&wfx);


		// 	if(FAILED(hr))
		// 	{
		// level2:
		// 		// second try using WAVEFORMATEX
		// 		wfx.Format.cbSize = 0;
		// 		wfx.Format.wFormatTag =
		// 			TVPPrimaryFloat ? WAVE_FORMAT_IEEE_FLOAT :
		// 				WAVE_FORMAT_PCM;
		// 		wfx.Format.nBlockAlign = (WORD)(wfx.Format.wBitsPerSample/8 * wfx.Format.nChannels);
		// 		wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
		// 		hr = TVPPrimaryBuffer->SetFormat((const WAVEFORMATEX*)&wfx);
		// 	}

		// 	if(FAILED(hr))
		// 	{
		// level1:
		// 		// third try using 16bit pcm
		// 		wfx.Format.cbSize = 0;
		// 		wfx.Format.wBitsPerSample = 16;
		// 		wfx.Samples.wValidBitsPerSample = 16;
		// 		wfx.Format.wFormatTag = WAVE_FORMAT_PCM;
		// 		wfx.Format.nBlockAlign = (WORD)(wfx.Format.wBitsPerSample/8 * wfx.Format.nChannels);
		// 		wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
		// 		hr = TVPPrimaryBuffer->SetFormat((const WAVEFORMATEX*)&wfx);
		// 	}

		// 	if(FAILED(hr))
		// 	{
		// 		// specified parameter is denied
		// 		TVPAddImportantLog(ttstr(TJS_W("Warning: IDirectSoundBuffer::SetFormat failed. ")
		// 				TJS_W("(") + ttstr(TVPPriamrySBFrequency) + TJS_W("Hz, ") +
		// 				ttstr(TVPPrimarySBBits) + TJS_W("bits, ") +
		// 				ttstr((tjs_int)wfx.Format.nChannels) + TJS_W("channels)/HR="))+
		// 				TJSInt32ToHex((tjs_uint32)hr));
		// 		TVPAddImportantLog(TJS_W("Retrying with 44100Hz, ")
		// 				TJS_W("16bits, 2channels"));

		// level0:
		// 		// fourth very basic buffer format
		// 		WAVEFORMATEX wfx;
		// 		wfx.cbSize = 0;
		// 		wfx.wFormatTag = WAVE_FORMAT_PCM;
		// 		wfx.nChannels = 2;
		// 		wfx.nSamplesPerSec = 44100;
		// 		wfx.wBitsPerSample = 16;
		// 		wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample/8*wfx.nChannels);
		// 		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec*wfx.nBlockAlign;
		// 		TVPPrimaryBuffer->SetFormat(&wfx); // here does not check errors
		// 	}


			hr = TVPAudioDevice->GetFormat((AudioFormat*)&wfx);
	
			if(hr==0)
			{
				ttstr debuglog(TJS_W("(info) Accepted DirectSound primary buffer format : "));
				debuglog += TVPGetSoundBufferFormatString(wfx);
				TVPAddImportantLog(debuglog);
			}

		}
	}
	catch(...)
	{
		if(TVPAudioDevice)  TVPAudioDevice = nullptr;
		// if(TVPDirectSound3DListener) TVPDirectSound3DListener->Release(), TVPDirectSound3DListener = NULL;
		// if(TVPDirectSound)  TVPDirectSound = nullptr;
		// if(TVPDirectSoundDLL) FreeLibrary(TVPDirectSoundDLL), TVPDirectSoundDLL = NULL;
		throw;
	}
}
//---------------------------------------------------------------------------
static void TVPUninitDirectSound()
{
	// release all objects except for secondary buffers.
	if(TVPAudioDevice) TVPAudioDevice = nullptr;
	// if(TVPDirectSound3DListener) TVPDirectSound3DListener->Release(), TVPDirectSound3DListener = NULL;
	// if(TVPDirectSound)  TVPDirectSound = nullptr;
	// if(TVPDirectSoundDLL) FreeLibrary(TVPDirectSoundDLL), TVPDirectSoundDLL = NULL;
}
static void TVPUninitDirectSoundAtExitProc()
{
	TVPUninitDirectSound();
	if( TVPPrimaryDelayedStopperTimer ) delete TVPPrimaryDelayedStopperTimer, TVPPrimaryDelayedStopperTimer = NULL;
	TVPDirectSoundShutdown = true;
}
static tTVPAtExit TVPUninitDirectSoundAtExit
	(TVP_ATEXIT_PRI_RELEASE, TVPUninitDirectSoundAtExitProc);
//---------------------------------------------------------------------------
tPtrAuDev TVPGetDirectSound()
{
	try
	{
		TVPInitDirectSound();
	}
	catch(...)
	{
		return nullptr;
	}
	// if(TVPDirectSound) TVPDirectSound->AddRef();
	return TVPAudioDevice; // note that returned TVPDirectSound is AddRefed
}
//---------------------------------------------------------------------------
void TVPWaveSoundBufferCommitSettings()
{
	// commit all defered sound buffer settings
	if(TVPDeferedSettingAvailable)
	{
		// if(TVPDirectSound3DListener)
		// 	TVPDirectSound3DListener->CommitDeferredSettings();
		TVPDeferedSettingAvailable = false;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Format related functions
//---------------------------------------------------------------------------
static void TVPWaveFormatToWAVEFORMATEXTENSIBLE(const tTVPWaveFormat *in,
	AudioFormat *out, bool use3d)
{
	// convert tTVPWaveFormat structure to WAVEFORMATEXTENSIBLE structure.

	ZeroMemory(out, sizeof(AudioFormat));
	out->nChannels = in->Channels;
	out->nSamplesPerSec = in->SamplesPerSec;
	out->wBitsPerSample = in->BytesPerSample * 8;
	// out->Samples.wValidBitsPerSample = out->Format.wBitsPerSample;

	bool fraction_found = in->BitsPerSample != in->BytesPerSample * 8;

	// const WORD cbextsize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

	if(in->SpeakerConfig == 0)
	{
		if(in->Channels == 1)
		{
			// mono
			if(TVPExpandToQuad && !use3d)
			{
				assert(false);
				// out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
				// out->Format.cbSize = cbextsize;
				// out->dwChannelMask =
				// 	SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT |
				// 	SPEAKER_BACK_RIGHT;
				out->nChannels = 4;
				// out->Samples.wValidBitsPerSample = in->BitsPerSample;
			}
			else
			{
				if(in->BytesPerSample >= 3 || fraction_found)
				{
					// out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
					// out->Format.cbSize = cbextsize;
					// out->dwChannelMask = KSAUDIO_SPEAKER_MONO;
					// out->Samples.wValidBitsPerSample = in->BitsPerSample;
					assert(false);
				}
				else
				{
					out->wFormatTag =
						in->IsFloat ? WAVE_FORMAT_IEEE_FLOAT:WAVE_FORMAT_PCM;
					out->format = AL_FORMAT_MONO16;
					// out->Format.cbSize = 0;
				}
			}
		}
		else if(in->Channels == 2)
		{
			// stereo
			if(TVPExpandToQuad)
			{
				assert(false);
				out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
				// out->Format.cbSize = cbextsize;
				// out->dwChannelMask =
				// 	SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT |
				// 	SPEAKER_BACK_RIGHT;
				out->nChannels = 4;
				// out->Samples.wValidBitsPerSample = in->BitsPerSample;
			}
			else
			{
				if(in->BytesPerSample >= 3 || fraction_found)
				{
					assert(false);
					out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
					// out->Format.cbSize = cbextsize;
					// out->dwChannelMask = KSAUDIO_SPEAKER_STEREO;
					// out->Samples.wValidBitsPerSample = in->BitsPerSample;
				}
				else
				{
					out->wFormatTag =
						in->IsFloat ? WAVE_FORMAT_IEEE_FLOAT:WAVE_FORMAT_PCM;
					// out->Format.cbSize = 0;
					out->format = AL_FORMAT_STEREO16;
				}
			}
		}
		else if(in->Channels == 4)
		{
			assert(false);
			// assumed as FL, FR, BL, BR
			out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			// out->Format.cbSize = cbextsize;
			// out->dwChannelMask =
				// SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT |
				// SPEAKER_BACK_RIGHT;
			// out->Samples.wValidBitsPerSample = in->BitsPerSample;
		}
		else if(in->Channels == 6)
		{
			assert(false);
			// assumed as 5 point 1
			out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
			// out->Format.cbSize = cbextsize;
			// out->dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			// out->Samples.wValidBitsPerSample = in->BitsPerSample;
		}
		else
		{
			throw ttstr(TJS_W("Unsupported channel count : "))+
				ttstr((tjs_int)(in->Channels));
		}
	}
	else
	{
		out->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		// out->Format.cbSize = cbextsize;
		// out->dwChannelMask = in->SpeakerConfig;
		// out->Samples.wValidBitsPerSample = in->BitsPerSample;
	}

	// memcpy(&out->SubFormat, in->IsFloat ?
	// 	TVP_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT:TVP_GUID_KSDATAFORMAT_SUBTYPE_PCM, 16);

	out->nBlockAlign =
		out->wBitsPerSample * out->nChannels / 8;
	out->nAvgBytesPerSec =
		out->nSamplesPerSec * out->nBlockAlign;
	SDL_Log("Convert format:tag:%d,%d",out->wFormatTag,out->format);
}
//---------------------------------------------------------------------------
static void TVPWaveFormatToWAVEFORMATEXTENSIBLE2(const tTVPWaveFormat *in,
	AudioFormat *out, bool use3d)
{
	// suggest 2nd expression of WAVEFORMATEXTENSIBLE.
	TVPWaveFormatToWAVEFORMATEXTENSIBLE(in, out, use3d);

	bool fraction_found = in->BitsPerSample != in->BytesPerSample * 8;

	if(in->SpeakerConfig == 0)
	{
		if(in->Channels == 1)
		{
			// mono
			out->nChannels = 1;
			if(in->BytesPerSample >= 3 && !fraction_found)
			{
				out->wFormatTag =
					in->IsFloat ? WAVE_FORMAT_IEEE_FLOAT:WAVE_FORMAT_PCM;
				// out->Format.cbSize = 0;
			}
		}
		else if(in->Channels == 2)
		{
			// stereo
			out->nChannels = 2;
			if(in->BytesPerSample >= 3 && !fraction_found)
			{
				out->wFormatTag =
					in->IsFloat ? WAVE_FORMAT_IEEE_FLOAT:WAVE_FORMAT_PCM;
				// out->Format.cbSize = 0;
			}
		}
	}

	out->nBlockAlign =
		out->wBitsPerSample * out->nChannels / 8;
	out->nAvgBytesPerSec =
		out->nSamplesPerSec * out->nBlockAlign;
}
//---------------------------------------------------------------------------
static void TVPWaveFormatToWAVEFORMATEXTENSIBLE16bits(const tTVPWaveFormat *in,
	AudioFormat *out, bool use3d)
{
	// suggest 16bit output format.
	TVPWaveFormatToWAVEFORMATEXTENSIBLE(in, out, use3d);

	out->wBitsPerSample = 16;
	// out->Samples.wValidBitsPerSample = 16;

	if(out->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		assert(false);//Not Implent.
		// memcpy(&out->SubFormat, TVP_GUID_KSDATAFORMAT_SUBTYPE_PCM, 16);
	}
	else
	{
		out->wFormatTag = WAVE_FORMAT_PCM;
	}

	out->nBlockAlign =
		out->wBitsPerSample * out->nChannels / 8;
	out->nAvgBytesPerSec =
		out->nSamplesPerSec * out->nBlockAlign;
}
//---------------------------------------------------------------------------
static void TVPWaveFormatToWAVEFORMATEXTENSIBLE16bitsMono(const tTVPWaveFormat *in,
	AudioFormat *out, bool use3d)
{
	// suggest 16bit output format.
	TVPWaveFormatToWAVEFORMATEXTENSIBLE(in, out, use3d);

	out->wBitsPerSample = 16;
	// out->Samples.wValidBitsPerSample = 16;
	out->nChannels = 1;
	// memcpy(&out->SubFormat, TVP_GUID_KSDATAFORMAT_SUBTYPE_PCM, 16);
	out->wFormatTag = WAVE_FORMAT_PCM;
	// out->Format.cbSize = 0;

	out->nBlockAlign =
		out->wBitsPerSample * out->nChannels / 8;
	out->nAvgBytesPerSec =
		out->nSamplesPerSec * out->nBlockAlign;
}
//---------------------------------------------------------------------------
#pragma pack(push, 1)
static void TVPConvertWaveFormatToDestinationFormat(void *dest, const void *src,
	tjs_int count, const AudioFormat *df, const  tTVPWaveFormat*sf)
{
	// convert PCM format descripted in "sf" to the destination format "df"

	// check whether to convert to 16bit integer
	bool convert_to_16 = false;
	if(df->wBitsPerSample == 16)
	{
		if(df->wFormatTag == WAVE_FORMAT_PCM)
		{
			convert_to_16 = true;
		}
		else if(df->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		{
			// if(!memcmp(&df->SubFormat, TVP_GUID_KSDATAFORMAT_SUBTYPE_PCM, 16))
			// 	convert_to_16 = true;
			assert(false);//not implent.
		}
	}

	if(convert_to_16)
	{
		// convert to 16bit sample
		if(df->nChannels == 1 && sf->Channels != 1)
			TVPConvertPCMTo16bits((tjs_int16*)dest, src, *sf, count, true);
		else
			TVPConvertPCMTo16bits((tjs_int16*)dest, src, *sf, count, false);
	}
	else
	{
		// intact copy
		memcpy(dest, src, count * sf->BytesPerSample * sf->Channels);
	}


	struct type_24bit { tjs_uint8 data[3]; };

	#define PROCESS_BY_SAMPLESIZE \
		if(df->wBitsPerSample == 8) \
		{ \
			typedef tjs_int8 type; \
			PROCESS; \
		} \
		else if(df->wBitsPerSample == 16) \
		{ \
			typedef tjs_int16 type; \
			PROCESS;\
		} \
		else if(df->wBitsPerSample == 24) \
		{ \
			typedef type_24bit type; \
			PROCESS; \
		} \
		else if(df->wBitsPerSample == 32) \
		{ \
			typedef tjs_int32 type; \
			PROCESS; \
		} \

	if(df->nChannels == 4 && sf->Channels == 1)
	{
		// expand mono to quadphone
		#define PROCESS \
			type *ps = (type *)dest + (count-1); \
			type *pd = (type *)dest + (count-1) * 4; \
			for(tjs_int i = 0; i < count; i++) \
			{ \
				pd[0] = pd[1] = pd[2] = pd[3] = ps[0]; \
				pd -= 4; \
				ps -= 1; \
			}

		PROCESS_BY_SAMPLESIZE

		#undef PROCESS
	}

	if(df->nChannels == 4 && sf->Channels == 2)
	{
		// expand stereo to quadphone
		#define PROCESS \
			type *ps = (type *)dest + (count-1) * 2; \
			type *pd = (type *)dest + (count-1) * 4; \
			for(tjs_int i = 0; i < count; i++) \
			{ \
				pd[0] = pd[2] = ps[0]; \
				pd[1] = pd[3] = ps[1]; \
				pd -= 4; \
				ps -= 2; \
			}

		PROCESS_BY_SAMPLESIZE

		#undef PROCESS
	}

	if(sf->SpeakerConfig == 0 && sf->Channels == 6 && df->nChannels == 6)
	{
		// if the "channels" is 6 and speaker configuration is not specified,
		// input data is assumued that the order is :
		// FL FC FR BL BR LF
		// is to be reordered as :
		// FL FR FC LF BL BR (which WAVEFORMATEXTENSIBLE expects)

		#define PROCESS \
			type *op = (type *)dest; \
			for(tjs_int i = 0; i < count; i++) \
			{ \
				type fc = op[1]; \
				type bl = op[3]; \
				type br = op[4]; \
				op[1] = op[2]; \
				op[2] = fc; \
				op[3] = op[5]; \
				op[4] = bl; \
				op[5] = br; \
				op += 6; \
			}

		PROCESS_BY_SAMPLESIZE


		#undef PROCESS
	}
}
#pragma pack(pop)
//---------------------------------------------------------------------------
static void TVPMakeSilentWaveBytes(void *dest, tjs_int bytes, const AudioFormat *format)
{
	if(format->wBitsPerSample == 8)
	{
		// 0x80
		memset(dest, 0x80, bytes);
	}
	else
	{
		// 0x00
		memset(dest, 0x00, bytes);
	}
}
//---------------------------------------------------------------------------
static void TVPMakeSilentWave(void *dest, tjs_int count, const AudioFormat *format)
{
// wFormatTag is WAVE_FORMAT_PCM, nBlockAlign must equal (nChannels × wBitsPerSample) / 8.
	tjs_int bytes = count * format->nBlockAlign;
	TVPMakeSilentWaveBytes(dest, bytes, format);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Buffer management
//---------------------------------------------------------------------------
std::vector<tTJSNI_WaveSoundBuffer *> TVPWaveSoundBufferVector;
tTJSCriticalSection TVPWaveSoundBufferVectorCS;

//---------------------------------------------------------------------------
// tTVPWaveSoundBufferThread : playing thread
//---------------------------------------------------------------------------
/*
	The system has one playing thread.
	The playing thread fills each sound buffer's L1 (DirectSound) buffer, and
	also manages timing for label events.
	The technique used in this algorithm is similar to Timer claass implementation.
*/
class tTVPWaveSoundBufferThread : public tTVPThread
{
	tTVPThreadEvent Event;

	//HWND UtilWindow; // utility window to notify the pending events occur
	bool PendingLabelEventExists;
	bool WndProcToBeCalled;
	DWORD NextLabelEventTick;
	DWORD LastFilledTick;

	NativeEventQueue<tTVPWaveSoundBufferThread> EventQueue;
public:
	tTVPWaveSoundBufferThread();
	~tTVPWaveSoundBufferThread();

private:
	//void __fastcall UtilWndProc(Messages::TMessage &Msg);
	void UtilWndProc( NativeEvent& ev );

public:
	void ReschedulePendingLabelEvent(tjs_int tick);

protected:
	void Execute(void);

public:
	void Start(void);
	void CheckBufferSleep();


} static *TVPWaveSoundBufferThread = NULL;
//---------------------------------------------------------------------------
tTVPWaveSoundBufferThread::tTVPWaveSoundBufferThread()
	: tTVPThread(true), EventQueue(this,&tTVPWaveSoundBufferThread::UtilWndProc)
{
	EventQueue.Allocate();
	PendingLabelEventExists = false;
	NextLabelEventTick = 0;
	LastFilledTick = 0;
	WndProcToBeCalled = false;
	SetPriority(ttpHighest);
	Resume();
}
//---------------------------------------------------------------------------
tTVPWaveSoundBufferThread::~tTVPWaveSoundBufferThread()
{
	SetPriority(ttpNormal);
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
	EventQueue.Deallocate();
}
//---------------------------------------------------------------------------
//void __fastcall tTVPWaveSoundBufferThread::UtilWndProc(Messages::TMessage &Msg)
//process in main thread??
void tTVPWaveSoundBufferThread::UtilWndProc( NativeEvent& ev )
{
	// Window procedure of UtilWindow
	if( ev.data1 && !GetTerminated())
	{
		// pending events occur
		tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS); // protect the object

		WndProcToBeCalled = false;

		tjs_int64 tick = TVPGetTickCount();

		int nearest_next = TVP_TIMEOFS_INVALID_VALUE;

		std::vector<tTJSNI_WaveSoundBuffer *>::iterator i;
		for(i = TVPWaveSoundBufferVector.begin();
			i != TVPWaveSoundBufferVector.end(); i++)
		{
			int next = (*i)->FireLabelEventsAndGetNearestLabelEventStep(tick);
				// fire label events and get nearest label event step
			if(next != TVP_TIMEOFS_INVALID_VALUE)
			{
				if(nearest_next == TVP_TIMEOFS_INVALID_VALUE || nearest_next > next)
					nearest_next = next;
			}
		}

		if(nearest_next != TVP_TIMEOFS_INVALID_VALUE)
		{
			PendingLabelEventExists = true;
			NextLabelEventTick = timeGetTime() + nearest_next;
		}
		else
		{
			PendingLabelEventExists = false;
		}
	}
	else
	{
		EventQueue.HandlerDefault(ev);
	}
}
//---------------------------------------------------------------------------
void tTVPWaveSoundBufferThread::ReschedulePendingLabelEvent(tjs_int tick)
{
	if(tick == TVP_TIMEOFS_INVALID_VALUE) return; // no need to reschedule
	DWORD eventtick = timeGetTime() + tick;

	tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);

	if(PendingLabelEventExists)
	{
		if((tjs_int32)NextLabelEventTick - (tjs_int32)eventtick > 0)
			NextLabelEventTick = eventtick;
	}
	else
	{
		PendingLabelEventExists = true;
		NextLabelEventTick = eventtick;
	}
}
//---------------------------------------------------------------------------
#define TVP_WSB_THREAD_SLEEP_TIME 60
void tTVPWaveSoundBufferThread::Execute(void)
{
	while(!GetTerminated())
	{
		// thread loop for playing thread
		DWORD time = timeGetTime();
		TVPPushEnvironNoise(&time, sizeof(time));

		{	// thread-protected
			tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);

			// check PendingLabelEventExists
			if(PendingLabelEventExists)
			{
				if(!WndProcToBeCalled)
				{
					WndProcToBeCalled = true;
					EventQueue.PostEvent( NativeEvent() );
				}
			}

			if(time - LastFilledTick >= TVP_WSB_THREAD_SLEEP_TIME)
			{
				std::vector<tTJSNI_WaveSoundBuffer *>::iterator i;
				for(i = TVPWaveSoundBufferVector.begin();
					i != TVPWaveSoundBufferVector.end(); i++)
				{
					if((*i)->ThreadCallbackEnabled)
					{
						(*i)->FillBuffer(); // fill sound buffer
					}
				}
				LastFilledTick = time;
			}
		}	// end-of-thread-protected

		DWORD time2;
		time2 = timeGetTime();
		time = time2 - time;

		if(time < TVP_WSB_THREAD_SLEEP_TIME)
		{
			tjs_int sleep_time = TVP_WSB_THREAD_SLEEP_TIME - time;
			if(PendingLabelEventExists)
			{
				tjs_int step_to_next = (tjs_int32)NextLabelEventTick - (tjs_int32)time2;
				if(step_to_next < sleep_time)
					sleep_time = step_to_next;
				if(sleep_time < 1) sleep_time = 1;
			}
			Event.WaitFor(sleep_time);
		}
		else
		{
			Event.WaitFor(1);
		}
	}
}
//---------------------------------------------------------------------------
void tTVPWaveSoundBufferThread::Start()
{
	Event.Set();
	Resume();
}
//---------------------------------------------------------------------------
void tTVPWaveSoundBufferThread::CheckBufferSleep()
{
	tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);
	tjs_uint size, nonwork_count;
	nonwork_count = 0;
	size = (tjs_uint)TVPWaveSoundBufferVector.size();
	std::vector<tTJSNI_WaveSoundBuffer *>::iterator i;
	for(i = TVPWaveSoundBufferVector.begin();
		i != TVPWaveSoundBufferVector.end(); i++)
	{
		if(!(*i)->ThreadCallbackEnabled)
			nonwork_count ++;
	}
	if(nonwork_count == size)
	{
		Suspend(); // all buffers are sleeping...
		TVPStopPrimaryBuffer();
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void TVPReleaseSoundBuffers(bool disableevent = true)
{
	// release all secondary buffers.
	tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);
	std::vector<tTJSNI_WaveSoundBuffer *>::iterator i;
	for(i = TVPWaveSoundBufferVector.begin();
		i != TVPWaveSoundBufferVector.end(); i++)
	{
		(*i)->FreeDirectSoundBuffer(disableevent);
	}
}
//---------------------------------------------------------------------------
static void TVPShutdownWaveSoundBuffers()
{
	// clean up soundbuffers at exit
	if(TVPWaveSoundBufferThread)
		delete TVPWaveSoundBufferThread, TVPWaveSoundBufferThread = NULL;
	TVPReleaseSoundBuffers();
}
static tTVPAtExit TVPShutdownWaveSoundBuffersAtExit
	(TVP_ATEXIT_PRI_PREPARE, TVPShutdownWaveSoundBuffers);
//---------------------------------------------------------------------------
static void TVPEnsureWaveSoundBufferWorking()
{
	if(!TVPWaveSoundBufferThread)
		TVPWaveSoundBufferThread = new tTVPWaveSoundBufferThread();
	TVPWaveSoundBufferThread->Start();
}
//---------------------------------------------------------------------------
static void TVPCheckSoundBufferAllSleep()
{
	if(TVPWaveSoundBufferThread)
		TVPWaveSoundBufferThread->CheckBufferSleep();
}
//---------------------------------------------------------------------------
static void TVPAddWaveSoundBuffer(tTJSNI_WaveSoundBuffer * buffer)
{
	tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);
	TVPWaveSoundBufferVector.push_back(buffer);
}
//---------------------------------------------------------------------------
static void TVPRemoveWaveSoundBuffer(tTJSNI_WaveSoundBuffer * buffer)
{
	bool bufferempty;

	{
		tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);
		std::vector<tTJSNI_WaveSoundBuffer *>::iterator i;
		i = std::find(TVPWaveSoundBufferVector.begin(),
			TVPWaveSoundBufferVector.end(),
			buffer);
		if(i != TVPWaveSoundBufferVector.end())
			TVPWaveSoundBufferVector.erase(i);
		bufferempty = TVPWaveSoundBufferVector.size() == 0;
	}

	if(bufferempty)
	{
		if(TVPWaveSoundBufferThread)
			delete TVPWaveSoundBufferThread, TVPWaveSoundBufferThread = NULL;
	}
}
//---------------------------------------------------------------------------
static void TVPReschedulePendingLabelEvent(tjs_int tick)
{
	if(TVPWaveSoundBufferThread)
		TVPWaveSoundBufferThread->ReschedulePendingLabelEvent(tick);
}
//---------------------------------------------------------------------------
void TVPResetVolumeToAllSoundBuffer()
{
	// call each SoundBuffer's SetVolumeToSoundBuffer
	tTJSCriticalSectionHolder holder(TVPWaveSoundBufferVectorCS);
	std::vector<tTJSNI_WaveSoundBuffer *>::iterator i;
	for(i = TVPWaveSoundBufferVector.begin();
		i != TVPWaveSoundBufferVector.end(); i++)
	{
		(*i)->SetVolumeToSoundBuffer();
	}
}
//---------------------------------------------------------------------------
void TVPReleaseDirectSound()
{
	TVPReleaseSoundBuffers(false);
	TVPUninitDirectSound();
}
//---------------------------------------------------------------------------
void TVPSetWaveSoundBufferUse3DMode(bool b)
{
	// changing the 3D mode will stop all the buffers.
	if(b != TVPDirectSoundUse3D)
	{
		TVPReleaseDirectSound();
		TVPDirectSoundUse3D = b;
	}
}
//---------------------------------------------------------------------------
bool TVPGetWaveSoundBufferUse3DMode()
{
	return TVPDirectSoundUse3D;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPWaveSoundBufferDcodeThread : decoding thread
//---------------------------------------------------------------------------
class tTVPWaveSoundBufferDecodeThread : public tTVPThread
{
	tTJSNI_WaveSoundBuffer * Owner;
	tTVPThreadEvent Event;
	tTJSCriticalSection OneLoopCS;
	volatile bool Running;

public:
	tTVPWaveSoundBufferDecodeThread(tTJSNI_WaveSoundBuffer * owner);
	~tTVPWaveSoundBufferDecodeThread();

	void Execute(void);

	void Interrupt();
	void Continue();

	bool GetRunning() const { return Running; }
};
//---------------------------------------------------------------------------
tTVPWaveSoundBufferDecodeThread::tTVPWaveSoundBufferDecodeThread(
	tTJSNI_WaveSoundBuffer * owner)
	: tTVPThread(true)
{
	TVPInitSoundOptions();

	Owner = owner;
	SetPriority(TVPDecodeThreadHighPriority);
	Running = false;
	Resume();
}
//---------------------------------------------------------------------------
tTVPWaveSoundBufferDecodeThread::~tTVPWaveSoundBufferDecodeThread()
{
	SetPriority(TVPDecodeThreadHighPriority);
	Running = false;
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
}
//---------------------------------------------------------------------------
#define TVP_WSB_DECODE_THREAD_SLEEP_TIME 110
void tTVPWaveSoundBufferDecodeThread::Execute(void)
{
	SDL_Log("Decode Thread running!");
	while(!GetTerminated())
	{
		// decoder thread main loop
		DWORD st = GetTickCount();
		while(Running)
		{
			bool wait;
			DWORD et;

			if(Running)
			{
				volatile tTJSCriticalSectionHolder cs_holder(OneLoopCS);
				wait = !Owner->FillL2Buffer(false, true); // fill
			}

			if(GetTerminated()) break;

			if(Running)
			{
				et = GetTickCount();
				TVPPushEnvironNoise(&et, sizeof(et));
				if(wait)
				{
					// buffer is full; sleep longer

					DWORD elapsed = et -st;

					// SDL_Log("buffer is full sleep!%d,%d",
					// elapsed,TVP_WSB_DECODE_THREAD_SLEEP_TIME - elapsed);
					if(elapsed < TVP_WSB_DECODE_THREAD_SLEEP_TIME)
					{
						Event.WaitFor(
							TVP_WSB_DECODE_THREAD_SLEEP_TIME - elapsed);
					}
				}
				else
				{
					// buffer is not full; sleep shorter
					Sleep(1);
					if(!GetTerminated()) SetPriority(TVPDecodeThreadLowPriority);
				}
				st = et;
			}
		}
		if(GetTerminated()) break;
		// sleep while running
		Event.WaitFor(INFINITE);
	}
}
//---------------------------------------------------------------------------
void tTVPWaveSoundBufferDecodeThread::Interrupt()
{
	// interrupt the thread
	if(!Running) return;
	SetPriority(TVPDecodeThreadHighPriority);
	Event.Set();
	tTJSCriticalSectionHolder cs_holder(OneLoopCS);
		// this ensures that this function stops the decoding
	Running = false;
}
//---------------------------------------------------------------------------
void tTVPWaveSoundBufferDecodeThread::Continue()
{
	SetPriority(TVPDecodeThreadHighPriority);
	Running = true;
	Event.Set();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTJSNI_WaveSoundBuffer
//---------------------------------------------------------------------------
tjs_int tTJSNI_WaveSoundBuffer::GlobalVolume = 100000;
tTVPSoundGlobalFocusMode tTJSNI_WaveSoundBuffer::GlobalFocusMode = sgfmNeverMute;
//---------------------------------------------------------------------------
tTJSNI_WaveSoundBuffer::tTJSNI_WaveSoundBuffer()
{
	TVPInitSoundOptions();
	//TODO
	TVPRegisterTSSWaveDecoderCreator();
// #ifdef TVP_SUPPORT_OLD_WAVEUNPACKER
// 	TVPRegisterWaveUnpackerCreator();
// #endif
// #ifdef TVP_SUPPORT_KPI
// 	TVPRegisterKMPWaveDecoderCreator();
// #endif
	TVPInitLogTable();
	Decoder = NULL;
	LoopManager = NULL;
	Thread = NULL;
	UseVisBuffer = false;
	VisBuffer = NULL;
	ThreadCallbackEnabled = false;
	Level2Buffer = NULL;
	Level2BufferSize = 0;
	Volume =  100000;
	Volume2 = 100000;
	BufferCanControlPan = false;
	Pan = 0;
	PosX = PosY = PosZ = (D3DVALUE)0.0;
	SoundBuffer = NULL;
	Sound3DBuffer = NULL;
	L2BufferDecodedSamplesInUnit = NULL;
	L1BufferSegmentQueues = NULL;
	L2BufferSegmentQueues = NULL;
	L1BufferDecodeSamplePos = NULL;
	DecodePos = 0;
	L1BufferUnits = 0;
	L2BufferUnits = 0;
	TVPAddWaveSoundBuffer(this);
	Thread = new tTVPWaveSoundBufferDecodeThread(this);
	ZeroMemory(&C_InputFormat, sizeof(C_InputFormat));
	ZeroMemory(&InputFormat, sizeof(InputFormat));
	ZeroMemory(&Format, sizeof(Format));
	Looping = false;
	DSBufferPlaying = false;
	BufferPlaying = false;
	Paused = false;
	BufferBytes = 0;
	AccessUnitBytes = 0;
	AccessUnitSamples = 0;
	L2AccessUnitBytes = 0;
	SoundBufferPrevReadPos = 0;
	SoundBufferWritePos = 0;
	PlayStopPos = 0;
	L2BufferReadPos = 0;
	L2BufferWritePos = 0;
	L2BufferRemain = 0;
	L2BufferEnded = false;
	LastCheckedDecodePos = -1;
	LastCheckedTick = 0;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
tTJSNI_WaveSoundBuffer::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = inherited::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_WaveSoundBuffer::Invalidate()
{
	inherited::Invalidate();

	Clear();

	DestroySoundBuffer();

	if(Thread) delete Thread, Thread = NULL;

	TVPRemoveWaveSoundBuffer(this);
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::ThrowSoundBufferException(const ttstr &reason)
{
	TVPThrowExceptionMessage(TVPCannotCreateDSSecondaryBuffer,
		reason, ttstr().printf(TJS_W("frequency=%d/channels=%d/bits=%d"),
		InputFormat.SamplesPerSec, InputFormat.Channels,
		InputFormat.BitsPerSample));
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::TryCreateSoundBuffer(bool use3d)
{
	// release previous sound buffer
	if(SoundBuffer) SoundBuffer->Release(), SoundBuffer = nullptr;

	// compute buffer bytes
	AccessUnitSamples = Format.nSamplesPerSec / TVP_WSB_ACCESS_FREQ; // freq/8
	AccessUnitBytes = AccessUnitSamples * Format.nBlockAlign; //
	SDL_Log("===AccessUnitSamples:%d,AccessUnitBytes:%d",AccessUnitSamples,AccessUnitBytes);

//L1 buffer 1000ms len, split 8 part？
//one unit is 125ms?
//L1BufferUnits = 8
	L1BufferUnits = TVPL1BufferLength / (1000 / TVP_WSB_ACCESS_FREQ);
	if(L1BufferUnits <= 1) L1BufferUnits = 2;
	if(L1BufferSegmentQueues) delete [] L1BufferSegmentQueues, L1BufferSegmentQueues = NULL;
	L1BufferSegmentQueues = new tTVPWaveSegmentQueue[L1BufferUnits];
	LabelEventQueue.clear();
	if(L1BufferDecodeSamplePos) delete [] L1BufferDecodeSamplePos, L1BufferDecodeSamplePos = NULL;
	L1BufferDecodeSamplePos = new tjs_int64[L1BufferUnits];
	BufferBytes = AccessUnitBytes * L1BufferUnits;
		// l1 buffer bytes

	if(BufferBytes <= 0)
		ThrowSoundBufferException(TJS_W("Invalid format."));

	// allocate visualization buffer
	if(UseVisBuffer) ResetVisBuffer();

	// allocate level2 buffer ( 4sec. buffer )
	L2BufferUnits = TVPL2BufferLength / (1000 / TVP_WSB_ACCESS_FREQ);
	if(L2BufferUnits <= 1) L2BufferUnits = 2;

	if(L2BufferDecodedSamplesInUnit) delete [] L2BufferDecodedSamplesInUnit, L2BufferDecodedSamplesInUnit = NULL;
	if(L2BufferSegmentQueues) delete [] L2BufferSegmentQueues, L2BufferSegmentQueues = NULL;
	L2BufferDecodedSamplesInUnit = new tjs_int[L2BufferUnits];
	L2BufferSegmentQueues = new tTVPWaveSegmentQueue[L2BufferUnits];
//one unit bytes?
	L2AccessUnitBytes = AccessUnitSamples * InputFormat.BytesPerSample * InputFormat.Channels;
	Level2BufferSize = L2AccessUnitBytes * L2BufferUnits;
	if(Level2Buffer) delete [] Level2Buffer, Level2Buffer = NULL;
	Level2Buffer = new tjs_uint8[Level2BufferSize];

	// setup parameters
	// DSBUFFERDESC dsbd;
	// ZeroMemory(&dsbd, sizeof(dsbd));
	// dsbd.dwSize = sizeof(dsbd);
	// dsbd.dwFlags = 	DSBCAPS_GETCURRENTPOSITION2 |
	// 	DSBCAPS_CTRLVOLUME;
	// BufferCanControlFrequency = true;
	// if(!(TVPDirectSoundUse3D && use3d))
	// 	dsbd.dwFlags |= DSBCAPS_CTRLPAN, BufferCanControlPan = true;
	// else
	// 	dsbd.dwFlags |= DSBCAPS_CTRL3D, BufferCanControlPan = false;
	// dsbd.dwFlags |= DSBCAPS_CTRLFREQUENCY;
	// if(!(TVPSoundGlobalFocusMuteVolume == 0 &&
	// 	TVPSoundGlobalFocusModeByOption >= sgfmMuteOnDeactivate))
	// 	dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
	// if(TVPUseSoftwareBuffer)
	// 	dsbd.dwFlags |= DSBCAPS_LOCSOFTWARE;

	// dsbd.dwBufferBytes = BufferBytes;

	// dsbd.lpwfxFormat = (WAVEFORMATEX*)&Format;

	// create sound buffer
	int hr;
	hr = TVPAudioDevice->CreateSoundBuffer(SoundBuffer,
		L1BufferUnits,BufferBytes,Format);
	// if(FAILED(hr))
	// {
	// 	if(BufferCanControlPan)
	// 	{
	// 		dsbd.dwFlags &= ~ DSBCAPS_CTRLPAN;
	// 		BufferCanControlPan = false;
	// 		hr = TVPAudioDevice->CreateSoundBuffer(&dsbd, &SoundBuffer, NULL);
	// 	}
	// }

	// if(FAILED(hr))
	// {
	// 	if(BufferCanControlFrequency)
	// 	{
	// 		dsbd.dwFlags &= ~ DSBCAPS_CTRLFREQUENCY;
	// 		BufferCanControlFrequency = false;
	// 		hr = TVPDirectSound->CreateSoundBuffer(&dsbd, &SoundBuffer, NULL);
	// 	}
	// }

	if(hr<0) SoundBuffer = nullptr;

	// if((TVPDirectSoundUse3D && use3d) && SUCCEEDED(hr))
	// {
	// 	// retrieve DirectSound3DBuffer interface
	// 	hr = SoundBuffer->QueryInterface(IID_IDirectSound3DBuffer,
	// 		(void**)&Sound3DBuffer);
	// 	if(FAILED(hr)) Sound3DBuffer = NULL;
	// }

	// report failure
	// if(hr)
	// {
	// 	if(SoundBuffer) SoundBuffer->Release();
	// 	SoundBuffer = NULL;
	// 	delete [] Level2Buffer;
	// 	Level2Buffer = NULL;
	// 	ThrowSoundBufferException(
	// 		ttstr(TJS_W("IDirectSound::CreateSoundBuffer ")
	// 			TJS_W("(on to create a secondary buffer) failed./HR=") +
	// 			TJSInt32ToHex(hr)));
	// }
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::CreateSoundBuffer()
{
	// create a direct sound secondary buffer which has given format.

	TVPInitDirectSound(); // ensure DirectSound object

	bool format_is_not_identical = TVPAlwaysRecreateSoundBuffer ||
		C_InputFormat.SamplesPerSec		!= InputFormat.SamplesPerSec ||
		C_InputFormat.Channels			!= InputFormat.Channels ||
		C_InputFormat.BitsPerSample		!= InputFormat.BitsPerSample ||
		C_InputFormat.BytesPerSample	!= InputFormat.BytesPerSample ||
		C_InputFormat.SpeakerConfig		!= InputFormat.SpeakerConfig ||
		C_InputFormat.IsFloat			!= InputFormat.IsFloat;
	// bool format_is_not_identical = true;

	if(format_is_not_identical)
	{
		try
		{
			ttstr msg;
			bool failed;
			bool firstfailed = false;
			ttstr firstformat;
			bool use3d = (InputFormat.Channels >= 3 || InputFormat.SpeakerConfig != 0) ?
				false : TVPDirectSoundUse3D;
				// currently DirectSound3D cannot handle multiple speaker configuration
				// other than stereo.
			int forcemode = 0;

			// if(TVPForceConvertMode == fcm16bit) goto try16bits;
			// if(TVPForceConvertMode == fcm16bitMono) goto try16bits_mono;

			failed = false;
			TVPWaveFormatToWAVEFORMATEXTENSIBLE(&InputFormat, &Format, use3d);
			try
			{
				TryCreateSoundBuffer(use3d);
			}
			catch(eTJSError &e)
			{
				failed = true;
				msg = e.GetMessage();
			}

	

			if(failed)
				TVPThrowExceptionMessage(msg.c_str());


			// log
			if(SoundBuffer && firstfailed)
			{
				AudioFormat wfx;
				int hr = SoundBuffer->GetFormat(wfx); 

				ttstr log;
				if(!use3d)
					log = TJS_W("(info) Accepted DirectSound secondary buffer format : ");
				else
					log = TJS_W("(info) Accepted DirectSound3D secondary buffer format : ");
				if(hr == 0)
					log += TVPGetSoundBufferFormatString(wfx);
				else
					log += TJS_W("unknown format");
				if(firstfailed)
				{
					log += TJS_W(" (") + firstformat +
						TJS_W(" was requested but denied. Continuing operation with ");
					if(forcemode == 1)
						log += TJS_W("16bit mode");
					else if(forcemode == 2)
						log += TJS_W("16bit mono mode");
					log += TJS_W(".)");
				}

				if(firstfailed)
					TVPAddImportantLog(log);
			}
		}
		catch(ttstr & e)
		{
			ThrowSoundBufferException(e);
		}
		catch(...)
		{
			throw;
		}

	}

	// reset volume, sound position and frequency
	SetVolumeToSoundBuffer();
	Set3DPositionToBuffer();
	SetFrequencyToBuffer();

	// reset sound buffer
	ResetSoundBuffer();

	C_InputFormat = InputFormat;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::DestroySoundBuffer()
{
	// if(Sound3DBuffer)
	// {
	// 	Sound3DBuffer->Release();
	// 	Sound3DBuffer = NULL;
	// }

	if(SoundBuffer)
	{
		SoundBuffer->stop();
		SoundBuffer->Release();
		SoundBuffer = nullptr;
	}

	DSBufferPlaying = false;
	BufferPlaying = false;

	if(L1BufferSegmentQueues) delete [] L1BufferSegmentQueues, L1BufferSegmentQueues = NULL;
	LabelEventQueue.clear();
	if(L1BufferDecodeSamplePos) delete [] L1BufferDecodeSamplePos, L1BufferDecodeSamplePos = NULL;
	if(L2BufferDecodedSamplesInUnit) delete [] L2BufferDecodedSamplesInUnit, L2BufferDecodedSamplesInUnit = NULL;
	if(L2BufferSegmentQueues) delete [] L2BufferSegmentQueues, L2BufferSegmentQueues = NULL;
	if(Level2Buffer) delete [] Level2Buffer, Level2Buffer = NULL;
	L1BufferUnits = 0;
	L2BufferUnits = 0;

	ZeroMemory(&C_InputFormat, sizeof(C_InputFormat));

	Level2BufferSize = 0;

	DeallocateVisBuffer();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::ResetSoundBuffer()
{
	if(SoundBuffer)
	{
		// fill the buffer with silence

		SoundBuffer->reset();
		uint8_t *p1 = nullptr;
		int b1;
		int hr;
		// hr = SoundBuffer->Lock(0, BufferBytes, (void**)&p1,
		// 	&b1, (void**)&p2, &b2, 0);
		// SoundBuffer->lock(0,BufferBytes,(void**)&p1,&b1);


		// if(p1)
		// {
		// 	TVPMakeSilentWaveBytes(p1, BufferBytes, &Format);

		// 	SoundBuffer->unlock(p1, b1);
		// }

		// SoundBuffer->seek(0);

		// fill level2 buffer with silence
		TVPMakeSilentWaveBytes(Level2Buffer, Level2BufferSize, &Format);
	}

	ResetSamplePositions();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::ResetSamplePositions()
{
	// reset L1BufferSegmentQueues and L2BufferSegmentQueues, and labels
	if(L1BufferSegmentQueues)
	{
		for(int i = 0; i < L1BufferUnits; i++)
			L1BufferSegmentQueues[i].Clear();
	}
	if(L2BufferSegmentQueues)
	{
		for(int i = 0; i < L2BufferUnits; i++)
			L2BufferSegmentQueues[i].Clear();
	}
	if(L1BufferDecodeSamplePos)
	{
		for(int i = 0; i < L1BufferUnits; i++)
			L1BufferDecodeSamplePos[i] = -1;
	}
	LabelEventQueue.clear();
	DecodePos = 0;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::Clear()
{
	// clear all status and unload current decoder
	Stop();
	ThreadCallbackEnabled = false;
	TVPCheckSoundBufferAllSleep();
	Thread->Interrupt();
	if(LoopManager) delete LoopManager, LoopManager = NULL;
	ClearFilterChain();
	if(Decoder) delete Decoder, Decoder = NULL;
	BufferPlaying = false;
	DSBufferPlaying = false;
	Paused = false;

	ResetSamplePositions();

	SetStatus(ssUnload);
}
//---------------------------------------------------------------------------
tjs_uint tTJSNI_WaveSoundBuffer::Decode(void *buffer, tjs_uint bufsamplelen,
		tTVPWaveSegmentQueue & segments)
{
	// decode one buffer unit
	tjs_uint w = 0;

	try
	{
		// decode
		FilterOutput->Decode((tjs_uint8*)buffer, bufsamplelen, w, segments);
	}
	catch(...)
	{
		// ignore errors
		w = 0;
	}

	return w;
}
//---------------------------------------------------------------------------
bool tTJSNI_WaveSoundBuffer::FillL2Buffer(bool firstwrite, bool fromdecodethread)
{
	SDL_Log("FillL2Buffer:cur_pos:%d,remain:%d",L2BufferWritePos,L2BufferRemain);
	if(!fromdecodethread && Thread->GetRunning())
		Thread->SetPriority(ttpHighest);
			// make decoder thread priority high, before entering critical section

	tTJSCriticalSectionHolder holder(L2BufferCS);

	if(firstwrite)
	{
		// only main thread runs here
		L2BufferReadPos = L2BufferWritePos = L2BufferRemain = 0;
		L2BufferEnded = false;
		for(tjs_int i = 0; i<L2BufferUnits; i++)
			L2BufferDecodedSamplesInUnit[i] = 0;
	}

	{
		tTVPThreadPriority ttpbefore = TVPDecodeThreadHighPriority;
		bool retflag = false;
		if(Thread->GetRunning())
		{
			ttpbefore = Thread->GetPriority();
			Thread->SetPriority(TVPDecodeThreadHighPriority);
		}
		{
			tTJSCriticalSectionHolder holder(L2BufferRemainCS);
			if(L2BufferRemain == L2BufferUnits) retflag = true;
		}
		if(!retflag) UpdateFilterChain(); // if the buffer is not full, update filter internal state
		if(Thread->GetRunning()) Thread->SetPriority(ttpbefore);
		if(retflag) return false; // buffer is full
	}

	if(L2BufferEnded)
	{
		L2BufferSegmentQueues[L2BufferWritePos].Clear();
		L2BufferDecodedSamplesInUnit[L2BufferWritePos] = 0;
	}
	else
	{
		L2BufferSegmentQueues[L2BufferWritePos].Clear();
		tjs_uint decoded = Decode(
			L2BufferWritePos * L2AccessUnitBytes + Level2Buffer,
			AccessUnitSamples,
			L2BufferSegmentQueues[L2BufferWritePos]);

		if(decoded < (tjs_uint) AccessUnitSamples) L2BufferEnded = true;

		L2BufferDecodedSamplesInUnit[L2BufferWritePos] = decoded;
		SDL_Log("FillL2Buffer end:%d,is_end:%d",decoded,L2BufferEnded);
	}

	L2BufferWritePos++;
	if(L2BufferWritePos >= L2BufferUnits) L2BufferWritePos = 0;

	{
		tTVPThreadPriority ttpbefore = TVPDecodeThreadHighPriority;
		if(Thread->GetRunning())
		{
			ttpbefore = Thread->GetPriority();
			Thread->SetPriority(TVPDecodeThreadHighPriority);
		}
		{
			tTJSCriticalSectionHolder holder(L2BufferRemainCS);
			L2BufferRemain++;
		}
		if(Thread->GetRunning()) Thread->SetPriority(ttpbefore);
	}


	return true;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::PrepareToReadL2Buffer(bool firstread)
{
	if(L2BufferRemain == 0 && !L2BufferEnded)
		FillL2Buffer(firstread, false);

	if(Thread->GetRunning()) Thread->SetPriority(TVPDecodeThreadHighPriority);
			// make decoder thread priority higher than usual,
			// before entering critical section
}
//---------------------------------------------------------------------------
tjs_uint tTJSNI_WaveSoundBuffer::ReadL2Buffer(void *buffer,
		tTVPWaveSegmentQueue & segments)
{
	// This routine is protected by BufferCS, not L2BufferCS, while
	// this routine reads L2 buffer.
	// But It's ok because this function will never read currently writing L2
	// buffer. L2 buffer having at least one rendered unit is
	// guaranteed at this point.
	

	tjs_uint decoded = L2BufferDecodedSamplesInUnit[L2BufferReadPos];

	SDL_Log("ReadL2Buffer:pos:%d,decoded:%d",L2BufferReadPos,decoded);

	segments = L2BufferSegmentQueues[L2BufferReadPos];

	TVPConvertWaveFormatToDestinationFormat(buffer,
		L2BufferReadPos * L2AccessUnitBytes + Level2Buffer, decoded,
		&Format, &InputFormat);

	if(decoded < (tjs_uint)AccessUnitSamples)
	{
		// fill rest with silence
		TVPMakeSilentWave((tjs_uint8*)buffer + decoded*Format.nBlockAlign,
			AccessUnitSamples - decoded, &Format);
	}

	L2BufferReadPos++;
	if(L2BufferReadPos >= L2BufferUnits) L2BufferReadPos = 0;

	{
		tTJSCriticalSectionHolder holder(L2BufferRemainCS);
		L2BufferRemain--;
	}

	return decoded;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::FillDSBuffer(tjs_int writepos,
		tTVPWaveSegmentQueue & segments)
{
	uint8_t *p1, *p2;
	int b1, b2;

	segments.Clear();
	SDL_Log("FillDSBuffer pos:%d",writepos);

	bool hr;
	hr = SoundBuffer->lock(writepos, AccessUnitBytes,
		(void**)&p1, &b1);
	// if(hr == DSERR_BUFFERLOST)
	// {
	// 	// retry after restoring lost buffer memory
	// 	SoundBuffer->Restore();
	// 	hr = SoundBuffer->Lock(writepos, AccessUnitBytes,
	// 		(void**)&p1, &b1, (void**)&p2, &b2, 0);
	// }

	if(hr)
	{
		tjs_uint decoded;

		if(UseVisBuffer)
		{
			decoded = ReadL2Buffer(VisBuffer + writepos, segments);
			memcpy(p1, VisBuffer + writepos, AccessUnitBytes);
		}
		else
		{
			decoded = ReadL2Buffer(p1, segments);
		}

		if(PlayStopPos == -1 && decoded < (tjs_uint)AccessUnitSamples)
		{
			// decoding was finished
			PlayStopPos = writepos + decoded*Format.nBlockAlign;
			SDL_Log("will stop on :%d",PlayStopPos);
				// set stop position
		}

		SoundBuffer->unlock((void*)p1, b1);
		SDL_Log("FillDSBuffer ok,pos:%d,len:%d",writepos,decoded);
	}
}
//---------------------------------------------------------------------------
bool tTJSNI_WaveSoundBuffer::FillBuffer(bool firstwrite, bool allowpause)
{
	// fill DirectSound secondary buffer with one render unit.

	tTJSCriticalSectionHolder holder(BufferCS);

	if(!SoundBuffer) return true;
	if(!Decoder) return true;
	if(!BufferPlaying) return true;
	SDL_Log("FillBuffer:firstwrite:%d,cur_pos:%d,remain:%d",
			firstwrite,SoundBufferWritePos,L2BufferRemain);


	// check paused state
	if(allowpause)
	{
		if(Paused)
		{
			if(DSBufferPlaying)
			{
				SoundBuffer->pause();
				DSBufferPlaying = false;
			}
			return true;
		}
		else
		{
			if(!DSBufferPlaying)
			{
				SoundBuffer->play();
				DSBufferPlaying = true;
			}
		}
	}

	// check decoder thread status
	tjs_int bufferremain;
	{
		tTJSCriticalSectionHolder holder(L2BufferRemainCS);
		bufferremain = L2BufferRemain;
	}

	if(Thread->GetRunning() && bufferremain < TVP_WSB_ACCESS_FREQ )
		Thread->SetPriority(ttpNormal); // buffer remains under 1 sec 

	// check buffer playing position
	tjs_int writepos;

	int pp = 0, wp = 0; // write pos and read pos
	SoundBuffer->GetCurrentPosition(pp, wp);

	TVPPushEnvironNoise(&pp, sizeof(pp));
	TVPPushEnvironNoise(&wp, sizeof(wp));
		// drift between main clock and clocks which come from other sources
		// is a good environ noise.

	// check position
	tTVPWaveSegmentQueue * segment;
	tjs_int64 * bufferdecodesamplepos;

	if(firstwrite)
	{
		writepos = 0;
		segment = L1BufferSegmentQueues + 0;
		bufferdecodesamplepos = L1BufferDecodeSamplePos + 0;
		PlayStopPos = -1;
		SoundBufferWritePos = 1;
		SoundBufferPrevReadPos = 0;
	}
	else
	{
		ResetLastCheckedDecodePos(pp);

		if(PlayStopPos != -1)
		{
			// check whether the buffer playing position passes over PlayStopPos
			if(SoundBufferPrevReadPos > (tjs_int)pp)
			{
				if(PlayStopPos >= SoundBufferPrevReadPos ||
					PlayStopPos < (tjs_int)pp)
				{
					FlushAllLabelEvents();
					SoundBuffer->stop();
					ResetSamplePositions();
					DSBufferPlaying = false;
					BufferPlaying = false;
					if(LoopManager) LoopManager->SetPosition(0);
					return true;
				}
			}
			else
			{
				if(PlayStopPos >= SoundBufferPrevReadPos &&
					PlayStopPos < (tjs_int)pp)
				{
					FlushAllLabelEvents();
					SoundBuffer->stop();
					ResetSamplePositions();
					DSBufferPlaying = false;
					BufferPlaying = false;
					if(LoopManager) LoopManager->SetPosition(0);
					return true;
				}
			}
		}

		// tjs_int wpb = wp / AccessUnitBytes;
		// tjs_int ppb = pp / AccessUnitBytes;

		// tjs_int d = wpb - SoundBufferWritePos;
		// if(d < 0) d += L1BufferUnits;

		// wpb -= ppb;
		// if(wpb < 0) wpb += L1BufferUnits;

		// if(d <= wpb)
		// {
		// 	// pp thru wp is currently playing position; cannot write there
		// 	SDL_Log("====Can't not write!%d,%d,%d,%d",d,wpb,pp,wp);
		// 	return true;
		// }
		if(!SoundBuffer->writeable())
		{
			auto p = SoundBuffer->plb;
			SDL_Log("===can't write!:rp:%d,rsize:%d,wp:%d,wsize%d:",
			p->r_pos,p->readable_len.load(),p->w_pos,p->len - p->readable_len.load()
			);

			return true;
		}

		writepos = SoundBufferWritePos * AccessUnitBytes;
		segment = L1BufferSegmentQueues + SoundBufferWritePos;
		bufferdecodesamplepos = L1BufferDecodeSamplePos + SoundBufferWritePos;
		SoundBufferWritePos ++;
		if(SoundBufferWritePos >= L1BufferUnits)
			SoundBufferWritePos = 0;
	}

	SoundBufferPrevReadPos = pp;

	// decode
	if(bufferremain > 1) // buffer is ready
	{
		// with no locking operations
		FillDSBuffer(writepos, *segment);
	}
	else
	{
		PrepareToReadL2Buffer(false); // complete decoding before reading from L2

		{
			tTJSCriticalSectionHolder l2holder(L2BufferCS);
			FillDSBuffer(writepos, *segment);
		}
	}

	// insert labels into LabelEventQueue and sort
	const std::deque<tTVPWaveLabel> & labels = segment->GetLabels();
	if(labels.size() != 0)
	{
		// add DecodePos offset to each item->Offset
		// and insert into LabelEventQueue
		for(std::deque<tTVPWaveLabel>::const_iterator i = labels.begin();
			i != labels.end(); i++)
		{
			LabelEventQueue.push_back(
				tTVPWaveLabel(i->Position,
						i->Name, static_cast<tjs_int>(i->Offset + DecodePos)));
		}

		// sort
		std::sort(LabelEventQueue.begin(), LabelEventQueue.end(),
			tTVPWaveLabel::tSortByOffsetFuncObj());

		// re-schedule label events
		TVPReschedulePendingLabelEvent(GetNearestEventStep());
	}

	// write bufferdecodesamplepos
	*bufferdecodesamplepos = DecodePos;
	DecodePos += AccessUnitSamples;

	return false;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::ResetLastCheckedDecodePos(int pp)
{
	// set LastCheckedDecodePos and  LastCheckedTick
	// we shoud reset these values because the clock sources are usually
	// not identical.
	tTJSCriticalSectionHolder holder(BufferCS);

	if(pp == -1)
	{
		if(!SoundBuffer) return;
		int wp = 0;
		SoundBuffer->GetCurrentPosition(pp, wp);
	}

	tjs_int ppb = pp / AccessUnitBytes;
	tjs_int ppm = pp % AccessUnitBytes;
	if(L1BufferDecodeSamplePos[ppb] != -1)
	{
		LastCheckedDecodePos = L1BufferDecodeSamplePos[ppb] + ppm / Format.nBlockAlign;
		LastCheckedTick = TVPGetTickCount();
	}
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_WaveSoundBuffer::FireLabelEventsAndGetNearestLabelEventStep(tjs_int64 tick)
{
	// fire events, event.EventTick <= tick, and return relative time to
	// next nearest event (return TVP_TIMEOFS_INVALID_VALUE for no events).

	// the vector LabelEventQueue must be sorted by the position.
	tTJSCriticalSectionHolder holder(BufferCS);

	if(!BufferPlaying) return TVP_TIMEOFS_INVALID_VALUE; // buffer is not currently playing
	if(!DSBufferPlaying) return TVP_TIMEOFS_INVALID_VALUE; // direct sound buffer is not currently playing

	if(LabelEventQueue.size() == 0) return TVP_TIMEOFS_INVALID_VALUE; // no more events

	// calculate current playing decodepos
	// at this point, LastCheckedDecodePos must not be -1
	if(LastCheckedDecodePos == -1) ResetLastCheckedDecodePos();
	tjs_int64 decodepos = (tick - LastCheckedTick) * Frequency / 1000 +
		LastCheckedDecodePos;

	while(true)
	{
		if(LabelEventQueue.size() == 0) break;
		std::vector<tTVPWaveLabel>::iterator i = LabelEventQueue.begin();
		int diff = (tjs_int32)i->Offset - (tjs_int32)decodepos;
		if(diff <= 0)
			InvokeLabelEvent(i->Name);
		else
			break;
		LabelEventQueue.erase(i);
	}

	if(LabelEventQueue.size() == 0) return TVP_TIMEOFS_INVALID_VALUE; // no more events

	return (tjs_int)(
		(LabelEventQueue[0].Offset - (tjs_int32)decodepos) * 1000 / Frequency);
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_WaveSoundBuffer::GetNearestEventStep()
{
	// get nearest event stop from current tick
	// (current tick is taken from TVPGetTickCount)
	tTJSCriticalSectionHolder holder(BufferCS);

	if(LabelEventQueue.size() == 0) return TVP_TIMEOFS_INVALID_VALUE; // no more events

	// calculate current playing decodepos
	// at this point, LastCheckedDecodePos must not be -1
	if(LastCheckedDecodePos == -1) ResetLastCheckedDecodePos();
	tjs_int64 decodepos = (TVPGetTickCount() - LastCheckedTick) * Frequency / 1000 +
		LastCheckedDecodePos;

	return (tjs_int)(
		(LabelEventQueue[0].Offset - (tjs_int32)decodepos) * 1000 / Frequency);
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::FlushAllLabelEvents()
{
	// called at the end of the decode.
	// flush all undelivered events.
	tTJSCriticalSectionHolder holder(BufferCS);

	for(std::vector<tTVPWaveLabel>::iterator i = LabelEventQueue.begin();
		i != LabelEventQueue.end(); i++)
		InvokeLabelEvent(i->Name);

	LabelEventQueue.clear();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::StartPlay()
{
	if(!Decoder) return;

	// let primary buffer to start running
	TVPEnsurePrimaryBufferPlay();

	// ensure playing thread
	TVPEnsureWaveSoundBufferWorking();

	// play from first

	{	// thread protected block
		if(Thread->GetRunning()) { Thread->SetPriority(TVPDecodeThreadHighPriority); }
		tTJSCriticalSectionHolder holder(BufferCS);
		tTJSCriticalSectionHolder l2holder(L2BufferCS);

		CreateSoundBuffer();

		// reset filter chain
		ResetFilterChain();

		// fill sound buffer with some first samples
		BufferPlaying = true;
		FillL2Buffer(true, false);
		FillBuffer(true, false);
		FillBuffer(false, false);
		FillBuffer(false, false);
		FillBuffer(false, false);

		// start playing
		if(!Paused)
		{
			SoundBuffer->play();
			DSBufferPlaying = true;
		}

		// re-schedule label events
		ResetLastCheckedDecodePos();
		TVPReschedulePendingLabelEvent(GetNearestEventStep());
	}	// end of thread protected block

	// ensure thread
	TVPEnsureWaveSoundBufferWorking(); // wake the playing thread up again
	ThreadCallbackEnabled = true;
	// Thread->Continue();

}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::StopPlay()
{
	if(!Decoder) return;
	if(!SoundBuffer) return;

	if(Thread->GetRunning()) { Thread->SetPriority(TVPDecodeThreadHighPriority);}
	tTJSCriticalSectionHolder holder(BufferCS);
	tTJSCriticalSectionHolder l2holder(L2BufferCS);

	SoundBuffer->stop();
	DSBufferPlaying = false;
	BufferPlaying = false;

}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::Play()
{
	// play from first or current position
	if(!Decoder) return;
	if(BufferPlaying) return;

	StopPlay();

	TVPEnsurePrimaryBufferPlay(); // let primary buffer to start running

	if(Thread->GetRunning()) { Thread->SetPriority(TVPDecodeThreadHighPriority);}
	tTJSCriticalSectionHolder holder(BufferCS);
	tTJSCriticalSectionHolder l2holder(L2BufferCS);

	StartPlay();
	SetStatus(ssPlay);
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::Stop()
{
	// stop playing
	StopPlay();

	// delete thread
	ThreadCallbackEnabled = false;
	TVPCheckSoundBufferAllSleep();
	Thread->Interrupt();

	// set status
	if(Status != ssUnload) SetStatus(ssStop);

	// rewind
	if(LoopManager) LoopManager->SetPosition(0);
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPaused(bool b)
{
	if(Thread->GetRunning())
		{/*orgpri = Thread->Priority;*/
			Thread->SetPriority(TVPDecodeThreadHighPriority); }
	tTJSCriticalSectionHolder holder(BufferCS);
	tTJSCriticalSectionHolder l2holder(L2BufferCS);

	Paused = b;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::TimerBeatHandler()
{
	inherited::TimerBeatHandler();

	// check buffer stopping
	if(Status == ssPlay && !BufferPlaying)
	{
		// buffer was stopped
		ThreadCallbackEnabled = false;
		TVPCheckSoundBufferAllSleep();
		Thread->Interrupt();
		SetStatusAsync(ssStop);
	}
}
//---------------------------------------------------------------------------
//main thread.
void tTJSNI_WaveSoundBuffer::Open(const ttstr & storagename)
{
	// open a storage and prepare to play
	TVPEnsurePrimaryBufferPlay(); // let primary buffer to start running

	Clear();
	SDL_Log("tTJSNI_WaveSoundBuffer::open!!!:%ls",storagename.c_str());

	Decoder = TVPCreateWaveDecoder(storagename);

	try
	{
		// make manager
		LoopManager = new tTVPWaveLoopManager();
		LoopManager->SetDecoder(Decoder);
		LoopManager->SetLooping(Looping);

		// build filter chain
		RebuildFilterChain();

		// retrieve format
		InputFormat = FilterOutput->GetFormat();
		Frequency = InputFormat.SamplesPerSec;
	}
	catch(...)
	{
		Clear();
		throw;
	}

	// open loop information file
	ttstr sliname = storagename + TJS_W(".sli");
	if(TVPIsExistentStorage(sliname))
	{
		tTVPStreamHolder slistream(sliname);
		char *buffer;
		tjs_uint size;
		buffer = new char [ (size = static_cast<tjs_uint>(slistream->GetSize())) +1];
		try
		{
			slistream->ReadBuffer(buffer, size);
			buffer[size] = 0;

			if(!LoopManager->ReadInformation(buffer))
				TVPThrowExceptionMessage(TVPInvalidLoopInformation, sliname);
			RecreateWaveLabelsObject();
		}
		catch(...)
		{
			delete [] buffer;
			Clear();
			throw;
		}
		delete [] buffer;
	}

	// set status to stop
	SetStatus(ssStop);
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetLooping(bool b)
{
	Looping = b;
	if(LoopManager) LoopManager->SetLooping(Looping);
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSNI_WaveSoundBuffer::GetSamplePosition()
{
	if(!Decoder) return 0L;
	if(!SoundBuffer) return 0L;

	tTJSCriticalSectionHolder holder(BufferCS);

	int wp, pp;
	SoundBuffer->GetCurrentPosition(pp, wp);

	tjs_int rblock = pp / AccessUnitBytes;

	tTVPWaveSegmentQueue & segs = L1BufferSegmentQueues[rblock];

	tjs_int offset = pp % AccessUnitBytes;

	offset /= Format.nBlockAlign;

	return segs.FilteredPositionToDecodePosition(offset);
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetSamplePosition(tjs_uint64 pos)
{
	tjs_uint64 possamples = pos; // in samples

	if(InputFormat.TotalSamples && InputFormat.TotalSamples <= possamples) return;

	if(BufferPlaying && DSBufferPlaying)
	{
		StopPlay();
		LoopManager->SetPosition(possamples);
		StartPlay();
	}
	else
	{
		LoopManager->SetPosition(possamples);
	}
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSNI_WaveSoundBuffer::GetPosition()
{
	if(!Decoder) return 0L;
	if(!SoundBuffer) return 0L;

	return GetSamplePosition() * 1000 / Format.nSamplesPerSec;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPosition(tjs_uint64 pos)
{
	SetSamplePosition(pos * Format.nSamplesPerSec / 1000); // in samples
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSNI_WaveSoundBuffer::GetTotalTime()
{
	return InputFormat.TotalSamples * 1000 / Format.nSamplesPerSec;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetVolumeToSoundBuffer()
{
	// set current volume/pan to DirectSound buffer
	if(SoundBuffer)
	{
		tjs_int v;
		tjs_int mutevol = 100000;
		if(TVPSoundGlobalFocusModeByOption >= sgfmMuteOnDeactivate &&
			TVPSoundGlobalFocusMuteVolume == 0)
		{
			// no mute needed here;
			// muting will be processed in DirectSound framework.
			;
		}
		else
		{
			// mute mode is choosen from GlobalFocusMode or
			// TVPSoundGlobalFocusModeByOption which is more restrictive.
			tTVPSoundGlobalFocusMode mode =
				GlobalFocusMode > TVPSoundGlobalFocusModeByOption ?
				GlobalFocusMode : TVPSoundGlobalFocusModeByOption;

			switch(mode)
			{
			case sgfmNeverMute:
				;
				break;
			case sgfmMuteOnMinimize:
				// if(!  Application->GetNotMinimizing())
					mutevol = TVPSoundGlobalFocusMuteVolume;
				break;
			case sgfmMuteOnDeactivate:
				// if(! (  Application->GetActivating() && Application->GetNotMinimizing()))
					mutevol = TVPSoundGlobalFocusMuteVolume;
				break;
			}
		}

		// compute volume for each buffer
		v = (Volume / 10) * (Volume2 / 10) / 1000;
		v = (v / 10) * (GlobalVolume / 10) / 1000;
		v = (v / 10) * (mutevol / 10) / 1000;
		// SoundBuffer->SetVolume(TVPVolumeToDSAttenuate(v));
		SoundBuffer->SetVolume(v);

		// if(BufferCanControlPan)
		// {
		// 	// set pan
		// 	SoundBuffer->SetPan(TVPPanToDSAttenuate(Pan));
		// }
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetVolume(tjs_int v)
{
	if(v < 0) v = 0;
	if(v > 100000) v = 100000;

	if(Volume != v)
	{
		Volume = v;
		SetVolumeToSoundBuffer();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetVolume2(tjs_int v)
{
	if(v < 0) v = 0;
	if(v > 100000) v = 100000;

	if(Volume2 != v)
	{
		Volume2 = v;
		SetVolumeToSoundBuffer();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPan(tjs_int v)
{
	if(v < -100000) v = -100000;
	if(v > 100000) v = 100000;
	if(Pan != v)
	{
		Pan = v;
		if(BufferCanControlPan)
		{
			// set pan with SetPan
			SetVolumeToSoundBuffer();
		}
		else
		{
			// set pan with 3D sound facility
			// note that setting pan can reset 3D position.
			PosZ = (D3DVALUE)0.0;
			PosY = (D3DVALUE)0.001;
			// PosX = -0.003 .. -0.0001 = 0 = +0.0001 ... +0.003
			float t;
			t = static_cast<float>( ((float)v / 100000.0) );
			t *= static_cast<float>( t * 0.003 );
			if(v < 0) t = - t;
			PosX = t;
			Set3DPositionToBuffer();
		}
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetGlobalVolume(tjs_int v)
{
	if(v < 0) v = 0;
	if(v > 100000) v = 100000;

	if(GlobalVolume != v)
	{
		GlobalVolume = v;
		TVPResetVolumeToAllSoundBuffer();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetGlobalFocusMode(tTVPSoundGlobalFocusMode b)
{
	if(GlobalFocusMode != b)
	{
		GlobalFocusMode = b;
		TVPResetVolumeToAllSoundBuffer();
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::Set3DPositionToBuffer()
{
	if(Sound3DBuffer)
	{
		// Sound3DBuffer->SetPosition(PosX, PosY, PosZ, DS3D_DEFERRED);
		// defered settings are to be commited at next tickbeat event.
		TVPDeferedSettingAvailable = true;
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPos(D3DVALUE x, D3DVALUE y, D3DVALUE z)
{
	PosX = x;
	PosY = y;
	PosZ = z;
	Set3DPositionToBuffer();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPosX(D3DVALUE v)
{
	PosX = v;
	Set3DPositionToBuffer();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPosY(D3DVALUE v)
{
	PosY = v;
	Set3DPositionToBuffer();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetPosZ(D3DVALUE v)
{
	PosZ = v;
	Set3DPositionToBuffer();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetFrequencyToBuffer()
{
	if(BufferCanControlFrequency)
	{
		if(SoundBuffer) SoundBuffer->SetFrequency(Frequency);
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetFrequency(tjs_int freq)
{
	// set frequency
	Frequency = freq;
	SetFrequencyToBuffer();
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::SetUseVisBuffer(bool b)
{
	tTJSCriticalSectionHolder holder(BufferCS);

	if(b)
	{
		UseVisBuffer = true;

		if(SoundBuffer) ResetVisBuffer();
	}
	else
	{
		DeallocateVisBuffer();
		UseVisBuffer = false;
	}
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::ResetVisBuffer()
{
	// reset or recreate visualication buffer
	tTJSCriticalSectionHolder holder(BufferCS);

	DeallocateVisBuffer();

	VisBuffer = new tjs_uint8 [BufferBytes];
	UseVisBuffer = true;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::DeallocateVisBuffer()
{
	tTJSCriticalSectionHolder holder(BufferCS);

	if(VisBuffer) delete [] VisBuffer, VisBuffer = NULL;
	UseVisBuffer = false;
}
//---------------------------------------------------------------------------
void tTJSNI_WaveSoundBuffer::CopyVisBuffer(tjs_int16 *dest, const tjs_uint8 *src,
	tjs_int numsamples, tjs_int channels)
{
	bool isfloat = Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT;

	if(channels == 1)
	{
		TVPConvertPCMTo16bits(dest, (const void*)src, Format.nChannels,
			Format.wBitsPerSample / 8, Format.wBitsPerSample,
			isfloat, numsamples, true);
	}
	else if(channels == Format.nChannels)
	{
		TVPConvertPCMTo16bits(dest, (const void*)src, Format.nChannels,
			Format.wBitsPerSample / 8, Format.wBitsPerSample,
			isfloat, numsamples, false);
	}
}
//---------------------------------------------------------------------------
tjs_int tTJSNI_WaveSoundBuffer::GetVisBuffer(tjs_int16 *dest, tjs_int numsamples,
	tjs_int channels, tjs_int aheadsamples)
{
	// read visualization buffer samples
	if(!UseVisBuffer) return 0;
	if(!VisBuffer) return 0;
	if(!Decoder) return 0;
	if(!SoundBuffer) return 0;
	if(!DSBufferPlaying || !BufferPlaying) return 0;

	if(channels != Format.nChannels && channels != 1) return 0;

	// retrieve current playing position

	int wp, pp;
	{
		tTJSCriticalSectionHolder holder(BufferCS);
		// the critical section protects only here;
		// the rest is not important code (does anyone care about that the retrieved
		// visualization becomes wrong a little ?)

		SoundBuffer->GetCurrentPosition(pp, wp);

		pp += aheadsamples * Format.nBlockAlign;
		pp = pp % BufferBytes;

		if(L1BufferSegmentQueues[pp/AccessUnitBytes].GetFilteredLength() == 0)
			return 0;
	}

	pp /= Format.nBlockAlign;

	tjs_int buffersamples = BufferBytes / Format.nBlockAlign;

	// copy to distination buffer
	tjs_int writtensamples = 0;
	if(numsamples > 0)
	{
		while(true)
		{
			tjs_int bufrest = buffersamples - pp;
			tjs_int copysamples = (bufrest > numsamples ? numsamples : bufrest);

			CopyVisBuffer(dest, VisBuffer + pp * Format.nBlockAlign,
				copysamples, channels);

			numsamples -= copysamples;
			writtensamples += copysamples;
			if(numsamples <= 0) break;

			dest += channels * copysamples;
			pp += copysamples;
			pp = pp % buffersamples;
		}
	}

	return writtensamples;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSNC_WaveSoundBuffer
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_WaveSoundBuffer::CreateNativeInstance()
{
	return new tTJSNI_WaveSoundBuffer();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPCreateNativeClass_WaveSoundBuffer
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_WaveSoundBuffer()
{
	tTJSNativeClass *cls = new tTJSNC_WaveSoundBuffer();
	static tjs_uint32 TJS_NCM_CLASSID;
	TJS_NCM_CLASSID = tTJSNC_WaveSoundBuffer::ClassID;

//----------------------------------------------------------------------
// methods
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/freeDirectSound)  /* static */
{
	// release directsound
	TVPReleaseDirectSound();

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL_OUTER(/*object to register*/cls,
	/*func. name*/freeDirectSound)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getVisBuffer)
{
	// get samples for visualization 
	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
		/*var. type*/tTJSNI_WaveSoundBuffer);

	if(numparams < 3) return TJS_E_BADPARAMCOUNT;
	tjs_int16 *dest = (tjs_int16*)(tjs_int)(*param[0]);

	tjs_int ahead = 0;
	if(numparams >= 4) ahead = (tjs_int)*param[3];

	tjs_int res = _this->GetVisBuffer(dest, *param[1], *param[2], ahead);

	if(result) *result = res;

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL_OUTER(/*object to register*/cls,
	/*func. name*/getVisBuffer)
//----------------------------------------------------------------------



//----------------------------------------------------------------------
// properties
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(useVisBuffer)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
			/*var. type*/tTJSNI_WaveSoundBuffer);

		*result = _this->GetUseVisBuffer();
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_BEGIN_NATIVE_PROP_SETTER
	{
		TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
			/*var. type*/tTJSNI_WaveSoundBuffer);

		_this->SetUseVisBuffer(0!=(tjs_int)*param);

		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL_OUTER(cls, useVisBuffer)
//----------------------------------------------------------------------
	return cls;
}
//---------------------------------------------------------------------------

