#ifndef _SM_MAP_MAPSCHEDULER
#define _SM_MAP_MAPSCHEDULER
#include "AnyType.h"
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
		Optional<Media::DrawPen> p;
		Optional<Media::DrawBrush> b;
		Media::DrawImage *ico;
		Double icoSpotX;
		Double icoSpotY;
		ThreadState dt;
		Math::RectAreaDbl *objBounds;
		UOSInt *objCnt;
		UOSInt maxCnt;

		Sync::Mutex taskMut;
		Data::ArrayList<Optional<Math::Geometry::Vector2D>> tasks;
		Bool toStop;
		Sync::Event taskEvt;
		Sync::Event finishEvt;
		Bool threadRunning;
		Bool taskFinish;
		Bool isFirst;
		Bool *isLayerEmpty;

	private:
		static UInt32 __stdcall MapThread(AnyType obj);

	private:
		void DrawVector(NN<Math::Geometry::Vector2D> vec);
		void DrawPoint(NN<Math::Geometry::Point> pt);
		void DrawLineString(NN<Math::Geometry::LineString> pl);
		void DrawPolyline(NN<Math::Geometry::Polyline> pl);
		void DrawPolygon(NN<Math::Geometry::Polygon> pg);
		void DrawMultiPolygon(NN<Math::Geometry::MultiPolygon> mpg);
		void DrawMultiSurface(NN<Math::Geometry::MultiSurface> ms);
		void DrawCurvePolygon(NN<Math::Geometry::CurvePolygon> cp);
		void DrawGeometryCollection(NN<Math::Geometry::GeometryCollection> geomColl);
	public:
		MapScheduler();
		virtual ~MapScheduler();

		void SetMapView(NN<Map::MapView> map, NN<Media::DrawImage> img);
		void SetDrawType(NN<Map::MapDrawLayer> lyr, Optional<Media::DrawPen> p, Optional<Media::DrawBrush> b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty);
		void SetDrawObjs(Math::RectAreaDbl *objBounds, UOSInt *objCnt, UOSInt maxCnt);
		void Draw(NN<Math::Geometry::Vector2D> vec);
		void DrawNextType(Optional<Media::DrawPen> p, Optional<Media::DrawBrush> b);
		void WaitForFinish();
	};
}
#endif
