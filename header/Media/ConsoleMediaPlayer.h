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
		NN<Media::MonitorSurfaceMgr> surfaceMgr;
		NN<Media::ColorManager> colorMgr;
		NN<Media::ColorManagerSess> colorSess;
		NN<Media::ConsoleVideoRenderer> renderer;

		virtual void OnMediaOpened();
	public:
		ConsoleMediaPlayer(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr, NN<Parser::ParserList> parsers, Media::AudioDevice *audioDev);
		virtual ~ConsoleMediaPlayer();

		Bool IsError();
		void SetRotateType(Media::RotateType rotateType);
		void SetSurfaceBugMode(Bool surfaceBugMode);
	};
}
#endif
