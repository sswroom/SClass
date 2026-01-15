#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioSource.h"
#include "Media/MediaSource.h"
#include "Media/RefClock.h"
#include "Media/WaveOutRenderer.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#if !defined(_WIN32_WCE)
#include <ks.h>
#include <ksmedia.h>
#elif (_WIN32_WCE >= 0x501)
#define WAVEOUTCAPSW WAVEOUTCAPS
#define waveOutGetDevCapsW waveOutGetDevCaps

#if defined(__cplusplus) && _MSC_VER >= 1100
#define DEFINE_GUIDSTRUCT(g, n) struct __declspec(uuid(g)) n
#define DEFINE_GUIDNAMED(n) __uuidof(struct n)
#else // !defined(__cplusplus)
#define DEFINE_GUIDSTRUCT(g, n) DEFINE_GUIDEX(n)
#define DEFINE_GUIDNAMED(n) n
#endif // !defined(__cplusplus)

#if !defined( STATIC_KSDATAFORMAT_SUBTYPE_PCM )
#define STATIC_KSDATAFORMAT_SUBTYPE_PCM\
    DEFINE_WAVEFORMATEX_GUID(WAVE_FORMAT_PCM)
DEFINE_GUIDSTRUCT("00000001-0000-0010-8000-00aa00389b71", KSDATAFORMAT_SUBTYPE_PCM);
#define KSDATAFORMAT_SUBTYPE_PCM DEFINE_GUIDNAMED(KSDATAFORMAT_SUBTYPE_PCM)
#endif
#else
#define WAVEOUTCAPSW WAVEOUTCAPS
#define waveOutGetDevCapsW waveOutGetDevCaps
#define WAVE_FORMAT_EXTENSIBLE 65534

typedef struct WAVEFORMATEXTENSIBLE {
    WAVEFORMATEX Format;
	union {
		WORD wValidBitsPerSample;
		WORD wSamplesPerBlock;
		WORD wReserved;
	} Samples;
	DWORD dwChannelMask;
	GUID SubFormat;
} WAVEFORMATEXTENSIBLE;

#if defined(__cplusplus) && _MSC_VER >= 1100
#define DEFINE_GUIDSTRUCT(g, n) struct __declspec(uuid(g)) n
#define DEFINE_GUIDNAMED(n) __uuidof(struct n)
#else // !defined(__cplusplus)
#define DEFINE_GUIDSTRUCT(g, n) DEFINE_GUIDEX(n)
#define DEFINE_GUIDNAMED(n) n
#endif // !defined(__cplusplus)

#if !defined( STATIC_KSDATAFORMAT_SUBTYPE_PCM )
#define STATIC_KSDATAFORMAT_SUBTYPE_PCM\
    DEFINE_WAVEFORMATEX_GUID(WAVE_FORMAT_PCM)
DEFINE_GUIDSTRUCT("00000001-0000-0010-8000-00aa00389b71", KSDATAFORMAT_SUBTYPE_PCM);
#define KSDATAFORMAT_SUBTYPE_PCM DEFINE_GUIDNAMED(KSDATAFORMAT_SUBTYPE_PCM)
#endif

// Speaker Positions:
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000
#endif
#define BUFFLENG 16384

void __stdcall Media::WaveOutRenderer::WaveEvents(void *hwo, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2)
{
	Media::WaveOutRenderer *me = (Media::WaveOutRenderer*)dwInstance;
	if (uMsg == WOM_DONE)
	{
		me->buffEmpty[((WAVEHDR*)dwParam1)->dwUser] = true;
		me->playEvt->Set();
	}
}

UInt32 __stdcall Media::WaveOutRenderer::PlayThread(AnyType obj)
{
	NN<Media::WaveOutRenderer> me = obj.GetNN<Media::WaveOutRenderer>();
	Media::AudioFormat af;
	Sync::Event *evt;
	WAVEHDR hdrs[4];
	UInt32 i;
	Data::Duration refStart;
	Data::Duration audStartTime;
	MMTIME mmt;
	UIntOS buffLeng = BUFFLENG;
	UIntOS minLeng;
	Data::Duration thisT;
	Data::Duration lastT;
	Int32 stmEnd;
	Bool needNotify = false;
	NN<Media::AudioSource> audsrc;
	NN<Media::RefClock> clk;

	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	NEW_CLASS(evt, Sync::Event());

	me->playing = true;
	me->threadInit = true;
	if (me->audsrc.SetTo(audsrc))
	{
		audsrc->GetFormat(af);
		if (me->buffTime)
		{
			buffLeng = (me->buffTime * af.frequency / 1000) * af.align;
		}
		i = 4;
		MemClear(&hdrs[0], sizeof(WAVEHDR) * 4);
		audStartTime = audsrc->GetCurrTime();
		minLeng = audsrc->GetMinBlockSize();
		if (minLeng > buffLeng)
			buffLeng = minLeng;

		if (me->clk.SetTo(clk)) clk->Start(audStartTime);
		audsrc->Start(evt, buffLeng);

		waveOutRestart((HWAVEOUT)me->hwo);
		waveOutGetPosition((HWAVEOUT)me->hwo, &mmt, sizeof(mmt));
		lastT = thisT = Media::WaveOutRenderer::GetDurFromTime(&mmt, af);
		refStart = thisT - audStartTime;
		stmEnd = 0;

		while (i-- > 0)
		{
			hdrs[i].dwBufferLength = (DWORD)buffLeng;
			hdrs[i].lpData = MemAlloc(CHAR, buffLeng);
			hdrs[i].dwUser = i;
			me->buffEmpty[i] = false;
			hdrs[i].dwBufferLength = (DWORD)audsrc->ReadBlockLPCM(Data::ByteArray((UInt8*)hdrs[i].lpData, buffLeng), af);

			waveOutPrepareHeader((HWAVEOUT)me->hwo, &hdrs[i], sizeof(WAVEHDR));
			waveOutWrite((HWAVEOUT)me->hwo, &hdrs[i], sizeof(WAVEHDR));
		}

		while (!me->stopPlay)
		{
			i = 4;
			while (i-- > 0)
			{
				if (me->buffEmpty[i])
				{
					me->buffEmpty[i] = false;
					if (stmEnd == 4)
					{
						me->stopPlay = true;
						audsrc->Stop();
						needNotify = true;
						break;
					}
					else if (stmEnd > 0)
					{
						stmEnd++;
						hdrs[i].dwBufferLength = (DWORD)buffLeng;
						MemClear(hdrs[i].lpData, buffLeng);
					}
					else
					{
						hdrs[i].dwBufferLength = (DWORD)audsrc->ReadBlockLPCM(Data::ByteArray((UInt8*)hdrs[i].lpData, buffLeng), af);
						if (hdrs[i].dwBufferLength == 0)
						{
							hdrs[i].dwBufferLength = (DWORD)buffLeng;
							MemClear(hdrs[i].lpData, buffLeng);
							stmEnd = 1;
						}
					}

					waveOutPrepareHeader((HWAVEOUT)me->hwo, &hdrs[i], sizeof(WAVEHDR));
					waveOutWrite((HWAVEOUT)me->hwo, &hdrs[i], sizeof(WAVEHDR));

					waveOutGetPosition((HWAVEOUT)me->hwo, &mmt, sizeof(mmt));
					thisT = Media::WaveOutRenderer::GetDurFromTime(&mmt, af);
					if (lastT > thisT)
					{
						waveOutReset((HWAVEOUT)me->hwo);
						waveOutRestart((HWAVEOUT)me->hwo);
						waveOutGetPosition((HWAVEOUT)me->hwo, &mmt, sizeof(mmt));
						lastT = thisT = Media::WaveOutRenderer::GetDurFromTime(&mmt, af);
						refStart = thisT - audsrc->GetCurrTime();
					}
					else
					{
						if (me->clk.SetTo(clk)) clk->Start(thisT - refStart);
						lastT = thisT;
					}
				}

			}

			me->playEvt->Wait();
		}

		waveOutPause((HWAVEOUT)me->hwo);
		waveOutReset((HWAVEOUT)me->hwo);
		i = 4;
		while (i-- > 0)
		{
			MemFree(hdrs[i].lpData);
		}
	}
	DEL_CLASS(evt);
	me->playing = false;

	if (needNotify)
	{
		if (me->endHdlr)
		{
			me->endHdlr(me->endHdlrObj);
		}
	}
	return 0;
}

UInt32 Media::WaveOutRenderer::GetMSFromTime(void *mmTime, NN<const AudioFormat> fmt)
{
	MMTIME *mmt = (MMTIME *)mmTime;
	if (mmt->wType == TIME_MS)
	{
		return mmt->u.ms;
	}
	else if (mmt->wType == TIME_SAMPLES)
	{
		return MulDivU32(mmt->u.sample, 1000, fmt->frequency);
	}
	else if (mmt->wType == TIME_BYTES)
	{
		return MulDivU32(mmt->u.cb, 1000, fmt->frequency * fmt->nChannels * fmt->bitpersample >> 3);
	}
	else if (mmt->wType == TIME_SMPTE)
	{
		return (UInt32)mmt->u.smpte.hour * 3600000 + 
			(UInt32)mmt->u.smpte.min * 60000 + 
			(UInt32)mmt->u.smpte.sec * 1000 + 
			MulDivU32(mmt->u.smpte.frame, 1000, mmt->u.smpte.fps);
	}
	else
	{
		return 0;
	}
}

Data::Duration Media::WaveOutRenderer::GetDurFromTime(void *mmTime, NN<const AudioFormat> fmt)
{
	MMTIME *mmt = (MMTIME *)mmTime;
	if (mmt->wType == TIME_MS)
	{
		return mmt->u.ms;
	}
	else if (mmt->wType == TIME_SAMPLES)
	{
		return Data::Duration::FromRatioU64(mmt->u.sample, fmt->frequency);
	}
	else if (mmt->wType == TIME_BYTES)
	{
		return Data::Duration::FromRatioU64(mmt->u.cb, fmt->frequency * fmt->nChannels * fmt->bitpersample >> 3);
	}
	else if (mmt->wType == TIME_SMPTE)
	{
		return Data::Duration::FromSecNS(mmt->u.smpte.hour * 3600 + mmt->u.smpte.min * 60 + mmt->u.smpte.sec, MulDivU32(mmt->u.smpte.frame, 1000000000, mmt->u.smpte.fps));
	}
	else
	{
		return 0;
	}
}

UIntOS Media::WaveOutRenderer::GetDeviceCount()
{
	return waveOutGetNumDevs();
}

UnsafeArrayOpt<UTF8Char> Media::WaveOutRenderer::GetDeviceName(UnsafeArray<UTF8Char> buff, UIntOS devNo)
{
	WAVEOUTCAPSW caps;
	waveOutGetDevCapsW(devNo, &caps, sizeof(caps));
	return Text::StrWChar_UTF8(buff, caps.szPname);
}

Media::WaveOutRenderer::WaveOutRenderer(UnsafeArrayOpt<const UTF8Char> devName)
{
	UIntOS i = GetDeviceCount();
	UTF8Char buff[256];
	this->devId = -1;
	UnsafeArray<const UTF8Char> nndevName;
	if (!devName.SetTo(nndevName))
	{
		this->devId = 0;
	}
	else
	{
		while (i-- > 0)
		{
			if (GetDeviceName(buff, i).NotNull())
			{
				if (Text::StrEquals(buff, nndevName))
				{
					this->devId = (IntOS)i;
					break;
				}
			}
		}
	}
	this->audsrc = nullptr;
	this->playing = false;
	this->endHdlr = 0;
	this->buffTime = 0;
}

Media::WaveOutRenderer::WaveOutRenderer(Int32 devId)
{
	this->devId = devId;
	this->audsrc = nullptr;
	this->playing = false;
	this->endHdlr = 0;
	this->buffTime = 0;
}

Media::WaveOutRenderer::~WaveOutRenderer()
{
	if (this->audsrc.NotNull())
	{
		BindAudio(nullptr);
	}
}

Bool Media::WaveOutRenderer::IsError()
{
	return false;
}

Bool Media::WaveOutRenderer::BindAudio(Optional<Media::AudioSource> audsrc)
{
	HWAVEOUT hwo;
	Media::AudioFormat fmt;
	NN<Media::AudioSource> nnaudsrc;
	if (playing)
	{
		Stop();
	}
	if (this->audsrc.NotNull())
	{
		waveOutClose((HWAVEOUT)this->hwo);
		this->audsrc = nullptr;
		this->hwo = 0;
		DEL_CLASS(playEvt);
	}
	if (!audsrc.SetTo(nnaudsrc))
		return false;

	nnaudsrc->GetFormat(fmt);
	if (fmt.formatId != 1 && fmt.formatId != WAVE_FORMAT_IEEE_FLOAT)
	{
		return false;
	}

	WAVEFORMATEXTENSIBLE format;
	if (fmt.nChannels <= 2 && fmt.bitpersample <= 16)
	{
		format.Format.wFormatTag = 1;
		format.Format.nSamplesPerSec = fmt.frequency;
		format.Format.wBitsPerSample = fmt.bitpersample;
		format.Format.nChannels = fmt.nChannels;
		format.Format.nBlockAlign = (UInt16)(fmt.nChannels * fmt.bitpersample >> 3);
		format.Format.nAvgBytesPerSec = fmt.frequency * format.Format.nBlockAlign;
		format.Format.cbSize = 0;
	}
	else
	{
		format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		format.Format.nSamplesPerSec = fmt.frequency;
		format.Format.wBitsPerSample = fmt.bitpersample;
		format.Format.nChannels = fmt.nChannels;
		format.Format.nBlockAlign = (UInt16)(fmt.nChannels * fmt.bitpersample >> 3);
		format.Format.nAvgBytesPerSec = fmt.frequency * format.Format.nBlockAlign;
		format.Format.cbSize = 22;
		format.Samples.wValidBitsPerSample = fmt.bitpersample;
		if (fmt.formatId == 1)
		{
			format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		}
#if defined(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		else if (fmt.formatId == 3)
		{
			format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		}
#endif
		if (fmt.nChannels == 1)
		{
			format.dwChannelMask = SPEAKER_FRONT_CENTER;
		}
		else if (fmt.nChannels == 2)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
		}
		else if (fmt.nChannels == 3)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER;
		}
		else if (fmt.nChannels == 4)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
		}
		else if (fmt.nChannels == 5)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
		}
		else if (fmt.nChannels == 6)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
		}
		else if (fmt.nChannels == 7)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_BACK_CENTER;
		}
		else if (fmt.nChannels == 8)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT;
		}
	}
	if (waveOutOpen(&hwo, (UINT)this->devId, (WAVEFORMATEX*)&format, (DWORD_PTR)WaveEvents, (DWORD_PTR)this, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
	{
		this->hwo = hwo;
		this->audsrc = audsrc;
		NEW_CLASS(this->playEvt, Sync::Event());
		return true;
	}
	else
	{
		return false;
	}
}

void Media::WaveOutRenderer::AudioInit(Optional<Media::RefClock> clk)
{
	if (playing)
		return;
	if (this->audsrc.IsNull())
		return;
	this->clk = clk;
}

void Media::WaveOutRenderer::Start()
{
	if (playing)
		return;
	if (this->audsrc.IsNull())
		return;
	threadInit = false;
	stopPlay = false;
	Sync::ThreadUtil::Create(PlayThread, this);
	while (!threadInit)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

void Media::WaveOutRenderer::Stop()
{
	NN<Media::AudioSource> audsrc;
	stopPlay = true;
	if (!playing)
		return;
	playEvt->Set();
	if (this->audsrc.SetTo(audsrc))
	{
		audsrc->Stop();
	}
	while (playing)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Bool Media::WaveOutRenderer::IsPlaying()
{
	return this->playing;
}

void Media::WaveOutRenderer::SetEndNotify(EndNotifier endHdlr, AnyType endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

Int32 Media::WaveOutRenderer::GetDeviceVolume()
{
	HMIXER hMxr;
	Int32 vol = -1;
	if (mixerOpen(&hMxr, 0, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
	{
		hMxr = 0;
	}
	else
	{
		MIXERLINE ml;
		MIXERLINECONTROLS mlc;
		MIXERCONTROL mc;
		MIXERCONTROLDETAILS mcd;
		ZeroMemory(&mlc, sizeof(mlc));
		ZeroMemory(&ml, sizeof(ml));
		ml.cbStruct = sizeof(ml);
		ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		if (mixerGetLineInfo((HMIXEROBJ)hMxr, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
		{
			mlc.cbStruct = sizeof(mlc);
			mlc.dwLineID = ml.dwLineID;
			mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mlc.cControls = 1;
			mlc.cbmxctrl = sizeof(mc);
		    mlc.pamxctrl = &mc;
			mc.cbStruct = sizeof(mc);
			if (mixerGetLineControls((HMIXEROBJ)hMxr, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
			{
				mcd.cbStruct = sizeof(mcd);
				mcd.dwControlID = mc.dwControlID;
				mcd.cChannels = 1;
				mcd.hwndOwner = 0;
				mcd.cbDetails = sizeof(vol);
				mcd.paDetails = &vol;
				mixerGetControlDetails((HMIXEROBJ)hMxr, &mcd, MIXER_OBJECTF_HMIXER);
			}
		}
		mixerClose(hMxr);
	}
	return vol;
}

void Media::WaveOutRenderer::SetDeviceVolume(Int32 volume)
{
	HMIXER hMxr;
	if (volume > 65535)
		volume = 65535;
	if (mixerOpen(&hMxr, 0, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
	{
		hMxr = 0;
	}
	else
	{
		MIXERLINE ml;
		MIXERLINECONTROLS mlc;
		MIXERCONTROL mc;
		MIXERCONTROLDETAILS mcd;
		ZeroMemory(&mlc, sizeof(mlc));
		ZeroMemory(&ml, sizeof(ml));
		ml.cbStruct = sizeof(ml);
		ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		if (mixerGetLineInfo((HMIXEROBJ)hMxr, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
		{
			mlc.cbStruct = sizeof(mlc);
			mlc.dwLineID = ml.dwLineID;
			mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mlc.cControls = 1;
			mlc.cbmxctrl = sizeof(mc);
		    mlc.pamxctrl = &mc;
			mc.cbStruct = sizeof(mc);
			if (mixerGetLineControls((HMIXEROBJ)hMxr, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
			{
				mcd.cbStruct = sizeof(mcd);
				mcd.dwControlID = mc.dwControlID;
				mcd.cChannels = 1;
				mcd.hwndOwner = 0;
				mcd.cbDetails = sizeof(volume);
				mcd.paDetails = &volume;
				mixerSetControlDetails((HMIXEROBJ)hMxr, &mcd, MIXER_OBJECTF_HMIXER);
			}
		}
		mixerClose(hMxr);
	}
}

void Media::WaveOutRenderer::SetBufferTime(UInt32 ms)
{
	this->buffTime = ms;
}
