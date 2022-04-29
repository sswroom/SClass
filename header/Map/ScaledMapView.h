#ifndef _SM_MAP_SCALEDMAPVIEW
#define _SM_MAP_SCALEDMAPVIEW
#include "Map/MapView.h"

namespace Map
{
	class ScaledMapView : public MapView
	{
	private:
		Math::Coord2DDbl centMap;
		Double hdpi;
		Double ddpi;
		Double scale;
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;

	public:
		ScaledMapView(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale);
		virtual ~ScaledMapView();

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
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst); // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst); // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos);
		virtual Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos);
		virtual Map::MapView *Clone();
	};
}
#endif
