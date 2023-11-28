#ifndef _SM_MAP_MAPDRAWUTIL
#define _SM_MAP_MAPDRAWUTIL
#include "Map/MapView.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/Vector2D.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/DrawEngine.h"

namespace Map
{
	class MapDrawUtil
	{
	private:
		static Bool DrawPoint(NotNullPtr<Math::Geometry::Point> pt, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawLineString(NotNullPtr<Math::Geometry::LineString> pl, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawPolyline(NotNullPtr<Math::Geometry::Polyline> pl, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawPolygon(NotNullPtr<Math::Geometry::Polygon> pg, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawMultiPolygon(NotNullPtr<Math::Geometry::MultiPolygon> mpg, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawMultiSurface(NotNullPtr<Math::Geometry::MultiSurface> ms, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawCurvePolygon(NotNullPtr<Math::Geometry::CurvePolygon> cp, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawGeometryCollection(NotNullPtr<Math::Geometry::GeometryCollection> geomColl, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawEllipse(NotNullPtr<Math::Geometry::Ellipse> circle, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawVectorImage(NotNullPtr<Math::Geometry::VectorImage> vimg, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
	public:
		static Bool DrawVector(NotNullPtr<Math::Geometry::Vector2D> vec, NotNullPtr<Media::DrawImage> img, NotNullPtr<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
	};
}
#endif
