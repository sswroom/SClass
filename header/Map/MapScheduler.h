#ifndef _SM_MAP_MAPSCHEDULER
#define _SM_MAP_MAPSCHEDULER
#include "Data/ArrayList.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapView.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Vector2D.h"
#include "Media/DrawEngine.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Map
{
	class MapScheduler
	{
	public:
		enum class ThreadState
		{
			Clearing,
			Drawing
		};

	protected:
		Map::MapView *map;
		Map::IMapDrawLayer *lyr;
		Media::DrawImage *img;
		Media::DrawPen *p;
		Media::DrawBrush *b;
		Media::DrawImage *ico;
		Double icoSpotX;
		Double icoSpotY;
		ThreadState dt;
		Math::RectAreaDbl *objBounds;
		UOSInt *objCnt;
		UOSInt maxCnt;

		Sync::Mutex taskMut;
		Data::ArrayList<Math::Geometry::Vector2D *> tasks;
		Bool toStop;
		Sync::Event taskEvt;
		Sync::Event finishEvt;
		Bool threadRunning;
		Bool taskFinish;
		Bool isFirst;
		Bool *isLayerEmpty;

	private:
		static UInt32 __stdcall MapThread(void *obj);

	protected:
		virtual void DrawPoints(Math::Geometry::Point *pt);
	public:
		MapScheduler();
		virtual ~MapScheduler();

		void SetMapView(Map::MapView *map, Media::DrawImage *img);
		void SetDrawType(Map::IMapDrawLayer *lyr, Media::DrawPen *p, Media::DrawBrush *b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty);
		void SetDrawObjs(Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxCnt);
		void Draw(Math::Geometry::Vector2D *vec);
		void DrawNextType(Media::DrawPen *p, Media::DrawBrush *b);
		void WaitForFinish();
	};
}
#endif
