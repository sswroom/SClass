#ifndef _SM_MAP_PROJECTEDMAPVIEW
#define _SM_MAP_PROJECTEDMAPVIEW
#include "Map/MapView.h"

namespace Map
{
	class ProjectedMapView : public MapView
	{
	private:
		Math::Coord2D<Double> centMap;
		Double hdpi;
		Double ddpi;
		Double scale;
		Double leftX;
		Double topY;
		Double rightX;
		Double bottomY;

	public:
		ProjectedMapView(Double scnWidth, Double scnHeight, Double centX, Double centY, Double scale);
		virtual ~ProjectedMapView();

		virtual void ChangeViewXY(Double scnWidth, Double scnHeight, Math::Coord2D<Double> centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2D<Double> mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Double width, Double height);
		virtual void SetDPI(Double hdpi, Double ddpi);

		virtual Double GetLeftX();
		virtual Double GetTopY();
		virtual Double GetRightX();
		virtual Double GetBottomY();
		virtual Double GetMapScale();
		virtual Double GetViewScale();
		virtual Double GetCenterX();
		virtual Double GetCenterY();
		virtual Double GetHDPI();
		virtual Double GetDDPI();

		virtual Bool InViewXY(Double mapX, Double mapY);
		virtual Bool MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2D<Double> *srcArr, Math::Coord2D<Double> *destArr, UOSInt nPoints, Math::Coord2D<Double> ofst); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Math::Coord2D<Double> MapXYToScnXY(Math::Coord2D<Double> mapPos);
		virtual Math::Coord2D<Double> ScnXYToMapXY(Math::Coord2D<Double> scnPos);
		virtual Map::MapView *Clone();
	};
}
#endif
