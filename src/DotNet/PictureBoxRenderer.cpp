#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "DotNet/PictureBoxRenderer.h"

void DotNet::PictureBoxRenderer::SizeChg(System::Object *sender, System::EventArgs *e)
{
	renderer->OnSizeChanged();
}

void DotNet::PictureBoxRenderer::PlayThread()
{
	playing = true;

	UInt8 *frameBuff;
	Int32 frameTime;
	Media::FrameType frameType;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;

/*	Int32 maxFrameSize;
	this->renderer->StartRender(this->playClk, this->vidsrc->IsRealTimeSrc());
	this->vidsrc->GetVideoInfo(this->info, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
	maxFrameSize = csconv->GetSrcFrameSize(this->info->width, this->info->height);
	frameBuff = MemAlloc(UInt8, maxFrameSize);
	this->vidsrc->SeekToTime(this->playClk->GetCurrTime());
	this->vidsrc->Start();
	if (info->par == 0)
	{
		info->par = 1;
	}
	Bool realTimeSrc = this->vidsrc->IsRealTimeSrc();
	
	while (this->vidsrc->ReadNextFrame(frameBuff, &frameTime, &frameType))
	{
		this->renderer->AddFrame(frameBuff, frameTime, frameType);
		if (this->captureFrame)
		{
			this->captureMut->Lock();
			if (this->captureFrame && this->captureResult == 0)
			{
				Media::StaticImage *img;
				NEW_CLASS(img, Media::StaticImage(this->info->width, this->info->height, this->info->fourcc, this->info->bpp, maxFrameSize, this->info->rgbType, this->info->yuvType, this->info->rgbGamma, Media::AT_NO_ALPHA));
				img->info->par = this->info->par;
				MemCopy(img->data, frameBuff, maxFrameSize);
				this->captureResult = img;
				this->captureEvt->Set();
			}
			this->captureMut->Unlock();
		}

		if (stopPlay)
			break;
	}
	this->vidsrc->Stop();
	this->renderer->StopRender();
	MemFree(frameBuff);*/
	playing = false;
}

DotNet::PictureBoxRenderer::PictureBoxRenderer(System::Windows::Forms::Control *formCtrl, Media::ColorManager *colorMgr)
{
	this->formCtrl = formCtrl;
	this->colorMgr = colorMgr;
	this->csconv = 0;
	this->evtHdlr = new System::EventHandler(this, SizeChg);
	this->formCtrl->SizeChanged += evtHdlr;
//	NEW_CLASS(this->renderer, Media::DDrawRenderer2(formCtrl->Handle.ToPointer(), colorMgr));
	NEW_CLASS(this->renderer, Media::DDrawRendererLR(formCtrl->Handle.ToPointer(), colorMgr));
	NEW_CLASS(this->captureEvt, Sync::Event(true, L"DotNet.PictureBoxRenderer.captureEvt"));
	NEW_CLASS(this->captureMut, Sync::Mutex());
	this->captureFrame = 0;
	this->captureResult = 0;
	this->info = MemAlloc(Media::FrameInfo, 1);
	this->frames = MemAlloc(DotNet::VideoFrame, 1);
}

DotNet::PictureBoxRenderer::~PictureBoxRenderer()
{
	this->formCtrl->SizeChanged -= this->evtHdlr;
	if (this->playing)
	{
		Stop();
	}
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
		this->csconv = 0;
	}
	DEL_CLASS(this->renderer);
	DEL_CLASS(this->captureEvt);
	DEL_CLASS(this->captureMut);
	MemFree(this->info);
	MemFree(this->frames);
}

Bool DotNet::PictureBoxRenderer::BindVideo(Media::IVideoSource *vidsrc)
{
	if (this->playing)
	{
		Stop();
	}
	Int32 maxFrameSize;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	vidsrc->GetVideoInfo(this->info, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
	this->renderer->ChangeFrameFormat(this->info);

	Media::CS::CSConverter *csconv;
//	csconv = Media::CS::CSConverter::NewConverter(info->fourcc, info->bpp, info->rgbType, 0, 32, Media::CS::TRANT_DISPLAY, this->colorMgr);
	csconv = Media::CS::CSConverter::NewConverter(info->fourcc, info->bpp, info->rgbType, *(Int32*)"LRGB", 64, Media::CS::TRANT_sRGB, info->rgbGamma, info->yuvType, this->colorMgr);
	if (csconv)
	{
		if (this->csconv)
		{
			DEL_CLASS(this->csconv);
			this->csconv = 0;
		}
		this->csconv = csconv;
		this->vidsrc = vidsrc;
		return true;
	}
	else
	{
		return false;
	}
}

Media::IVideoRenderer::RendererResult DotNet::PictureBoxRenderer::Start(Media::RefClock *clk)
{
	if (playing)
		return Media::IVideoRenderer::RR_NO_STATUS_CHG;
	if (!clk->Running())
		return Media::IVideoRenderer::RR_CLOCK_NOT_RUNNING;
	if (this->vidsrc == 0)
		return Media::IVideoRenderer::RR_NO_VIDEO;
	stopPlay = false;
	this->playClk = clk;

	System::Threading::Thread *t = new System::Threading::Thread(new System::Threading::ThreadStart(this, PlayThread));
	t->Start();
	return Media::IVideoRenderer::RR_SUCCEED;
}

void DotNet::PictureBoxRenderer::Stop()
{
	if (!playing)
		return;
	stopPlay = true;
	this->vidsrc->Stop();
	this->renderer->StopRender();
	while (playing)
	{
		Sync::SimpleThread::Sleep(10);
		stopPlay = true;
	}
}

Bool DotNet::PictureBoxRenderer::IsPlaying()
{
	return this->playing;
}

Media::Image *DotNet::PictureBoxRenderer::CaptureFrame()
{
	if (!this->playing)
		return 0;
	this->captureMut->Lock();
	if (this->captureFrame)
	{
		this->captureMut->Unlock();
		return 0;
	}
	this->captureFrame = true;
	this->captureResult = 0;
	this->captureMut->Unlock();

	System::DateTime startTime = System::DateTime::UtcNow;
	while (this->captureResult == 0)
	{
		if (System::DateTime::UtcNow.Subtract(startTime).TotalSeconds > 3)
		{
			break;
		}
		this->captureEvt->Wait(1000);
	}
	Media::Image *retImg;
	this->captureMut->Lock();
	this->captureFrame = false;
	retImg = this->captureResult;
	this->captureResult = 0;
	this->captureMut->Unlock();
	return retImg;
}
