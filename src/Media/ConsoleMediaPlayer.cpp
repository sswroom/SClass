#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/ConsoleMediaPlayer.h"
#include "Media/MonitorSurfaceMgrFactory.h"

void Media::ConsoleMediaPlayer::OnMediaOpened()
{
	this->PBStart();
}

Media::ConsoleMediaPlayer::ConsoleMediaPlayer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr, Parser::ParserList *parsers, Media::AudioDevice *audioDev) : Media::MediaPlayerInterface(parsers)
{
	this->colorMgr = colorMgr;
	this->surfaceMgr = Media::MonitorSurfaceMgrFactory::Create(monMgr, colorMgr);
	this->colorSess = 0;
	this->renderer = 0;
	this->colorSess = this->colorMgr->CreateSess(this->surfaceMgr->GetMonitorHandle(0)).Ptr();
	NEW_CLASS(this->renderer, Media::ConsoleVideoRenderer(this->surfaceMgr, this->colorSess));
	if (this->renderer && !this->renderer->IsError())
	{
		Media::MediaPlayer *player;
		NEW_CLASS(player, Media::MediaPlayer(this->renderer, audioDev));
		this->SetPlayer(player);
	}
}

Media::ConsoleMediaPlayer::~ConsoleMediaPlayer()
{
	if (this->player)
	{
		this->player->Close();
	}
	SDEL_CLASS(this->renderer);
	NotNullPtr<Media::ColorManagerSess> colorSess;
	if (colorSess.Set(this->colorSess))
	{
		this->colorMgr->DeleteSess(colorSess);
		this->colorSess = 0;
	}
	this->surfaceMgr.Delete();
}

Bool Media::ConsoleMediaPlayer::IsError()
{
	return this->player == 0;
}

void Media::ConsoleMediaPlayer::SetRotateType(Media::RotateType rotateType)
{
	this->renderer->SetRotateType(rotateType);
}

void Media::ConsoleMediaPlayer::SetSurfaceBugMode(Bool surfaceBugMode)
{
	this->renderer->SetSurfaceBugMode(surfaceBugMode);
}