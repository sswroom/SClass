#include "Stdafx.h"
#include "Media/VideoRenderer.h"

Media::VideoRenderer::VideoRenderer()
{

}

Media::VideoRenderer::~VideoRenderer()
{

}

/*void Media::VideoRenderer::SetVideo(Media::IVideoSource *video)
{
	UOSInt i;
	///////////////////////////////////////
	this->playing = false;
	this->dispMut->LockWrite();
	this->dispClk = 0;
	this->dispMut->UnlockWrite();

	this->VideoBeginLoad();
	this->avOfst = 0;
	if (this->video)
	{
		this->uvOfst->Stop();
	}

	this->video = video;
	if (this->video)
	{
		this->ivtc->SetSourceVideo(this->video);
		this->autoCrop->SetSourceVideo(this->ivtc);
		this->uvOfst->SetSourceVideo(this->autoCrop);
	}
	else
	{
		this->ivtc->SetSourceVideo(0);
		this->autoCrop->SetSourceVideo(0);
		this->uvOfst->SetSourceVideo(0);
	}
//	this->forseFT = false;
	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (this->buffs[i].srcBuff)
		{
			MemFreeA(this->buffs[i].srcBuff);
		}
		this->buffs[i].srcBuff = 0;
	}
	this->VideoEndLoad();

	this->procThisCount = 0;
	if (this->video)
	{
		Media::FrameInfo info;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UOSInt frameSize;
 		if (!this->video->GetVideoInfo(&info, &frameRateNorm, &frameRateDenorm, &frameSize))
		{
			this->video = 0;
			this->ivtc->SetSourceVideo(0);
			this->autoCrop->SetSourceVideo(0);
			this->uvOfst->SetSourceVideo(0);
			return;
		}

		this->VideoBeginLoad();
		this->videoInfo->Set(&info);
		this->frameRateNorm = frameRateNorm;
		this->frameRateDenorm = frameRateDenorm;
		if (info.fourcc == FFMT_YUV444P10LE)
		{
			frameSize = info.storeWidth * info.storeHeight * 8;
		}
		i = this->allBuffCnt;
		while (i-- > 0)
		{
			this->buffs[i].srcBuff = MemAllocA64(UInt8, frameSize);
		}

		this->currSrcRefLuminance = Media::CS::TransferFunc::GetRefLuminance(info.color->rtransfer);
		this->srcColor->Set(info.color);
		this->srcYUVType = info.yuvType;
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->CreateThreadResizer(&this->tstats[i]);
			this->CreateCSConv(&this->tstats[i], &info);
		}
		this->VideoEndLoad();
		this->UpdateCrop();
	}
}

void Media::VideoRenderer::SetHasAudio(Bool hasAudio)
{
	this->hasAudio = hasAudio;
}


void Media::VideoRenderer::SetTimeDelay(Int32 timeDelay)
{
	this->timeDelay = timeDelay;
}

void Media::VideoRenderer::VideoInit(Media::RefClock *clk)
{
	if (this->tstats[0].csconv && !this->playing)
	{
		if (this->video)
		{
			this->dispClk = clk;
			//this->video->Init(OnVideoFrame, OnVideoChange, this);
			this->uvOfst->Init(OnVideoFrame, OnVideoChange, this);
		}
		this->ClearBuff();
	}
}

void Media::VideoRenderer::VideoStart()
{
	if (this->tstats[0].csconv && !this->playing)
	{
		this->ClearBuff();
		if (this->video)
		{
			if (this->uvOfst->Start())
			{
				this->captureFrame = false;
				this->playing = true;
			}
		}
	}
}

void Media::VideoRenderer::StopPlay()
{
	if (this->video)
	{
		this->playing = false;
		this->uvOfst->Stop();
		this->dispEvt->Set();

		this->ClearBuff();
		this->dispMut->LockWrite();
		this->dispClk = 0;
		this->dispMut->UnlockWrite();
	}
}*/
