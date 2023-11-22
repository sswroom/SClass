#ifndef _SM_MAP_MAPSCHEDULER
#define _SM_MAP_MAPSCHEDULER
#include "Data/ArrayList.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapView.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
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
		Map::MapDrawLayer *lyr;
		Media::DrawImage *img;
		Media::DrawPen *p;
		Optional<Media::DrawBrush> b;
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

	private:
		void DrawVector(Math::Geometry::Vector2D *vec);
		void DrawPoint(Math::Geometry::Point *pt);
		void DrawLineString(Math::Geometry::LineString *pl);
		void DrawPolyline(Math::Geometry::Polyline *pl);
		void DrawPolygon(Math::Geometry::Polygon *pg);
		void DrawMultiPolygon(Math::Geometry::MultiPolygon *mpg);
		void DrawMultiSurface(Math::Geometry::MultiSurface *ms);
		void DrawCurvePolygon(Math::Geometry::CurvePolygon *cp);
		void DrawGeometryCollection(Math::Geometry::GeometryCollection *geomColl);
	public:
		MapScheduler();
		virtual ~MapScheduler();

		void SetMapView(NotNullPtr<Map::MapView> map, NotNullPtr<Media::DrawImage> img);
		void SetDrawType(NotNullPtr<Map::MapDrawLayer> lyr, Media::DrawPen *p, Optional<Media::DrawBrush> b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty);
		void SetDrawObjs(Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxCnt);
		void Draw(Math::Geometry::Vector2D *vec);
		void DrawNextType(Media::DrawPen *p, Media::DrawBrush *b);
		void WaitForFinish();
	};
}
#endif
