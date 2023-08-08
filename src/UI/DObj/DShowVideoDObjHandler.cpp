#include "Stdafx.h"
#include "Media/DrawEngine.h"
//#include "Media/DShow/DShowGraph.h"
//#include "Media/DShow/DShowManager.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "UI/GUIForm.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/DShowVideoDObjHandler.h"

void __stdcall UI::DObj::DShowVideoDObjHandler::OnVideoFrame(void *userObj, UInt8 *frameBuff, Int32 frameTime, UInt32 frameW, UInt32 frameH)
{
	UI::DObj::DShowVideoDObjHandler *me = (UI::DObj::DShowVideoDObjHandler*)userObj;
	Media::DrawImage *dimg = me->frameImg;
	Int32 dbpl = (Int32)me->videoSize.x << 2;
	UOSInt outW;
	UOSInt outH;
	if (frameW * me->videoSize.y > frameH * me->videoSize.x)
	{
		outW = me->videoSize.x;
		outH = me->videoSize.x * frameH / frameW;

	}
	else
	{
		outH = me->videoSize.y;
		outW = me->videoSize.y * frameW / frameH;
	}

	Bool revOrder;
	UInt8 *bptr = dimg->GetImgBits(&revOrder);
	if (bptr)
	{
		Sync::MutexUsage mutUsage(me->frameMut);
		if (revOrder)
		{
			me->resizer->Resize(frameBuff, frameW << 2, frameW, frameH, 0, 0, bptr + (Int32)(me->videoSize.y - ((me->videoSize.y - outH) >> 1)) * dbpl - dbpl + (((me->videoSize.x - outW) >> 1) << 2), -dbpl, outW, outH);
		}
		else
		{
			me->resizer->Resize(frameBuff, frameW << 2, frameW, frameH, 0, 0, bptr + (Int32)((me->videoSize.y - outH) >> 1) * dbpl + (((me->videoSize.x - outW) >> 1) << 2), dbpl, outW, outH);
		}
	}
	me->shown = false;
}

void __stdcall UI::DObj::DShowVideoDObjHandler::OnTimerTick(void *userObj)
{
/*	UI::DObj::DShowVideoDObjHandler *me = (UI::DObj::DShowVideoDObjHandler*)userObj;
	me->graph->CheckStatus();
	if (me->graph->IsCompleted())
	{
		me->graph->SeekToTime(0);
	}*/
}

void UI::DObj::DShowVideoDObjHandler::DrawBkg(Media::DrawImage *dimg)
{
	if (this->bmpBkg)
	{
		dimg->DrawImagePt(this->bmpBkg, Math::Coord2DDbl(0, 0));
	}
	this->DrawVideo(dimg);
}

void UI::DObj::DShowVideoDObjHandler::DrawVideo(Media::DrawImage *dimg)
{
	if (this->frameImg)
	{
		Sync::MutexUsage mutUsage(this->frameMut);
		dimg->DrawImagePt(this->frameImg, this->videoTL.ToDouble());
	}
}

UI::DObj::DShowVideoDObjHandler::DShowVideoDObjHandler(UI::GUIForm *ownerFrm, NotNullPtr<Media::DrawEngine> deng, Text::CStringNN imageFileName, Math::Coord2D<OSInt> videoPos, Math::Size2D<UOSInt> videoSize, Text::CString videoFileName) : UI::DObj::ImageDObjHandler(deng, imageFileName)
{
	this->videoTL = videoPos;
	this->videoSize = videoSize;
	this->ownerFrm = ownerFrm;
	this->videoFileName = Text::String::New(videoFileName);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));
	this->frameImg = this->deng->CreateImage32(videoSize, Media::AT_NO_ALPHA);
//	Media::GDIImage *dimg = (Media::GDIImage*)this->frameImg;
//	dimg->info->atype = Media::AT_NO_ALPHA;
	this->tmr = this->ownerFrm->AddTimer(100, OnTimerTick, this);

/*	this->graph = this->dshowMgr->CreateGraph(this->ownerFrm->GetHandle());
	this->renderer = this->dshowMgr->CreateVideoRecvFilter(OnVideoFrame, this);
	this->graph->AddFilter(this->renderer);
	if (this->graph->RenderFile(videoFileName))
	{
		this->graph->Run();
	}*/
}

UI::DObj::DShowVideoDObjHandler::~DShowVideoDObjHandler()
{
	this->ownerFrm->RemoveTimer(this->tmr);
//	SDEL_CLASS(this->graph);
//	DEL_CLASS(this->dshowMgr);
	DEL_CLASS(this->resizer);
	this->deng->DeleteImage(this->frameImg);
	this->videoFileName->Release();
}
