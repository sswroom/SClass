#ifndef _SM_MAP_MAPVIEW
#define _SM_MAP_MAPVIEW
#include "Math/Coord2D.h"
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

		virtual void ChangeViewXY(Double scnWidth, Double scnHeight, Math::Coord2D<Double> centMap, Double scale) = 0;
		virtual void SetCenterXY(Math::Coord2D<Double> mapPos) = 0;
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
		Math::Coord2D<Double> GetCenterMap();

		virtual Bool InViewXY(Double x, Double y) = 0;
		virtual Bool MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY) = 0; // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2D<Double> *srcArr, Math::Coord2D<Double> *destArr, UOSInt nPoints, Math::Coord2D<Double> ofst) = 0; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY) = 0; // return inScreen
		virtual Math::Coord2D<Double> MapXYToScnXY(Math::Coord2D<Double> mapPos) = 0;
		virtual Math::Coord2D<Double> ScnXYToMapXY(Math::Coord2D<Double> scnPos) = 0;
		virtual Map::MapView *Clone() = 0;

		Double GetScnWidth();
		Double GetScnHeight();

		void SetDestImage(Media::DrawImage *img);
		void ToPointCnt(Int32 *parts, Int32 nParts, Int32 nPoints);
		void SetViewBounds(Double x1, Double y1, Double x2, Double y2);
	};
}
#endif
