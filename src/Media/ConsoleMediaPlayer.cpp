#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/ConsoleMediaPlayer.h"
#include "Media/DefaultMonitorSurfaceMgr.h"

Media::ConsoleMediaPlayer::ConsoleMediaPlayer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr, Parser::ParserList *parsers) : Media::MediaPlayerInterface(parsers)
{
	this->surfaceMgr = Media::DefaultMonitorSurfaceMgr::Create(monMgr, colorMgr);
	if (this->surfaceMgr)
	{
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMgr->GetMonitorHandle(0), 0);
	}
	else
	{
		this->primarySurface = 0;
	}
}

Media::ConsoleMediaPlayer::~ConsoleMediaPlayer()
{
	SDEL_CLASS(this->primarySurface);
	SDEL_CLASS(this->surfaceMgr);
}

Bool Media::ConsoleMediaPlayer::IsError()
{
	return this->primarySurface == 0;
}

void Media::ConsoleMediaPlayer::Stop()
{

}
