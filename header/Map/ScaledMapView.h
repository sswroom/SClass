#ifndef _SM_MAP_SCALEDMAPVIEW
#define _SM_MAP_SCALEDMAPVIEW
#include "Map/MapView.h"

namespace Map
{
	class ScaledMapView : public MapView
	{
	private:
		Double centX;
		Double centY;
		Double hdpi;
		Double ddpi;
		Double scale;
		Double leftX;
		Double topY;
		Double rightX;
		Double bottomY;

	public:
		ScaledMapView(Double scnWidth, Double scnHeight, Double centLat, Double centLon, Double scale);
		virtual ~ScaledMapView();

		virtual void ChangeViewXY(Double scnWidth, Double scnHeight, Double centX, Double centY, Double scale);
		virtual void SetCenterXY(Double mapX, Double mapY);
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
		virtual void MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY);
		virtual void ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY);
		virtual Map::MapView *Clone();
	};
}
#endif
