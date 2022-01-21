#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Console.h"
#include "Net/RTPAACHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

UInt8 Net::RTPAACHandler::GetRateIndex()
{
	switch (this->freq)
	{
	case 96000:
		return 0;
	case 88200:
		return 1;
	case 64000:
		return 2;
	case 48000:
		return 3;
	case 44100:
		return 4;
	case 32000:
		return 5;
	case 24000:
		return 6;
	case 22050:
		return 7;
	case 16000:
		return 8;
	case 2000:
		return 9;
	case 11025:
		return 10;
	case 8000:
		return 11;
	default:
		return 15;
	}
}

Net::RTPAACHandler::RTPAACHandler(Int32 payloadType, UInt32 freq, UInt32 nChannel)
{
	this->payloadType = payloadType;
	this->freq = freq;
	this->nChannel = nChannel;
	this->evt = 0;
	this->aacm = AACM_UNKNOWN;
	this->streamType = 0;
	this->profileId = 0;
	this->config = 0;
	this->buffSize = 0;
	this->buff = MemAlloc(UInt8, 1048576);
	NEW_CLASS(this->mut, Sync::Mutex());
	this->dataEvt = 0;
}

Net::RTPAACHandler::~RTPAACHandler()
{
	this->Stop();
	MemFree(this->buff);
	DEL_CLASS(this->mut);
}

void Net::RTPAACHandler::MediaDataReceived(UInt8 *buff, UOSInt dataSize, UInt32 seqNum, UInt32 ts)
{
	UInt32 headerSize = ReadMUInt16(&buff[0]);
	if (headerSize & 7)
	{
		headerSize = (headerSize >> 3) + 1;
	}
	else
	{
		headerSize = headerSize >> 3;
	}
	UOSInt sizeLeft = dataSize - headerSize;
	UOSInt ofst = headerSize + 2;
	UOSInt i = 0;
	UInt32 thisSize;

	Sync::MutexUsage mutUsage(this->mut);
	switch (this->aacm)
	{
	case AACM_AAC_HBR:
		while (i < headerSize)
		{
			thisSize = (UInt32)(ReadMUInt16(&buff[i + 2]) >> 3);
			if ((this->buffSize + thisSize + 7) > 1048576 || thisSize > sizeLeft)
			{
				break;
			}
			//ADTS Header
			this->buff[this->buffSize + 0] = 0xff;
			this->buff[this->buffSize + 1] = 0xf9;
			this->buff[this->buffSize + 2] = (1 << 6) // profile = 1 (AAC-LC)
										| (UInt8)(GetRateIndex() << 2)
										| (UInt8)(this->nChannel >> 2);
			this->buff[this->buffSize + 3] = (UInt8)((this->nChannel & 3) << 6)
										| (UInt8)(((thisSize + 7) & 0x1800) >> 11);
			this->buff[this->buffSize + 4] = ((thisSize + 7) & 0x7f8) >> 3;
			this->buff[this->buffSize + 5] = ((thisSize + 7) & 7) << 5;
			this->buff[this->buffSize + 5] |= 0x1f;
			this->buff[this->buffSize + 6] = 0xfc;
			this->buff[this->buffSize + 6] |= 0; // number_of_raw_data_blocks_in_frame
			this->buffSize += 7;
			MemCopyNO(&this->buff[this->buffSize], &buff[ofst], thisSize);
			this->buffSize += thisSize;
			ofst += thisSize;
			sizeLeft -= thisSize;
			i += 2;
		}
		break;
	case AACM_GENERIC:
	case AACM_AAC_LBR:
	case AACM_CELP_CBR:
	case AACM_CELP_VBR:
	case AACM_UNKNOWN:
	default:
		mutUsage.EndUse();
		return;
	}
	mutUsage.EndUse();

	if (this->evt)
	{
		this->evt->Set();
	}
	if (this->dataEvt)
	{
		this->dataEvt->Set();
	}
}

void Net::RTPAACHandler::SetFormat(const UTF8Char *fmtStr)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::PString sarr[2];
	UOSInt i;
	sptr = Text::StrConcat(sbuff, fmtStr);
	sarr[1].v = sbuff;
	sarr[1].len = (UOSInt)(sptr - sbuff);
	while (true)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1].v, sarr[1].len, ';');
		if (Text::StrStartsWithC(sarr[0].v, sarr[0].len, UTF8STRC("mode=")))
		{
			if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("mode=generic")))
			{
				this->aacm = AACM_GENERIC;
			}
			else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("mode=CELP-cbr")))
			{
				this->aacm = AACM_CELP_CBR;
			}
			else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("mode=CELP-vbr")))
			{
				this->aacm = AACM_CELP_VBR;
			}
			else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("mode=AAC-lbr")))
			{
				this->aacm = AACM_AAC_LBR;
			}
			else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("mode=AAC-hbr")))
			{
				this->aacm = AACM_AAC_HBR;
			}
		}
		else if (Text::StrStartsWithC(sarr[0].v, sarr[0].len, UTF8STRC("streamType=")))
		{
			this->streamType = Text::StrToInt32(&sarr[0].v[11]);
		}
		else if (Text::StrStartsWithC(sarr[0].v, sarr[0].len, UTF8STRC("profile-level-id=")))
		{
			this->profileId = Text::StrToInt32(&sarr[0].v[17]);
		}
		else if (Text::StrStartsWithC(sarr[0].v, sarr[0].len, UTF8STRC("config=")))
		{
			this->config = Text::StrHex2Int32C(&sarr[0].v[7]);
		}
		else
		{
			sarr[0] = 0;
		}
		
		if (i == 1)
		{
			break;
		}
	}
	////////////////////////////////////
}

Int32 Net::RTPAACHandler::GetPayloadType()
{
	return this->payloadType;
}

UTF8Char *Net::RTPAACHandler::GetSourceName(UTF8Char *buff)
{
	return Text::StrConcatC(buff, UTF8STRC("mpeg4-generic"));
}

Bool Net::RTPAACHandler::CanSeek()
{
	return false;
}

Int32 Net::RTPAACHandler::GetStreamTime()
{
	return -1;
}

UInt32 Net::RTPAACHandler::SeekToTime(UInt32 time)
{
	return 0;
}

Bool Net::RTPAACHandler::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

void Net::RTPAACHandler::GetFormat(Media::AudioFormat *format)
{
	format->Clear();
	format->formatId = 255;
	format->frequency = this->freq;
	format->nChannels = (UInt16)this->nChannel;
	format->bitpersample = 16;
	format->align = 1;
	format->intType = Media::AudioFormat::IT_NORMAL;
	format->extraSize = 2;
	format->extra = MemAlloc(UInt8, 2);
	WriteInt16(format->extra, this->config);
	format->bitRate = 128000;
}

Bool Net::RTPAACHandler::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->evt = evt;
	if (this->dataEvt)
	{
		return true;
	}
	NEW_CLASS(this->dataEvt, Sync::Event(true, (const UTF8Char*)"Net.RTPAACHandler.dataEvt"));
	return true;
}

void Net::RTPAACHandler::Stop()
{
	this->evt = 0;
	SDEL_CLASS(this->dataEvt);
}

UOSInt Net::RTPAACHandler::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	while (this->buffSize == 0 && this->dataEvt)
	{
		this->dataEvt->Wait(1000);
	}
	Sync::MutexUsage mutUsage(this->mut);
	if (this->buffSize <= 0)
	{
		mutUsage.EndUse();
		return 0;
	}
	if (this->buffSize < blkSize)
	{
		blkSize = this->buffSize;
	}
	MemCopyNO(buff, this->buff, blkSize);
	MemCopyO(this->buff, &this->buff[blkSize], this->buffSize - blkSize);
	this->buffSize -= blkSize;
	mutUsage.EndUse();

	return blkSize;
}

UOSInt Net::RTPAACHandler::GetMinBlockSize()
{
	return 1;
}

UInt32 Net::RTPAACHandler::GetCurrTime()
{
	return 0;
}

Bool Net::RTPAACHandler::IsEnd()
{
	return true;
}
