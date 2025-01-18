#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/ConsoleMediaPlayer.h"
#include "Media/MonitorSurfaceMgrFactory.h"

void Media::ConsoleMediaPlayer::OnMediaOpened()
{
	this->PBStart();
}

Media::ConsoleMediaPlayer::ConsoleMediaPlayer(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr, NN<Parser::ParserList> parsers, Optional<Media::AudioDevice> audioDev) : Media::MediaPlayerInterface(parsers)
{
	this->colorMgr = colorMgr;
	this->surfaceMgr = Media::MonitorSurfaceMgrFactory::Create(monMgr, colorMgr);
	this->colorSess = this->colorMgr->CreateSess(this->surfaceMgr->GetMonitorHandle(0));
	NEW_CLASSNN(this->renderer, Media::ConsoleVideoRenderer(this->surfaceMgr, this->colorSess));
	if (!this->renderer->IsError())
	{
		Media::MediaPlayer *player;
		NEW_CLASS(player, Media::MediaPlayer(this->renderer, audioDev));
		this->SetPlayer(player);
	}
}

Media::ConsoleMediaPlayer::~ConsoleMediaPlayer()
{
	NN<Media::MediaPlayer> player;
	if (this->player.SetTo(player))
	{
		player->Close();
	}
	this->renderer.Delete();
	this->colorMgr->DeleteSess(this->colorSess);
	this->surfaceMgr.Delete();
}

Bool Media::ConsoleMediaPlayer::IsError()
{
	return this->player.IsNull();
}

void Media::ConsoleMediaPlayer::SetRotateType(Media::RotateType rotateType)
{
	this->renderer->SetRotateType(rotateType);
}

void Media::ConsoleMediaPlayer::SetSurfaceBugMode(Bool surfaceBugMode)
{
	this->renderer->SetSurfaceBugMode(surfaceBugMode);
}