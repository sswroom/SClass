#ifndef _SM_MEDIA_CONSOLEMEDIAPLAYER
#define _SM_MEDIA_CONSOLEMEDIAPLAYER
#include "Media/ColorManager.h"
#include "Media/ConsoleVideoRenderer.h"
#include "Media/MediaPlayerInterface.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"
#include "Parser/ParserList.h"

namespace Media
{
	class ConsoleMediaPlayer : public Media::MediaPlayerInterface
	{
	private:
		NotNullPtr<Media::MonitorSurfaceMgr> surfaceMgr;
		Media::ColorManager *colorMgr;
		Media::ColorManagerSess *colorSess;
		Media::ConsoleVideoRenderer *renderer;

		virtual void OnMediaOpened();
	public:
		ConsoleMediaPlayer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr, NotNullPtr<Parser::ParserList> parsers, Media::AudioDevice *audioDev);
		virtual ~ConsoleMediaPlayer();

		Bool IsError();
		void SetRotateType(Media::RotateType rotateType);
		void SetSurfaceBugMode(Bool surfaceBugMode);
	};
}
#endif
