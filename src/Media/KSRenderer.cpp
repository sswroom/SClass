#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/KSRenderer.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "kssample.h"

typedef struct
{
    KSSTREAM_HEADER Header;
    OVERLAPPED      Signal;
} DATA_PACKET;

void Media::KSRenderer::FillFormat(void *formatEx, Media::AudioFormat *fmt)
{
	WAVEFORMATEXTENSIBLE *format = (WAVEFORMATEXTENSIBLE *)formatEx;
	format->Format.wFormatTag = 1;
	format->Format.nChannels = fmt->nChannels;
	format->Format.wBitsPerSample = fmt->bitpersample;
	format->Format.nBlockAlign = (UInt16)(format->Format.nChannels * format->Format.wBitsPerSample >> 3);
	format->Format.nSamplesPerSec = fmt->frequency;
	format->Format.nAvgBytesPerSec = format->Format.nBlockAlign * format->Format.nSamplesPerSec;
	format->Format.cbSize = 0;
	
	if (format->Format.nChannels > 2 || format->Format.wBitsPerSample > 16)
	{
		format->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		format->Format.cbSize = 22;
		format->Samples.wValidBitsPerSample = format->Format.wBitsPerSample;
		switch (format->Format.nChannels)
		{
		case 1:
			format->dwChannelMask = KSAUDIO_SPEAKER_MONO;
			break;
		case 2:
			format->dwChannelMask = KSAUDIO_SPEAKER_STEREO;
			break;
		case 4:
			format->dwChannelMask = KSAUDIO_SPEAKER_QUAD;
			break;
		case 6:
			format->dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			break;
		case 8:
			format->dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
			break;
		default:
			format->dwChannelMask = 0;
		}
		format->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	}

}

UInt32 __stdcall Media::KSRenderer::PlayThread(AnyType obj)
{
	NN<Media::KSRenderer> me = obj.GetNN<Media::KSRenderer>();
	Media::AudioFormat af;
	UOSInt i;
	Data::Duration audStartTime;
//	CKsAudRenFilter *pFilter;
	CKsAudRenPin *pPin;
	Sync::Event *evt;
//	HRESULT hr;
	UOSInt buffLeng = 8192;

//	pFilter = (CKsAudRenFilter*)me->pFilter;
	pPin = (CKsAudRenPin*)me->pPin;

	me->threadInit = true;
	me->audsrc->GetFormat(af);
	audStartTime = me->audsrc->GetCurrTime();
	if (me->buffTime)
	{
		buffLeng = (me->buffTime * af.frequency / 1000) * af.align;
		if (buffLeng > 8192)
			buffLeng = 8192;
	}

	NEW_CLASS(evt, Sync::Event(true));
	me->clk->Start(audStartTime);
	me->playing = true;
	me->audsrc->Start(evt, buffLeng);

	UOSInt cPackets = (af.bitRate >> 17) + 1;
	UOSInt buffEndCnt = 0;
	DATA_PACKET *Packets = MemAlloc(DATA_PACKET, cPackets);
	HANDLE *hEventPool = MemAlloc(HANDLE, cPackets + 1);
	UInt8 *pcmBuffer = MemAlloc(UInt8, buffLeng * cPackets);
	MemClear(&Packets[0], sizeof(DATA_PACKET) * cPackets);

	i = 0;
	while (i < cPackets)
	{
        hEventPool[i] = CreateEvent(NULL, TRUE, TRUE, NULL);	// NO autoreset!

        Packets[i].Signal.hEvent = hEventPool[i];
        Packets[i].Header.Data = &pcmBuffer[buffLeng * i];
        Packets[i].Header.FrameExtent = (ULONG)buffLeng;
        Packets[i].Header.DataUsed = (ULONG)buffLeng;  // if we were capturing, we would init this to 0
        Packets[i].Header.Size = sizeof(Packets[i].Header);
        Packets[i].Header.PresentationTime.Numerator = 1;
        Packets[i].Header.PresentationTime.Denominator = 1;
		i++;
	}

	KSAUDIO_POSITION pos;
    pPin->SetState(KSSTATE_PAUSE);
    pPin->SetState(KSSTATE_RUN);

	UOSInt blkReadSize = buffLeng - (buffLeng % af.align);
	UInt64 skipSize = 0;
	UInt64 initSize = 0;
	i = 0;
	while (i < cPackets)
	{
		Packets[i].Header.DataUsed = (ULONG)me->audsrc->ReadBlockLPCM(Data::ByteArray((UInt8*)Packets[i].Header.Data, blkReadSize), af);
		if (Packets[i].Header.DataUsed == 0)
		{
			me->playEvt->Wait(500);
			Packets[i].Header.DataUsed = (ULONG)me->audsrc->ReadBlockLPCM(Data::ByteArray((UInt8*)Packets[i].Header.Data, blkReadSize), af);
		}
		ResetEvent(Packets[i].Signal.hEvent);

		pPin->WriteData(&Packets[i].Header, &Packets[i].Signal);
		initSize += Packets[i].Header.DataUsed;
		i++;
	}
	hEventPool[cPackets] = (HANDLE)me->playEvt->GetHandle();

	Bool firstWait = true;
	while (!me->stopPlay)
	{
		i = cPackets;
		if (pPin->GetPosition(&pos) == 0)
		{
			me->clk->Start(audStartTime + (UInt32)((pos.PlayOffset + skipSize) * 8000 / af.bitRate));
		}

		i = WaitForMultipleObjects((UInt32)cPackets + 1, hEventPool, FALSE, 1000);
		if (i == WAIT_TIMEOUT)
		{
			if (firstWait)
			{
				pPin->SetState(KSSTATE_PAUSE);
				i = cPackets;
				while (i-- > 0)
				{
					SetEvent(hEventPool[i]);
				}

				if (pPin->GetPosition(&pos) == 0)
				{
					skipSize = initSize - pos.WriteOffset;
				}
				pPin->SetState(KSSTATE_STOP);
				pPin->SetState(KSSTATE_PAUSE);
				pPin->SetState(KSSTATE_RUN);
				firstWait = false;
			}
			else
			{
				break;
			}
		}
		else if (i == WAIT_FAILED)
		{
		}
		else
		{
			firstWait = false;
			i -= WAIT_OBJECT_0;
			if (i < cPackets)
			{
				Packets[i].Header.FrameExtent = (ULONG)buffLeng;
				Packets[i].Header.DataUsed = (ULONG)me->audsrc->ReadBlockLPCM(Data::ByteArray((UInt8*)Packets[i].Header.Data, blkReadSize), af);
				if (Packets[i].Header.DataUsed == 0)
				{
					buffEndCnt++;
					if (buffEndCnt >= cPackets)
						break;
				}
				else
				{
					buffEndCnt = 0;

					MemClear(&Packets[i].Signal, sizeof(Packets[i].Signal));
					if (Packets[i].Header.DataUsed < (ULONG)buffLeng)
					{
						MemClear(&((UInt8*)Packets[i].Header.Data)[Packets[i].Header.DataUsed], buffLeng - Packets[i].Header.DataUsed);
					}
					ResetEvent(Packets[i].Signal.hEvent = hEventPool[i]);

					pPin->WriteData(&Packets[i].Header, &Packets[i].Signal);
				}
			}
		}
	}

	me->audsrc->Stop();
	DEL_CLASS(evt);
    pPin->SetState(KSSTATE_PAUSE);
    pPin->SetState(KSSTATE_STOP);
	i = cPackets;
	while (i-- > 0)
	{
		CloseHandle(Packets[i].Signal.hEvent);
	}
	MemFree(pcmBuffer);
	MemFree(Packets);
	MemFree(hEventPool);

	me->playing = false;
	if (me->endHdlr)
	{
		me->endHdlr(me->endHdlrObj);
	}
	return 0;
}

UOSInt Media::KSRenderer::GetDeviceCount()
{
	UOSInt cnt = 0;
	HRESULT hr;
    GUID  aguidEnumCats[] = { STATIC_KSCATEGORY_AUDIO, STATIC_KSCATEGORY_RENDER };
	CKsEnumFilters* pEnumerator = new CKsEnumFilters(&hr);
	hr = pEnumerator->EnumFilters(
			eAudRen,            // create audio render filters ...
            aguidEnumCats,      // ... of these categories
            2,                  // There are 2 categories
            TRUE,               // While you're at it, enumerate the pins
            FALSE,              // ... but don't bother with nodes
            TRUE                // Instantiate the filters
        );

	if (hr)
	{
		return 0;
	}
	cnt = pEnumerator->m_listFilters.GetCount();
	delete pEnumerator;
	return cnt;
}

UTF8Char *Media::KSRenderer::GetDeviceName(UTF8Char *buff, UOSInt devIndex)
{
	return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("Device ")), devIndex);
}

OSInt Media::KSRenderer::GetDeviceId(const UTF8Char *devName)
{
	if (Text::StrStartsWith(devName, (const UTF8Char*)"Device "))
	{
		return Text::StrToInt32(devName + 7);
	}
	return 0;
}

Media::KSRenderer::KSRenderer(OSInt devId)
{
	this->pFilter = 0;
	this->pEnumerator = 0;
	this->pPin = 0;
	this->playing = false;
	this->threadInit = false;
	this->stopPlay = false;
	this->audsrc = 0;
	this->playEvt = 0;
	this->endHdlr = 0;
	this->buffTime = 0;

	HRESULT hr;
    GUID  aguidEnumCats[] = { STATIC_KSCATEGORY_AUDIO, STATIC_KSCATEGORY_RENDER };
	CKsEnumFilters* pEnumerator = new CKsEnumFilters(&hr);
	hr = pEnumerator->EnumFilters(
			eAudRen,            // create audio render filters ...
            aguidEnumCats,      // ... of these categories
            2,                  // There are 2 categories
            TRUE,               // While you're at it, enumerate the pins
            FALSE,              // ... but don't bother with nodes
            TRUE                // Instantiate the filters
        );

	if (hr)
	{
		return;
	}
	CKsFilter *pFilter;
	LISTPOS pos = pEnumerator->m_listFilters.GetHeadPosition();
	while (devId-- > 0)
	{
		pEnumerator->m_listFilters.GetNext(pos, &pFilter);
	}
	BOOL ret = pEnumerator->m_listFilters.GetAt(pos, &pFilter);
	if (ret)
	{
		this->pFilter = pFilter;
		this->pEnumerator = pEnumerator;
	}
	else
	{
		delete pEnumerator;
	}
}

Media::KSRenderer::~KSRenderer()
{
	this->BindAudio(0);
	if (this->pEnumerator)
	{
		CKsEnumFilters *pEnumerator = (CKsEnumFilters*)this->pEnumerator;
		delete pEnumerator;
		this->pEnumerator = 0;
	}
}

Bool Media::KSRenderer::IsError()
{
	return this->pFilter == 0;
}

Bool Media::KSRenderer::BindAudio(Media::IAudioSource *audsrc)
{
	CKsAudRenFilter *pFilter;
	CKsAudRenPin *pPin;
	pFilter = (CKsAudRenFilter*)this->pFilter;
	this->Stop();
	this->audsrc = 0;
	if (this->pPin)
	{
		pPin = (CKsAudRenPin *)this->pPin;
		pFilter->EnumeratePins();
		pPin->ClosePin();
		delete pPin;
		this->pPin = 0;
	}
	SDEL_CLASS(this->playEvt);
	if (audsrc == 0 || this->pFilter == 0)
		return false;
	Media::AudioFormat fmt;
	audsrc->GetFormat(fmt);

	if (fmt.formatId != 1)
	{
		return false;
	}

	WAVEFORMATEXTENSIBLE format;
	FillFormat(&format, &fmt);

	this->pPin = pFilter->CreateRenderPin(&format.Format, FALSE);
	if (this->pPin != 0)
	{
		this->audsrc = audsrc;
		NEW_CLASS(this->playEvt, Sync::Event(true));
		return true;
	}
	else
	{
		return false;
	}
}

void Media::KSRenderer::AudioInit(Media::RefClock *clk)
{
	if (this->playing)
		return;
	if (this->audsrc == 0)
		return;
	this->clk = clk;
}

void Media::KSRenderer::Start()
{
	if (playing)
		return;
	if (this->audsrc == 0)
		return;
	this->threadInit = false;
	this->stopPlay = false;
	Sync::ThreadUtil::Create(PlayThread, this);
	while (!this->threadInit)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

void Media::KSRenderer::Stop()
{
	this->stopPlay = true;
	if (!this->playing)
		return;
	this->playEvt->Set();
	if (this->audsrc)
	{
		this->audsrc->Stop();
	}
	while (playing)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Bool Media::KSRenderer::IsPlaying()
{
	return this->playing;
}

void Media::KSRenderer::SetEndNotify(EndNotifier endHdlr, AnyType endHdlrObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = endHdlrObj;
}

void Media::KSRenderer::SetBufferTime(UInt32 ms)
{
	this->buffTime = ms;
}

Int32 Media::KSRenderer::GetDeviceVolume()
{
	return 65535;
}

void Media::KSRenderer::SetDeviceVolume(Int32 volume)
{
}
