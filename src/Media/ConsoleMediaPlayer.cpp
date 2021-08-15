#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/ConsoleMediaPlayer.h"
#include "Media/DefaultMonitorSurfaceMgr.h"

void Media::ConsoleMediaPlayer::OnMediaOpened()
{
	this->PBStart();
}

Media::ConsoleMediaPlayer::ConsoleMediaPlayer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr, Parser::ParserList *parsers, Media::AudioDevice *audioDev) : Media::MediaPlayerInterface(parsers)
{
	this->colorMgr = colorMgr;
	this->surfaceMgr = Media::DefaultMonitorSurfaceMgr::Create(monMgr, colorMgr);
	this->colorSess = 0;
	this->renderer = 0;

	if (this->surfaceMgr)
	{
		this->colorSess = this->colorMgr->CreateSess(this->surfaceMgr->GetMonitorHandle(0));
		NEW_CLASS(this->renderer, Media::ConsoleVideoRenderer(this->surfaceMgr, this->colorSess));
	}
	else
	{
		return;
	}
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
	if (this->colorSess)
	{
		this->colorMgr->DeleteSess(this->colorSess);
		this->colorSess = 0;
	}
	SDEL_CLASS(this->surfaceMgr);
}

Bool Media::ConsoleMediaPlayer::IsError()
{
	return this->player == 0;
}
