#ifndef _SM_MAP_MAPDRAWUTIL
#define _SM_MAP_MAPDRAWUTIL
#include "Map/MapView.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiCurve.h"
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
		static Bool DrawPoint(NN<Math::Geometry::Point> pt, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawLineString(NN<Math::Geometry::LineString> pl, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawPolyline(NN<Math::Geometry::Polyline> pl, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawLinearRing(NN<Math::Geometry::LinearRing> pg, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawPolygon(NN<Math::Geometry::Polygon> pg, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawMultiPolygon(NN<Math::Geometry::MultiPolygon> mpg, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawMultiSurface(NN<Math::Geometry::MultiSurface> ms, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawCurvePolygon(NN<Math::Geometry::CurvePolygon> cp, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawCompoundCurve(NN<Math::Geometry::CompoundCurve> cc, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawMultiCurve(NN<Math::Geometry::MultiCurve> mc, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawGeometryCollection(NN<Math::Geometry::GeometryCollection> geomColl, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawEllipse(NN<Math::Geometry::Ellipse> circle, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
		static Bool DrawVectorImage(NN<Math::Geometry::VectorImage> vimg, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
	public:
		static Bool DrawVector(NN<Math::Geometry::Vector2D> vec, NN<Media::DrawImage> img, NN<Map::MapView> view, Optional<Media::DrawBrush> b, Optional<Media::DrawPen> p, Math::Coord2DDbl ofst);
	};
}
#endif
