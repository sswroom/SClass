#ifndef _SM_MEDIA_CONSOLEMEDIAPLAYER
#define _SM_MEDIA_CONSOLEMEDIAPLAYER
#include "Media/ColorManager.h"
#include "Media/MediaFile.h"
#include "Media/MediaPlayerInterface.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"
#include "Parser/ParserList.h"

namespace Media
{
	class ConsoleMediaPlayer : public Media::MediaPlayerInterface
	{
	private:
		Media::MonitorSurfaceMgr *surfaceMgr;
		Media::MonitorSurface *primarySurface;

	public:
		ConsoleMediaPlayer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr, Parser::ParserList *parsers);
		virtual ~ConsoleMediaPlayer();

		Bool IsError();
		void Stop();
	};
}
#endif
