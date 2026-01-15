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

void __stdcall UI::DObj::VideoDObjHandler::OnTimerTick(AnyType userObj)
{
/*	NN<UI::DObj::VideoDObjHandler> me = userObj.GetNN<UI::DObj::VideoDObjHandler>();
	me->graph->CheckStatus();
	if (me->graph->IsCompleted())
	{
		me->graph->SeekToTime(0);
	}*/
}

void __stdcall UI::DObj::VideoDObjHandler::OnPBEnd(AnyType userObj)
{
	NN<UI::DObj::VideoDObjHandler> me = userObj.GetNN<UI::DObj::VideoDObjHandler>();
	NN<Media::MediaPlayer> player;
	if (me->player.SetTo(player))
	{
		player->SeekTo(0);
		player->StartPlayback();
	}
}

void UI::DObj::VideoDObjHandler::DrawBkg(NN<Media::DrawImage> dimg)
{
	ImageDObjHandler::DrawBkg(dimg);
	this->DrawVideo(dimg);
}

void UI::DObj::VideoDObjHandler::DrawVideo(NN<Media::DrawImage> dimg)
{
	NN<Media::DrawImage> frameImg;
	if (this->frameImg.SetTo(frameImg))
	{
		Sync::MutexUsage mutUsage(this->frameMut);
		dimg->DrawImagePt(frameImg, this->videoTL.ToDouble());
	}
}

void UI::DObj::VideoDObjHandler::LockUpdateSize(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->frameMut);
}

void UI::DObj::VideoDObjHandler::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn)
{
#if defined(VERBOSE)
	printf("VideoDObjHandler DrawFromSurface\r\n");
#endif
	NN<Media::DrawImage> img;
	if (this->frameImg.SetTo(img))
	{
		Sync::MutexUsage mutUsage(this->frameMut);
		Bool revOrder;
		UnsafeArray<UInt8> bits;
		if (img->GetImgBits(revOrder).SetTo(bits))
		{
			surface->GetRasterData(bits, 0, 0, buffSize.x, buffSize.y, img->GetImgBpl(), revOrder, Media::RotateType::None);
			img->GetImgBitsEnd(true);
			this->shown = false;
		}
	}
}

UI::DObj::VideoDObjHandler::VideoDObjHandler(NN<UI::GUIForm> ownerFrm, NN<Media::DrawEngine> deng, NN<Media::ColorManagerSess> colorSess, NN<Media::MonitorSurfaceMgr> surfaceMgr, NN<Parser::ParserList> parsers, Text::CStringNN imageFileName, Math::Coord2D<IntOS> videoTL, Math::Size2D<UIntOS> videoSize, Text::CStringNN videoFileName) : UI::DObj::ImageDObjHandler(deng, imageFileName, colorSess), Media::VideoRenderer(colorSess, surfaceMgr, 4, 4)
{
#if defined(VERBOSE)
	printf("VideoDObjHandler init: w = %d, h = %d\r\n", (UInt32)videoSize.x, (UInt32)videoSize.y);
#endif
	this->parsers = parsers;
	this->videoTL = videoTL;
	this->videoSize = videoSize;
	this->ownerFrm = ownerFrm;
	this->mf = nullptr;
	this->player = nullptr;
	this->UpdateDispInfo(videoSize, 32, Media::PF_B8G8R8A8);
	this->videoFileName = Text::String::New(videoFileName);
	this->frameImg = this->deng->CreateImage32(videoSize, Media::AT_IGNORE_ALPHA);
	{
		IO::StmData::FileData fd(videoFileName, false);
		this->mf = Optional<Media::MediaFile>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::MediaFile));
	}
	if (this->mf.NotNull())
	{
#if defined(VERBOSE)
		printf("VideoDObjHandler Media file parsed\r\n");
#endif
		NN<Media::MediaPlayer> player;
		NEW_CLASSNN(player, Media::MediaPlayer(*this, nullptr));
		this->player = player;
		player->LoadMedia(this->mf);
		player->StartPlayback();
		player->SetEndHandler(OnPBEnd, this);
	}
	this->tmr = this->ownerFrm->AddTimer(100, OnTimerTick, this);
}

UI::DObj::VideoDObjHandler::~VideoDObjHandler()
{
	NN<Media::MediaPlayer> player;
	if (this->player.SetTo(player))
	{
		player->StopPlayback();
		player.Delete();
	}
	this->ownerFrm->RemoveTimer(this->tmr);
	NN<Media::DrawImage> img;
	if (this->frameImg.SetTo(img))
	{
		this->deng->DeleteImage(img);
	}
	this->videoFileName->Release();
	this->mf.Delete();
}

void UI::DObj::VideoDObjHandler::UpdateVideoArea(Math::Coord2D<IntOS> videoTL, Math::Size2D<UIntOS> videoSize)
{
	Sync::MutexUsage mutUsage(this->frameMut);
	NN<Media::DrawImage> img;
	if (this->frameImg.SetTo(img))
	{
		this->deng->DeleteImage(img);
	}
	this->videoTL = videoTL;
	this->videoSize = videoSize;
	this->frameImg = this->deng->CreateImage32(videoSize, Media::AT_IGNORE_ALPHA);
	this->UpdateDispInfo(videoSize, 32, Media::PF_B8G8R8A8);
}

void UI::DObj::VideoDObjHandler::SetRotateType(Media::RotateType rotType)
{
}

Media::RotateType UI::DObj::VideoDObjHandler::GetRotateType() const
{
	return Media::RotateType::None;
}
