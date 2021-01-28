#ifndef _SM_MAP_PROJECTEDMAPVIEW
#define _SM_MAP_PROJECTEDMAPVIEW
#include "Map/MapView.h"

namespace Map
{
	class ProjectedMapView : public MapView
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
		ProjectedMapView(UOSInt scnWidth, UOSInt scnHeight, Double centX, Double centY, Double scale);
		virtual ~ProjectedMapView();

		virtual void ChangeViewXY(UOSInt scnWidth, UOSInt scnHeight, Double centX, Double centY, Double scale);
		virtual void SetCenterXY(Double mapX, Double mapY);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(UOSInt width, UOSInt height);
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
		virtual Bool MapXYToScnXY(const Double *srcArr, Int32 *destArr, OSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Bool MapXYToScnXY(const Double *srcArr, Double *destArr, OSInt nPoints, Double ofstX, Double ofstY); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, OSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual void MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY);
		virtual void ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY);
		virtual Map::MapView *Clone();
	};
}
#endif
