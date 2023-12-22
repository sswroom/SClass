#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Media/M2VStreamSource.h"
#include "Media/MPEGVideoParser.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#define PLAYBUFFSIZE 30

void Media::M2VStreamSource::SubmitFrame(UOSInt frameSize, UOSInt frameStart, UOSInt pictureStart)
{
	UOSInt fieldCnt = 2;
	Data::Duration fieldAdd = 0;
	UInt32 startCode;
	Media::MPEGVideoParser::MPEGFrameProp prop;
	this->totalFrameCnt++;
	this->totalFrameSize += frameSize;
	pictureStart = frameStart;
	while (pictureStart < frameSize + frameStart - 3)
	{
		startCode = ReadMUInt32(&this->frameBuff[pictureStart]);
		if (startCode == 0)
		{
			pictureStart += 2;
		}
		else if (startCode == 1)
		{
			pictureStart += 1;
		}
		else if (startCode == 0x000001b2) //user_data_start_code
		{
			pictureStart += 4;
			while (this->frameBuff[pictureStart] != 0 || this->frameBuff[pictureStart + 1] != 0 || this->frameBuff[pictureStart + 2] != 1)
			{
				pictureStart += 1;
			}
		}
		else if (startCode == 0x000001b3) //sequence_header_code
		{
			Bool load_intra_quantiser_matrix = (this->frameBuff[pictureStart + 11] & 2) != 0;
			if (load_intra_quantiser_matrix)
			{
				pictureStart += 75;
			}
			else
			{
				pictureStart += 11;
			}
			Bool load_non_intra_quantiser_matrix = (this->frameBuff[pictureStart] & 1) != 0;
			if (load_non_intra_quantiser_matrix)
			{
				pictureStart += 65;
			}
			else
			{
				pictureStart++;
			}

		}
		else if (startCode == 0x000001b5) //extension_start_code
		{
			if ((this->frameBuff[pictureStart + 4] & 0xf0) == 0x10) //Sequence extension
			{
				pictureStart += 10;
			}
			else if ((this->frameBuff[pictureStart + 4] & 0xf0) == 0x20) // Sequence Display Extension
			{
				if ((this->frameBuff[pictureStart + 4] & 1) != 0)
				{
					pictureStart += 12;
				}
				else
				{
					pictureStart += 9;
				}
			}
			else
			{
				break;
			}
		}
		else if (startCode == 0x000001b8) //group_start_code
		{
			pictureStart += 8;
		}
		else if (startCode == 0x00000100) //picture_start_code
		{
			break;
		}
		else
		{
			break;
		}
	}
	Bool ret;
	ret = Media::MPEGVideoParser::GetFrameProp(&this->frameBuff[pictureStart], frameSize, &prop);
	if (ret)
	{
		if (prop.rff)
		{
			fieldCnt = 3;
			fieldAdd = Data::Duration::FromRatioU64(this->frameRateDenorm, this->frameRateNorm * 2);
		}
		else if (prop.pictureStruct == Media::MPEGVideoParser::PS_FRAME)
		{
			fieldCnt = 2;
		}
		else
		{
			fieldCnt = 1;
		}
	}
	if (this->finfoMode)
	{
		Media::IVideoSource::FrameStruct fs;
		Media::FrameType ft;
		if (ret)
		{
			if (prop.pictureCodingType == 'I')
				fs = Media::IVideoSource::FS_I;
			else if (prop.pictureCodingType == 'B')
				fs = Media::IVideoSource::FS_B;
			else
				fs = Media::IVideoSource::FS_P;
			if (prop.progressive)
				ft = Media::FT_NON_INTERLACE;
			else if (prop.pictureStruct == Media::MPEGVideoParser::PS_TOPFIELD)
				ft = Media::FT_MERGED_TF;
			else if (prop.pictureStruct == Media::MPEGVideoParser::PS_BOTTOMFIELD)
				ft = Media::FT_MERGED_BF;
			else if (prop.tff)
				ft = Media::FT_INTERLACED_TFF;
			else
				ft = Media::FT_INTERLACED_BFF;
		}
		else
		{
			fs = Media::IVideoSource::FS_P;
			ft = Media::FT_NON_INTERLACE;
		}
		this->finfoCb(this->thisFrameTime + fieldAdd, this->frameNum, frameSize, fs, ft, this->finfoData, Media::YCOFST_C_CENTER_LEFT);
		this->frameNum++;
	}
	else if (this->frameCb)
	{
		Manage::HiResClock clk;
		while (true)
		{
			OSInt nextIndex = this->playBuffEnd + 1;
			if (nextIndex >= PLAYBUFFSIZE)
			{
				nextIndex -= PLAYBUFFSIZE;
			}

			if (nextIndex == this->playBuffStart)
			{
				if (this->playToStop || (!this->playing && clk.GetTimeDiff() >= 3))
				{
#ifdef _DEBUG
					NotNullPtr<Sync::Mutex> debugMut;
					if (this->debugLog && debugMut.Set(this->debugMut))
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Add to Play buff "));
						sb.AppendDur((this->thisFrameTime + fieldAdd));
						sb.AppendC(UTF8STRC(" skipped"));
						Sync::MutexUsage debugMutUsage(debugMut);
						this->debugLog->WriteLineC(sb.ToString(), sb.GetLength());
					}
#endif
					break;
				}
				Sync::SimpleThread::Sleep(10);
			}
			else
			{
#ifdef _DEBUG
				NotNullPtr<Sync::Mutex> debugMut;
				if (this->debugLog && debugMut.Set(this->debugMut))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Add to Play buff "));
					sb.AppendDur((this->thisFrameTime + fieldAdd));
					Sync::MutexUsage debugMutUsage(debugMut);
					this->debugLog->WriteLineC(sb.ToString(), sb.GetLength());
				}
#endif
				Sync::MutexUsage mutUsage(this->playMut);
				this->playBuff[this->playBuffEnd].frame = MemAlloc(UInt8, frameSize);
				this->playBuff[this->playBuffEnd].frameNum = this->frameNum;
				this->playBuff[this->playBuffEnd].frameSize = frameSize;
				this->playBuff[this->playBuffEnd].frameTime = this->thisFrameTime + fieldAdd;
				this->playBuff[this->playBuffEnd].pictureStart = (OSInt)(pictureStart - frameStart);
				MemCopyNO(this->playBuff[this->playBuffEnd].frame, &this->frameBuff[frameStart], frameSize);

				this->playBuffEnd = nextIndex;
				this->playEvt.Set();
				mutUsage.EndUse();
				break;
			}
		}
		this->frameNum++;
	}
	this->thisFrameTime = this->syncFrameTime + Data::Duration::FromRatioU64(this->syncFieldCnt * (UInt64)this->frameRateDenorm, this->frameRateNorm * 2);
	this->syncFieldCnt += fieldCnt;
}

void Media::M2VStreamSource::ClearPlayBuff()
{
	Sync::MutexUsage mutUsage(this->playMut);
	while (this->playBuffStart != this->playBuffEnd)
	{
		MemFree(this->playBuff[this->playBuffStart].frame);

		this->playBuffStart++;
		if (this->playBuffStart >= PLAYBUFFSIZE)
		{
			this->playBuffStart -= PLAYBUFFSIZE;
		}
	}
	mutUsage.EndUse();
}

UInt32 __stdcall Media::M2VStreamSource::PlayThread(void *userObj)
{
	Media::M2VStreamSource *me = (Media::M2VStreamSource*)userObj;
	me->playInit = true;
	me->playing = true;
	while (!me->playToStop)
	{
		Sync::MutexUsage mutUsage(me->playMut);
		if (me->playBuffStart == me->playBuffEnd)
		{
			mutUsage.EndUse();
			if (me->playEOF)
			{
				break;
			}
			me->playEvt.Wait(1000);
		}
		else
		{
			Media::MPEGVideoParser::MPEGFrameProp prop;
			Bool ret;
			Int32 startCode;
			UOSInt pictureStart = 0;
			while (true)
			{
				startCode = ReadMInt32(&me->playBuff[me->playBuffStart].frame[pictureStart]);
				if (startCode == 0x00000100)
					break;

				if (startCode == 0)
				{
					pictureStart += 2;
				}
				else if (startCode == 1)
				{
					pictureStart += 1;
				}
				else if (startCode == 0x000001b2)
				{
					pictureStart += 4;
					while (me->playBuff[me->playBuffStart].frame[pictureStart] != 0 || me->playBuff[me->playBuffStart].frame[pictureStart + 1] != 0 || me->playBuff[me->playBuffStart].frame[pictureStart + 2] != 1)
					{
						pictureStart += 1;
					}
				}
				else if (startCode == 0x000001b3)
				{
					Media::FrameInfo info;
					UInt32 norm;
					UInt32 denorm;
					UInt64 bitRate;

					if (Media::MPEGVideoParser::GetFrameInfo(&me->playBuff[me->playBuffStart].frame[pictureStart], me->playBuff[me->playBuffStart].frameSize, info, norm, denorm, &bitRate, true))
					{
						if (info.par2 != me->par)
						{
							me->par = info.par2;
							if (me->fcCb)
							{
								me->fcCb(Media::IVideoSource::FC_PAR, me->frameCbData);
							}
						}
						me->bitRate = bitRate;
					}

					Bool load_intra_quantiser_matrix = (me->playBuff[me->playBuffStart].frame[pictureStart + 11] & 2) != 0;
					if (load_intra_quantiser_matrix)
					{
						pictureStart += 75;
					}
					else
					{
						pictureStart += 11;
					}
					Bool load_non_intra_quantiser_matrix = (me->playBuff[me->playBuffStart].frame[pictureStart] & 1) != 0;
					if (load_non_intra_quantiser_matrix)
					{
						pictureStart += 65;
					}
					else
					{
						pictureStart++;
					}

				}
				else if (startCode == 0x000001b5)
				{
					if ((me->playBuff[me->playBuffStart].frame[pictureStart + 4] & 0xf0) == 0x10) //Sequence extension
					{
						pictureStart += 10;
					}
					else if ((me->playBuff[me->playBuffStart].frame[pictureStart + 4] & 0xf0) == 0x20) // Sequence Display Extension
					{
						if ((me->playBuff[me->playBuffStart].frame[pictureStart + 4] & 1) != 0)
						{
							pictureStart += 12;
						}
						else
						{
							pictureStart += 9;
						}
					}
					else
					{
						break;
					}
				}
				else if (startCode == 0x000001b8) //gop start
				{
					pictureStart += 8;
				}
				else
				{
					break;
				}
			}
			ret = Media::MPEGVideoParser::GetFrameProp(&me->playBuff[me->playBuffStart].frame[pictureStart], me->playBuff[me->playBuffStart].frameSize - pictureStart, &prop);
			Media::IVideoSource::FrameStruct fs;
			Media::FrameType ft;
			if (ret)
			{
				if (prop.pictureCodingType == 'I')
					fs = Media::IVideoSource::FS_I;
				else if (prop.pictureCodingType == 'B')
					fs = Media::IVideoSource::FS_B;
				else
					fs = Media::IVideoSource::FS_P;
				if (prop.progressive)
					ft = Media::FT_NON_INTERLACE;
				else if (prop.pictureStruct == Media::MPEGVideoParser::PS_TOPFIELD)
					ft = Media::FT_MERGED_TF;
				else if (prop.pictureStruct == Media::MPEGVideoParser::PS_BOTTOMFIELD)
					ft = Media::FT_MERGED_BF;
				else if (prop.tff)
					ft = Media::FT_INTERLACED_TFF;
				else
					ft = Media::FT_INTERLACED_BFF;
			}
			else
			{
				fs = Media::IVideoSource::FS_P;
				ft = Media::FT_NON_INTERLACE;
			}

			UInt8 *frameBuff = me->playBuff[me->playBuffStart].frame;
			UInt32 frameSize = (UInt32)me->playBuff[me->playBuffStart].frameSize;
			UInt32 frameNum = (UInt32)me->playBuff[me->playBuffStart].frameNum;
			Data::Duration frameTime = me->playBuff[me->playBuffStart].frameTime;

			me->playBuffStart++;
			if (me->playBuffStart >= PLAYBUFFSIZE)
				me->playBuffStart -= PLAYBUFFSIZE;
			mutUsage.EndUse();

#ifdef _DEBUG
			NotNullPtr<Sync::Mutex> debugMut;
			if (me->debugLog && debugMut.Set(me->debugMut))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Output frame "));
				sb.AppendDur(frameTime);
				Sync::MutexUsage debugMutUsage(debugMut);
				me->debugLog->WriteLineC(sb.ToString(), sb.GetLength());
			}
#endif
			me->frameCb(frameTime, frameNum, &frameBuff, frameSize, fs, me->frameCbData, ft, (frameNum == 0)?Media::IVideoSource::FF_DISCONTTIME:Media::IVideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
			MemFree(frameBuff);

		}
	}
	me->ClearPlayBuff();
	me->playing = false;
	if (me->playEOF)
	{
		if (me->fcCb)
		{
			me->fcCb(Media::IVideoSource::FC_ENDPLAY, me->frameCbData);
		}
	}
	return 1001;
}

Media::M2VStreamSource::M2VStreamSource(NotNullPtr<Media::IStreamControl> pbc)
{
	this->info.Clear();
	this->pbc = pbc;
	this->frameCb = 0;
	this->frameCbData = 0;
	this->bitRate = 1000;
	this->finfoMode = false;
	this->info.fourcc = (UInt32)-1;
	this->info.dispSize = Math::Size2D<UOSInt>(0, 0);
	this->info.storeSize = Math::Size2D<UOSInt>(0, 0);
	this->frameRateNorm = 0;
	this->frameRateDenorm = 0;
	this->maxFrameSize = 10485760;
	this->frameBuff = MemAlloc(UInt8, this->maxFrameSize);
	this->frameBuffSize = 0;
	this->firstFrame = true;
	this->frameStart = (UOSInt)-1;
	this->frameNum = 0;
	this->totalFrameCnt = 0;
	this->totalFrameSize = 0;
	this->debugLog = 0;
	this->debugFS = 0;
	this->debugMut = 0;

#ifdef _DEBUG
	NotNullPtr<IO::FileStream> debugFS;
	NEW_CLASS(this->debugMut, Sync::Mutex());
	NEW_CLASSNN(debugFS, IO::FileStream(CSTR("M2VDebug.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	this->debugFS = debugFS.Ptr();
	NEW_CLASS(this->debugLog, Text::UTF8Writer(debugFS));
#endif

	this->playing = false;
	this->playToStop = false;
	playBuff = MemAlloc(FrameBuff, PLAYBUFFSIZE);
	playBuffStart = 0;
	playBuffEnd = 0;
}

Media::M2VStreamSource::~M2VStreamSource()
{
	this->Stop();

	this->ClearPlayBuff();
	MemFree(this->frameBuff);
	MemFree(playBuff);
#ifdef _DEBUG
	SDEL_CLASS(this->debugLog);
	SDEL_CLASS(this->debugFS);
	SDEL_CLASS(this->debugMut);
#endif
}

UTF8Char *Media::M2VStreamSource::GetSourceName(UTF8Char *buff)
{
	return this->pbc->GetMediaName(buff);
}

Text::CStringNN Media::M2VStreamSource::GetFilterName()
{
	return CSTR("M2VStreamSource");
}

Bool Media::M2VStreamSource::GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	info->Set(this->info);
	frameRateNorm.Set(this->frameRateNorm);
	frameRateDenorm.Set(this->frameRateDenorm);
	maxFrameSize.Set(this->maxFrameSize);
	info->par2 = this->par;
	return true;
}

Bool Media::M2VStreamSource::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData)
{
	this->frameCb = cb;
	this->fcCb = fcCb;
	if (userData == 0)
	{
		userData = 0;
	}
	this->frameCbData = userData;
	return true;
}

Bool Media::M2VStreamSource::Start()
{
	if (this->playing)
		return false;

#ifdef _DEBUG
	NotNullPtr<Sync::Mutex> debugMut;
	if (this->debugLog && debugMut.Set(this->debugMut))
	{
		Sync::MutexUsage debugMutUsage(debugMut);
		this->debugLog->WriteLineC(UTF8STRC("Start"));
	}
#endif
	Sync::MutexUsage mutUsage(this->pbcMut);
	this->playToStop = false;
	this->playInit = false;
	this->playEOF = false;
	Bool started = this->pbc->StartVideo();
	Sync::ThreadUtil::Create(PlayThread, this);
	mutUsage.EndUse();
	while (!this->playInit)
	{
		Sync::SimpleThread::Sleep(10);
	}
	return started;
}

void Media::M2VStreamSource::Stop()
{
	if (this->playing)
	{
		this->playToStop = true;
		this->playEvt.Set();
		while (this->playing)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	Sync::MutexUsage mutUsage(this->pbcMut);
	this->pbc->StopVideo();
	this->playToStop = false;
	this->frameCb = 0;
	this->fcCb = 0;
	this->frameCbData = 0;
	mutUsage.EndUse();
	this->ClearFrameBuff();
}

Bool Media::M2VStreamSource::IsRunning()
{
	return this->pbc->IsRunning() || this->playing;
}

Data::Duration Media::M2VStreamSource::GetStreamTime()
{
	return this->pbc->GetStreamTime();
}

Bool Media::M2VStreamSource::CanSeek()
{
	return this->pbc->CanSeek();
}

Data::Duration Media::M2VStreamSource::SeekToTime(Data::Duration time)
{
	this->frameBuffSize = 0;
	return this->pbc->SeekToTime(time);
}

Bool Media::M2VStreamSource::IsRealTimeSrc()
{
	return this->pbc->IsRealTimeSrc();
}

Bool Media::M2VStreamSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	///////////////////////////////////////////
	return false;
}


UOSInt Media::M2VStreamSource::GetDataSeekCount()
{
	return this->pbc->GetDataSeekCount();
}

Bool Media::M2VStreamSource::HasFrameCount()
{
	return false;
}

UOSInt Media::M2VStreamSource::GetFrameCount()
{
	return 0;
}

Data::Duration Media::M2VStreamSource::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::M2VStreamSource::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	this->finfoMode = true;
	this->finfoCb = cb;
	this->finfoData = userData;
	this->SeekToTime(0);
	this->pbc->StartVideo();
	while (this->pbc->IsRunning())
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->finfoMode = false;
}

UOSInt Media::M2VStreamSource::GetFrameSize(UOSInt frameIndex)
{
	return 0;
}

UOSInt Media::M2VStreamSource::ReadFrame(UOSInt frameIndex, UInt8 *buff)
{
	return 0;
}

UOSInt Media::M2VStreamSource::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	return 0;
}

void Media::M2VStreamSource::DetectStreamInfo(UInt8 *header, UOSInt headerSize)
{
	UInt64 bitRate;
	Media::MPEGVideoParser::GetFrameInfo(header, headerSize, this->info, this->frameRateNorm, this->frameRateDenorm, &bitRate, false);
	this->par = this->info.par2;
	this->bitRate = bitRate;
}

void Media::M2VStreamSource::ClearFrameBuff()
{
	this->firstFrame = true;
	this->frameBuffSize = 0;
	this->frameNum = 0;
	this->writeCnt = 0;
	this->ClearPlayBuff();
}

void Media::M2VStreamSource::SetStreamTime(Data::Duration time)
{
#ifdef _DEBUG
	NotNullPtr<Sync::Mutex> debugMut;
	if (this->debugLog && debugMut.Set(this->debugMut))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Set Stream Time "));
		sb.AppendDur(time);
		Sync::MutexUsage debugMutUsage(debugMut);
		this->debugLog->WriteLineC(sb.ToString(), sb.GetLength());
	}
#endif
	this->syncFrameTime = time;
	this->syncFieldCnt = 0;
}

void Media::M2VStreamSource::WriteFrameStream(UInt8 *buff, UOSInt buffSize)
{
	this->writeCnt++;
	UOSInt lastFrameSize = this->frameBuffSize;
	UOSInt i;
	Int32 hdr;
	Int32 gopHdr;
	Int32 pictureHdr;
	UOSInt j;
	WriteMInt32((UInt8*)&hdr, 0x000001b3);
	WriteMInt32((UInt8*)&gopHdr, 0x000001b8);
	WriteMInt32((UInt8*)&pictureHdr, 0x00000100);

#ifdef _DEBUG
	NotNullPtr<Sync::Mutex> debugMut;
	if (this->debugLog && debugMut.Set(this->debugMut))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("WriteFrameStream "));
		sb.AppendOSInt(buffSize);
		Sync::MutexUsage debugMutUsage(debugMut);
		this->debugLog->WriteLineC(sb.ToString(), sb.GetLength());
	}
#endif

	if (this->frameBuffSize == 0)
	{
		this->thisFrameTime = this->syncFrameTime;
		this->syncFieldCnt += 2;
	}
	while (this->frameBuffSize + buffSize > this->maxFrameSize)
	{
		Sync::SimpleThread::Sleep(10);
		if (!this->pbc->IsRunning())
		{
			return;
		}
	}
	MemCopyNO(&this->frameBuff[this->frameBuffSize], buff, buffSize);
	this->frameBuffSize += buffSize;

	i = 0;
	if (this->firstFrame)
	{
		Bool found = false;
		while (i < this->frameBuffSize - 3)
		{
			if (*(Int32*)&this->frameBuff[i] == hdr || *(Int32*)&this->frameBuff[i] == gopHdr)
			{
				found = true;
				break;
			}
			else if (*(Int32*)&this->frameBuff[i] == pictureHdr)
			{
				if (i > this->frameBuffSize - 6)
					break;
				j = (this->frameBuff[i + 5] & 0x38) >> 3;
				if (j != 1)
				{
					i++;
				}
				else
				{
					found = true;
					break;
				}
			}
			i++;
		}
		if (i > 0)
		{
			MemCopyO(this->frameBuff, &this->frameBuff[i], this->frameBuffSize - i);
			this->frameBuffSize -= i;
		}
		if (!found)
		{
			return;
		}
		this->firstFrame = false;
		this->frameStart = (UOSInt)-1;
	}

	j = 0;
	i = lastFrameSize - 3;
	if ((OSInt)i < 0)
		i = 0;
	
	while (i < this->frameBuffSize - 3)
	{
		if (*(Int32*)&this->frameBuff[i] == hdr || *(Int32*)&this->frameBuff[i] == gopHdr || *(Int32*)&this->frameBuff[i] == pictureHdr)
		{
			if (this->frameStart >= j)
			{
				this->SubmitFrame(i - j, j, this->frameStart);
				j = i;
				this->frameStart = i - 1;
			}
			if (*(Int32*)&this->frameBuff[i] == pictureHdr)
			{
				this->frameStart = i;
			}
		}
		i++;
	}
	if (j > 0 && this->frameBuffSize > j)
	{
		MemCopyO(this->frameBuff, &this->frameBuff[j], this->frameBuffSize - j);
		this->frameBuffSize -= j;
		this->frameStart -= j;
	}
}

Data::Duration Media::M2VStreamSource::GetFrameStreamTime()
{
	return this->thisFrameTime;
}

void Media::M2VStreamSource::EndFrameStream()
{
	if (this->frameBuffSize > 0 && this->frameStart >= 0)
	{
		this->SubmitFrame(this->frameBuffSize, 0, (UOSInt)this->frameStart);
		this->frameBuffSize = 0;
	}
	this->playEOF = true;
	while (this->playBuffStart != this->playBuffEnd && this->pbc->IsRunning())
	{
		Sync::SimpleThread::Sleep(10);
	}
}

UInt64 Media::M2VStreamSource::GetBitRate()
{
	if (this->totalFrameCnt > 10)
	{
		return this->totalFrameSize * 8 / this->totalFrameCnt * this->frameRateNorm / this->frameRateDenorm;
	}
	else
	{
		return this->bitRate;
	}
}
