#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/DrawEngine.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "UI/GUIForm.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/VideoDObjHandler.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void __stdcall UI::DObj::VideoDObjHandler::OnTimerTick(void *userObj)
{
/*	UI::DObj::DShowVideoDObjHandler *me = (UI::DObj::DShowVideoDObjHandler*)userObj;
	me->graph->CheckStatus();
	if (me->graph->IsCompleted())
	{
		me->graph->SeekToTime(0);
	}*/
}

void __stdcall UI::DObj::VideoDObjHandler::OnPBEnd(void *userObj)
{
	UI::DObj::VideoDObjHandler *me = (UI::DObj::VideoDObjHandler*)userObj;
	me->player->SeekTo(0);
	me->player->StartPlayback();
}

void UI::DObj::VideoDObjHandler::DrawBkg(Media::DrawImage *dimg)
{
	if (this->bmpBkg)
	{
		dimg->DrawImagePt(this->bmpBkg, 0, 0);
	}
	else
	{
		Media::DrawBrush *b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(0, 0, UOSInt2Double(dimg->GetWidth()), UOSInt2Double(dimg->GetHeight()), 0, b);
		dimg->DelBrush(b);
	}
	this->DrawVideo(dimg);
}

void UI::DObj::VideoDObjHandler::DrawVideo(Media::DrawImage *dimg)
{
	if (this->frameImg)
	{
		Sync::MutexUsage mutUsage(&this->frameMut);
		dimg->DrawImagePt(this->frameImg, this->videoX, this->videoY);
	}
}

void UI::DObj::VideoDObjHandler::LockUpdateSize(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->frameMut);
}

void UI::DObj::VideoDObjHandler::DrawFromSurface(Media::MonitorSurface *surface, OSInt destX, OSInt destY, UOSInt buffWidth, UOSInt buffHeight, Bool clearScn)
{
#if defined(VERBOSE)
	printf("VideoDObjHandler DrawFromSurface\r\n");
#endif
	if (this->frameImg)
	{
		Sync::MutexUsage mutUsage(&this->frameMut);
		Bool revOrder;
		UInt8 *bits = this->frameImg->GetImgBits(&revOrder);
		if (bits)
		{
			surface->GetImageData(bits, 0, 0, buffWidth, buffHeight, this->frameImg->GetImgBpl(), revOrder, Media::RotateType::None);
			this->frameImg->GetImgBitsEnd(true);
			this->shown = false;
		}
	}
}

UI::DObj::VideoDObjHandler::VideoDObjHandler(UI::GUIForm *ownerFrm, Media::DrawEngine *deng, Media::ColorManagerSess *colorSess, Media::MonitorSurfaceMgr *surfaceMgr, Parser::ParserList *parsers, Text::CString imageFileName, Int32 videoX, Int32 videoY, UInt32 videoW, UInt32 videoH, Text::CString videoFileName) : UI::DObj::ImageDObjHandler(deng, imageFileName), Media::VideoRenderer(colorSess, surfaceMgr, 4, 4)
{
#if defined(VERBOSE)
	printf("VideoDObjHandler init: w = %d, h = %d\r\n", videoW, videoH);
#endif
	this->parsers = this->parsers;
	this->videoX = videoX;
	this->videoY = videoY;
	this->videoW = videoW;
	this->videoH = videoH;
	this->ownerFrm = ownerFrm;
	this->mf = 0;
	this->player = 0;
	this->UpdateDispInfo(videoW, videoH, 32, Media::PF_B8G8R8A8);
	this->videoFileName = Text::String::New(videoFileName);
	this->frameImg = this->deng->CreateImage32(videoW, videoH, Media::AT_NO_ALPHA);
	{
		IO::StmData::FileData fd(videoFileName, false);
		this->mf = (Media::MediaFile*)parsers->ParseFileType(&fd, IO::ParserType::MediaFile);
	}
	if (this->mf)
	{
#if defined(VERBOSE)
		printf("VideoDObjHandler Media file parsed\r\n");
#endif
		NEW_CLASS(this->player, Media::MediaPlayer(this, 0));
		this->player->LoadMedia(this->mf);
		this->player->StartPlayback();
		this->player->SetEndHandler(OnPBEnd, this);
	}
	this->tmr = this->ownerFrm->AddTimer(100, OnTimerTick, this);
}

UI::DObj::VideoDObjHandler::~VideoDObjHandler()
{
	if (this->player)
	{
		this->player->StopPlayback();
		DEL_CLASS(this->player);
	}
	this->ownerFrm->RemoveTimer(this->tmr);
	this->deng->DeleteImage(this->frameImg);
	this->videoFileName->Release();
	SDEL_CLASS(this->mf);
}

void UI::DObj::VideoDObjHandler::UpdateVideoArea(Int32 videoX, Int32 videoY, UInt32 videoW, UInt32 videoH)
{
	Sync::MutexUsage mutUsage(&this->frameMut);
	this->deng->DeleteImage(this->frameImg);
	this->videoX = videoX;
	this->videoY = videoY;
	this->videoW = videoW;
	this->videoH = videoH;
	this->frameImg = this->deng->CreateImage32(videoW, videoH, Media::AT_NO_ALPHA);
	this->UpdateDispInfo(videoW, videoH, 32, Media::PF_B8G8R8A8);
}
