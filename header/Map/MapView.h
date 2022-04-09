#ifndef _SM_MAP_MAPVIEW
#define _SM_MAP_MAPVIEW

#include "Media/DrawEngine.h"

namespace Map
{
	class MapView
	{
	protected:
		Double scnWidth;
		Double scnHeight;
		
	public:
		MapView(Double scnWidth, Double scnHeight);
		virtual ~MapView();

		virtual void ChangeViewXY(Double scnWidth, Double scnHeight, Double centX, Double centY, Double scale) = 0;
		virtual void SetCenterXY(Double x, Double y) = 0;
		virtual void SetMapScale(Double scale) = 0;
		virtual void UpdateSize(Double width, Double height) = 0;
		virtual void SetDPI(Double hdpi, Double ddpi) = 0;

		virtual Double GetLeftX() = 0;
		virtual Double GetTopY() = 0;
		virtual Double GetRightX() = 0;
		virtual Double GetBottomY() = 0;
		virtual Double GetMapScale() = 0;
		virtual Double GetViewScale() = 0;
		virtual Double GetCenterX() = 0;
		virtual Double GetCenterY() = 0;
		virtual Double GetHDPI() = 0;
		virtual Double GetDDPI() = 0;

		virtual Bool InViewXY(Double x, Double y) = 0;
		virtual Bool MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY) = 0; // return inScreen
		virtual Bool MapXYToScnXY(const Double *srcArr, Double *destArr, UOSInt nPoints, Double ofstX, Double ofstY) = 0; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY) = 0; // return inScreen
		virtual void MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY) = 0;
		virtual void ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY) = 0;
		virtual Map::MapView *Clone() = 0;

		Double GetScnWidth();
		Double GetScnHeight();

		void SetDestImage(Media::DrawImage *img);
		void ToPointCnt(Int32 *parts, Int32 nParts, Int32 nPoints);
		void SetViewBounds(Double x1, Double y1, Double x2, Double y2);
	};
}
#endif
