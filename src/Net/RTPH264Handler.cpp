#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Data/ByteTool.h"
#include "IO/Console.h"
#include "Media/H264Parser.h"
#include "Net/RTPH264Handler.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Net::RTPH264Handler::RTPH264Handler(Int32 payloadType)
{
	this->payloadType = payloadType;
	this->cb = 0;
	this->cbData = 0;
	this->packetMode = -1;
	this->frameNum = 0;
	this->lastSeq = (UInt32)-1;
	this->missSeq = true;
	this->sps = 0;
	this->pps = 0;
	this->firstFrame = false;

	this->frameInfo.fourcc = *(UInt32*)"H264";
	this->frameInfo.dispSize = Math::Size2D<UOSInt>(0, 0);
	this->frameInfo.storeSize = Math::Size2D<UOSInt>(0, 0);
	this->frameInfo.storeBPP = 0;
	this->frameInfo.pf = Media::PF_UNKNOWN;
	this->frameInfo.byteSize = 0;
	this->frameInfo.par2 = 1;
	this->frameInfo.hdpi = 96;
	this->frameInfo.ftype = Media::FT_NON_INTERLACE;
	this->frameInfo.atype = Media::AT_IGNORE_ALPHA;
	this->frameInfo.color.SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
	this->frameInfo.yuvType = Media::ColorProfile::YUVT_BT601;
	this->frameInfo.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->frameInfo.rotateType = Media::RotateType::None;
}

Net::RTPH264Handler::~RTPH264Handler()
{
	if (this->sps)
	{
		MemFree(this->sps);
	}
	if (this->pps)
	{
		MemFree(this->pps);
	}
}


void Net::RTPH264Handler::MediaDataReceived(UInt8 *buff, UOSInt dataSize, UInt32 seqNum, UInt32 ts)
{
	UTF8Char sbuff[32];
	Sync::MutexUsage mutUsage(this->mut);
	Text::StrConcatC(Text::StrInt64(Text::StrConcatC(sbuff, UTF8STRC("ts: ")), ts), UTF8STRC("\r\n"));
	IO::Console::PrintStrO(sbuff);

	if (((lastSeq + 1) & 65535) != seqNum)
	{
		missSeq = true;
	}
	lastSeq = seqNum;
	UOSInt frameSize;
	UnsafeArray<UInt8> frameBuff;

	UInt8 tmpBuff[5];
	UOSInt i = 0;
	switch (buff[0] & 0x1f)
	{
	case 1:
		missSeq = true;
		this->mstm.Clear();
		WriteMInt32(tmpBuff, 1);
		this->mstm.Write(Data::ByteArrayR(tmpBuff, 4));
		this->mstm.Write(Data::ByteArrayR(buff, dataSize));
		frameBuff = this->mstm.GetBuff(frameSize);
		if (this->cb)
		{
			this->cb(ts / 90, this->frameNum++, &frameBuff, frameSize, Media::VideoSource::FS_P, this->cbData, Media::FT_NON_INTERLACE, (Media::VideoSource::FrameFlag)(this->firstFrame?Media::VideoSource::FF_DISCONTTIME:0), Media::YCOFST_C_CENTER_LEFT);
			this->firstFrame = false;
		}
		break;
	case 5:
		missSeq = true;
		this->mstm.Clear();
		if (this->sps)
		{
			this->mstm.Write(Data::ByteArrayR(this->sps, this->spsSize));
		}
		if (this->pps)
		{
			this->mstm.Write(Data::ByteArrayR(this->pps, this->ppsSize));
		}
		WriteMInt32(tmpBuff, 1);
		this->mstm.Write(Data::ByteArrayR(tmpBuff, 4));
		this->mstm.Write(Data::ByteArrayR(buff, dataSize));
		frameBuff = this->mstm.GetBuff(frameSize);
		if (this->cb)
		{
			this->cb(ts / 90, this->frameNum++, &frameBuff, frameSize, Media::VideoSource::FS_I, this->cbData, Media::FT_NON_INTERLACE, Media::VideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
		}
		break;
	case 24: //STAP-A
		this->mstm.Clear();
		WriteMInt32(tmpBuff, 1);
		i = 1;
		while (i < dataSize - 2)
		{
			frameSize = ReadMUInt16(&buff[i]);
			switch (buff[i + 2] & 0x1f)
			{
			case 1:
				this->mstm.Write(Data::ByteArrayR(tmpBuff, 4));
				this->mstm.Write(Data::ByteArrayR(&buff[i + 2], frameSize));
				this->isKey = false;
				break;
			case 5:
				if (this->sps)
				{
					this->mstm.Write(Data::ByteArrayR(this->sps, this->spsSize));
				}
				if (this->pps)
				{
					this->mstm.Write(Data::ByteArrayR(this->pps, this->ppsSize));
				}
				this->mstm.Write(Data::ByteArrayR(tmpBuff, 4));
				this->mstm.Write(Data::ByteArrayR(&buff[i + 2], frameSize));
				this->isKey = true;
				break;
			case 6:
				break;
			default:
				i = i + 1;
				i = i - 1;
				break;
			}
			i += frameSize + 2;
		}
		frameBuff = this->mstm.GetBuff(frameSize);
		if (this->cb)
		{
			this->cb(ts / 90, this->frameNum++, &frameBuff, frameSize, this->isKey?Media::VideoSource::FS_I:Media::VideoSource::FS_P, this->cbData, Media::FT_NON_INTERLACE, Media::VideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
		}
		break;
	case 28: //FU-A
		if (buff[1] & 0x80) //start
		{
			missSeq = false;
			this->mstm.Clear();
			if ((buff[1] & 0x1f) == 5)
			{
				this->isKey = true;
				if (this->sps)
				{
					this->mstm.Write(Data::ByteArrayR(this->sps, this->spsSize));
				}
				if (this->pps)
				{
					this->mstm.Write(Data::ByteArrayR(this->pps, this->ppsSize));
				}
			}
			else if ((buff[1] & 0x1f) == 1)
			{
				this->isKey = false;
			}
			else
			{
				this->isKey = false;
			}

			WriteMInt32(tmpBuff, 1);
			tmpBuff[4] = (buff[0] & 0xe0) | (buff[1] & 0x1f);
			this->mstm.Write(Data::ByteArrayR(tmpBuff, 5));
		}
		this->mstm.Write(Data::ByteArrayR(&buff[2], dataSize - 2));
		if (buff[1] & 0x40) //end
		{
			if (!missSeq)
			{
				missSeq = true;

				frameBuff = this->mstm.GetBuff(frameSize);
				if (this->cb)
				{
					this->cb(ts / 90, this->frameNum++, &frameBuff, frameSize, this->isKey?(Media::VideoSource::FS_I):(Media::VideoSource::FS_P), this->cbData, Media::FT_NON_INTERLACE, (this->isKey && this->firstFrame)?(Media::VideoSource::FF_DISCONTTIME):Media::VideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
					if (this->isKey)
					{
						this->firstFrame = false;
					}
				}
			}
			else
			{
				IO::Console::PrintStrO((const UTF8Char*)"Dropped frame\n");
			}
		}
		break;
	case 29:
		if (buff[1] & 0x80)
		{
			missSeq = false;
			this->mstm.Clear();
			if ((buff[1] & 0x1f) == 5)
			{
				this->isKey = true;
				if (this->sps)
				{
					this->mstm.Write(Data::ByteArrayR(this->sps, this->spsSize));
				}
				if (this->pps)
				{
					this->mstm.Write(Data::ByteArrayR(this->pps, this->ppsSize));
				}
			}
			else
			{
				this->isKey = false;
			}
			WriteMInt32(tmpBuff, 1);
			tmpBuff[4] = (buff[0] & 0xe0) | (buff[1] & 0x1f);
			this->mstm.Write(Data::ByteArrayR(tmpBuff, 5));
		}
		this->mstm.Write(Data::ByteArrayR(&buff[4], dataSize - 4));
		if (buff[1] & 0x40)
		{
			if (!missSeq)
			{
				missSeq = true;

				frameBuff = this->mstm.GetBuff(frameSize);
				if (this->cb)
				{
					this->cb(ts / 90, this->frameNum++, &frameBuff, frameSize, this->isKey?(Media::VideoSource::FS_I):(Media::VideoSource::FS_P), this->cbData, Media::FT_NON_INTERLACE, (this->isKey && this->firstFrame)?Media::VideoSource::FF_DISCONTTIME:Media::VideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
					if (this->isKey)
						this->firstFrame = false;
				}
			}
			else
			{
				IO::Console::PrintStrO((const UTF8Char*)"Dropped frame\n");
			}
		}
		break;
	default:
		i = 1;
		break;
	}
	mutUsage.EndUse();
}

void Net::RTPH264Handler::SetFormat(UnsafeArray<const UTF8Char> fmtStr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sarr[2];
	UnsafeArray<UTF8Char> sarr2[2];
	UInt8 buff[256];
	UOSInt splitCnt;
	Text::StrConcat(sbuff, fmtStr);
	sarr[1] = sbuff;
	while (true)
	{
		splitCnt = Text::StrSplit(sarr, 2, sarr[1], ';');

		if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"packetization-mode="))
		{
			this->packetMode = Text::StrToInt32(&sarr[0][19]);
		}
		else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"profile-level-id="))
		{
		}
		else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"sprop-parameter-sets="))
		{
			if (Text::StrSplit(sarr2, 2, &sarr[0][21], ',') == 2)
			{
				Crypto::Encrypt::Base64 b64;

				UOSInt txtSize = (UOSInt)(Text::StrConcat((UTF8Char*)&buff[4], sarr2[0]) - &buff[4]);
				WriteMInt32(buff, 1);
				txtSize = b64.Decrypt(&buff[4], txtSize - 1, &buff[4]);

				if (this->sps)
				{
					MemFree(this->sps);
				}
				this->spsSize = txtSize + 4;
				this->sps = MemAlloc(UInt8, spsSize);
				MemCopyNO(this->sps, buff, this->spsSize);
				
				Media::H264Parser::GetFrameInfo(buff, this->spsSize, this->frameInfo, 0);

				txtSize = (UOSInt)(Text::StrConcat(&buff[4], sarr2[1]) - &buff[4]);
				WriteMInt32(buff, 1);
				txtSize = b64.Decrypt(&buff[4], txtSize - 1, &buff[4]);

				if (this->pps)
				{
					MemFree(this->pps);
				}
				this->ppsSize = txtSize + 4;
				this->pps = MemAlloc(UInt8, spsSize);
				MemCopyNO(this->pps, buff, this->ppsSize);
			}
		}
		
		if (splitCnt == 1)
			break;
	}
	///////////////////////////////
}

Int32 Net::RTPH264Handler::GetPayloadType()
{
	return this->payloadType;
}

UnsafeArrayOpt<UTF8Char> Net::RTPH264Handler::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return Text::StrConcatC(buff, UTF8STRC("H.264 over RTP"));
}

Text::CStringNN Net::RTPH264Handler::GetFilterName()
{
	return CSTR("RTPH264Handler");
}

Bool Net::RTPH264Handler::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	if (this->frameInfo.dispSize.x == 0 || this->frameInfo.dispSize.y == 0)
		return false;
	info->Set(this->frameInfo);
	frameRateNorm.Set(30);
	frameRateDenorm.Set(1);
	maxFrameSize.Set(90000);
	return true;
}

Bool Net::RTPH264Handler::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	this->cbData = userData;
	this->cb = cb;
	this->fcCb = fcCb;
	this->firstFrame = true;
	return true;
}

Bool Net::RTPH264Handler::Start()
{
	this->firstFrame = true;
	return true;
}

void Net::RTPH264Handler::Stop()
{
	this->cb = 0;
	this->cbData = 0;
}

Bool Net::RTPH264Handler::IsRunning()
{
	return this->cb != 0;
}


Data::Duration Net::RTPH264Handler::GetStreamTime()
{
	return Data::Duration::Infinity();
}

Bool Net::RTPH264Handler::CanSeek()
{
	return false;
}

Data::Duration Net::RTPH264Handler::SeekToTime(Data::Duration time)
{
	return 0;
}

UOSInt Net::RTPH264Handler::GetDataSeekCount()
{
	return 0;
}

Bool Net::RTPH264Handler::HasFrameCount()
{
	return false;
}

UOSInt Net::RTPH264Handler::GetFrameCount()
{
	return 0;
}

Data::Duration Net::RTPH264Handler::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Net::RTPH264Handler::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
}

Bool Net::RTPH264Handler::IsRealTimeSrc()
{
	return true;
}

Bool Net::RTPH264Handler::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	return false;
}

UOSInt Net::RTPH264Handler::ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype)
{
	return 0;
}
