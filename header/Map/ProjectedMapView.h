#ifndef _SM_MAP_PROJECTEDMAPVIEW
#define _SM_MAP_PROJECTEDMAPVIEW
#include "Map/MapView.h"

namespace Map
{
	class ProjectedMapView : public MapView
	{
	private:
		Math::Coord2DDbl centMap;
		Double hdpi;
		Double ddpi;
		Double scale;
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;

	public:
		ProjectedMapView(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale);
		virtual ~ProjectedMapView();

		virtual void ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2DDbl mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Math::Size2D<Double> scnSize);
		virtual void SetDPI(Double hdpi, Double ddpi);

		virtual Math::Quadrilateral GetBounds();
		virtual Math::RectAreaDbl GetVerticalRect();
		virtual Double GetMapScale();
		virtual Double GetViewScale();
		virtual Math::Coord2DDbl GetCenter();
		virtual Double GetHDPI();
		virtual Double GetDDPI();

		virtual Bool InViewXY(Math::Coord2DDbl mapPos);
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos);
		virtual Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos);
		virtual Map::MapView *Clone();
	};
}
#endif
