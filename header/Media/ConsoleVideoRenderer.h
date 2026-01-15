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
		NN<Media::MonitorSurfaceMgr> surfaceMgr;
		Optional<Media::MonitorSurface> primarySurface;
		Sync::Mutex mut;

	public:
		ConsoleVideoRenderer(NN<Media::MonitorSurfaceMgr> surfaceMgr, NN<Media::ColorManagerSess> colorSess);
		virtual ~ConsoleVideoRenderer();

		Bool IsError();
		virtual void SetRotateType(Media::RotateType rotateType);
		virtual Media::RotateType GetRotateType() const;
		void SetSurfaceBugMode(Bool surfaceBugMode);

		virtual Bool IsUpdatingSize();
		virtual void LockUpdateSize(NN<Sync::MutexUsage> mutUsage);
		virtual void DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn);

	};
}
#endif
