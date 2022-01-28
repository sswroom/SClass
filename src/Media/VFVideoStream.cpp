#include "Stdafx.h"
#include "Media/VFVideoStream.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include "Media/VFAPI.h"

UInt32 __stdcall Media::VFVideoStream::PlayThread(void *userObj)
{
	Media::VFVideoStream *me = (Media::VFVideoStream*)userObj;
	UInt8 *frameBuff;
	UInt32 frameTime;
	Media::FrameType ft;
	UOSInt frameSize;
	UInt32 frameNum;
	me->threadRunning = true;
	frameNum = 0;
	frameSize = me->info->storeWidth * me->info->storeHeight * (me->info->storeBPP >> 3);
	frameBuff = MemAlloc(UInt8, frameSize);
	while (!me->threadToStop)
	{
		if (me->playing)
		{
			if (me->ReadNextFrame(frameBuff, &frameTime, &ft) == 0)
			{
				me->playing = false;
				me->currFrameNum = 0;
			}
			else
			{
				me->frameCb(frameTime, frameNum++, &frameBuff, frameSize, Media::IVideoSource::FS_I, me->frameCbObj, ft, (me->seeked)?Media::IVideoSource::FF_DISCONTTIME:Media::IVideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
				me->seeked = false;
			}
		}
		else
		{
			me->threadEvt->Wait(1000);
		}
	}
	MemFree(frameBuff);
	me->threadRunning = false;
	return 0;
}

Media::VFVideoStream::VFVideoStream(Media::VFMediaFile *mfile)
{
	this->mfile = mfile;
	this->mfile->mut->Lock();
	this->mfile->useCnt++;
	this->mfile->mut->Unlock();
	this->seeked = true;

	VF_StreamInfo_Video vinfo;
	VF_PluginFunc *funcs = (VF_PluginFunc*)mfile->plugin->funcs;
	vinfo.dwSize = sizeof(vinfo);
	funcs->GetStreamInfo(mfile->file, VF_STREAM_VIDEO, &vinfo);
	this->frameCnt = vinfo.dwLengthL;
	this->frameRate = vinfo.dwRate;
	this->frameRateScale = vinfo.dwScale;
	NEW_CLASS(this->info, Media::FrameInfo());
	this->info->storeWidth = vinfo.dwWidth;
	this->info->storeHeight = vinfo.dwHeight;
	this->info->dispWidth = this->info->storeWidth;
	this->info->dispHeight = this->info->storeHeight;
	this->info->fourcc = 0;
	this->info->storeBPP = vinfo.dwBitCount;
	this->info->pf = Media::PixelFormatGetDef(0, vinfo.dwBitCount);
	this->info->byteSize = 0;
	this->info->color->SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
	this->info->atype = Media::AT_NO_ALPHA;
	this->info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info->hdpi = 96;
	Bool isNTSC = false;
	//Bool isPAL = false;
	if (this->frameRateScale == 1001 && this->frameRate == 24000)
	{
		isNTSC = true;
	}
	else if (this->frameRateScale == 1001 && this->frameRate == 30000)
	{
		isNTSC = true;
	}
	else if (this->frameRateScale == 1000 && this->frameRate == 29970)
	{
		isNTSC = true;
	}
	else if (this->frameRateScale == 1000000 && this->frameRate == 29970029)
	{
		isNTSC = true;
	}
	if (isNTSC)
	{
		if ((this->info->dispWidth == 720 && this->info->dispHeight == 480) || (this->info->dispWidth == 704 && this->info->dispHeight == 480))
		{
			this->info->par2 = 1/1.097222222222222222222;
		}
		else
		{
			this->info->par2 = 1;
		}
	}
	else
	{
		this->info->par2 = 1; ////////////////////////
	}
	this->info->ftype = Media::FT_NON_INTERLACE;

	this->currFrameNum = 0;
	this->playing = false;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Media.VFVideoStream.threadEvt"));
	Sync::Thread::Create(PlayThread, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Media::VFVideoStream::~VFVideoStream()
{
	OSInt useCnt;
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->threadEvt);

	this->mfile->mut->Lock();
	useCnt = --this->mfile->useCnt;
	this->mfile->mut->Unlock();
	DEL_CLASS(this->info);
	if (useCnt == 0)
	{
		Text::StrDelNew(this->mfile->fileName);
		VF_PluginFunc *funcs = (VF_PluginFunc*)this->mfile->plugin->funcs;
		funcs->CloseFile(this->mfile->file);
		this->mfile->vfpmgr->Release();
		MemFree(this->mfile);
	}
}

UTF8Char *Media::VFVideoStream::GetSourceName(UTF8Char *buff)
{
	return Text::StrWChar_UTF8(buff, this->mfile->fileName);
}

Text::CString Media::VFVideoStream::GetFilterName()
{
	return CSTR("VFVideoStream");
}

Bool Media::VFVideoStream::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	info->Set(this->info);
	*maxFrameSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
	*frameRateNorm = this->frameRate;
	*frameRateDenorm = this->frameRateScale;
	return true;
}

Bool Media::VFVideoStream::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userObj)
{
	if (this->playing)
	{
		return false;
	}
	this->frameCb = cb;
	this->fcCb = fcCb;
	this->frameCbObj = userObj;
	return true;
}

Bool Media::VFVideoStream::Start()
{
	if (this->playing)
	{
		return false;
	}
	this->seeked = true;
	this->playing = true;
	this->threadEvt->Set();
	return true;
}

void Media::VFVideoStream::Stop()
{
	this->currFrameNum = 0;
	this->playing = false;
}

Bool Media::VFVideoStream::IsRunning()
{
	return this->playing;
}

Int32 Media::VFVideoStream::GetStreamTime()
{
	return (Int32)MulDivU32(this->frameCnt, this->frameRateScale * 1000, this->frameRate);
}

Bool Media::VFVideoStream::CanSeek()
{
	return true;
}

UInt32 Media::VFVideoStream::SeekToTime(UInt32 time)
{
	UInt32 newFrameNum = MulDivU32(time, this->frameRate, this->frameRateScale * 1000);
	if (newFrameNum >= this->frameCnt)
		newFrameNum = this->frameCnt;
	this->currFrameNum = newFrameNum;
	this->seeked = true;
	return MulDivU32(newFrameNum, this->frameRateScale * 1000, this->frameRate);
}

Bool Media::VFVideoStream::IsRealTimeSrc()
{
	return false;
}

Bool Media::VFVideoStream::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	////////////////////////////////////////////
	return false;
}

Bool Media::VFVideoStream::SetPreferFrameType(Media::FrameType ftype)
{
	this->info->ftype = ftype;
	return true;
}

UOSInt Media::VFVideoStream::GetDataSeekCount()
{
	return 0;
}

Bool Media::VFVideoStream::HasFrameCount()
{
	return true;
}

UOSInt Media::VFVideoStream::GetFrameCount()
{
	return this->frameCnt;
}
UInt32 Media::VFVideoStream::GetFrameTime(UOSInt frameIndex)
{
	return MulDivU32((UInt32)frameIndex, this->frameRateScale * 1000, this->frameRate);
}

void Media::VFVideoStream::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	UInt32 i;
	UOSInt dataSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
	i = 0;
	while (i < this->frameCnt)
	{
		if (!cb(MulDivU32(i, this->frameRateScale * 1000, this->frameRate), i, dataSize, Media::IVideoSource::FS_I, Media::FT_NON_INTERLACE, userData, Media::YCOFST_C_CENTER_LEFT))
		{
			break;
		}
		i++;
	}
}

UOSInt Media::VFVideoStream::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	VF_PluginFunc *funcs = (VF_PluginFunc*)mfile->plugin->funcs;
	if (this->currFrameNum >= this->frameCnt)
		return 0;

	VF_ReadData_Video rd;
	rd.dwSize = sizeof(rd);
	rd.dwFrameNumberH = 0;
	rd.dwFrameNumberL = this->currFrameNum;
	rd.lpData = frameBuff;
	rd.lPitch = (int)(this->info->storeWidth * (this->info->storeBPP >> 3));
	funcs->ReadData(mfile->file, VF_STREAM_VIDEO, &rd);
	*frameTime = MulDivU32(this->currFrameNum, this->frameRateScale * 1000, this->frameRate);
	*ftype = this->info->ftype;
	this->currFrameNum++;
	return (UInt32)rd.lPitch * this->info->storeHeight;
}
