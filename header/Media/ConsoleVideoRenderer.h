#ifndef _SM_MEDIA_CONSOLEVIDEORENDERER
#define _SM_MEDIA_CONSOLEVIDEORENDERER
#include "Media/ColorManager.h"
#include "Media/MediaFile.h"
#include "Media/MediaPlayerInterface.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"
#include "Parser/ParserList.h"

namespace Media
{
	class ConsoleVideoRenderer : public Media::VideoRenderer
	{
	private:
		Media::MonitorSurfaceMgr *surfaceMgr;
		Media::MonitorSurface *primarySurface;
		Sync::Mutex mut;

	public:
		ConsoleVideoRenderer(Media::MonitorSurfaceMgr *surfaceMgr, Media::ColorManagerSess *colorSess);
		virtual ~ConsoleVideoRenderer();

		Bool IsError();
		virtual void SetRotateType(Media::RotateType rotateType);
		void SetSurfaceBugMode(Bool surfaceBugMode);

		virtual Bool IsUpdatingSize();
		virtual void LockUpdateSize(NotNullPtr<Sync::MutexUsage> mutUsage);
		virtual void DrawFromSurface(Media::MonitorSurface *surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn);

	};
}
#endif
