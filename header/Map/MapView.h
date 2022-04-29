#ifndef _SM_MAP_MAPVIEW
#define _SM_MAP_MAPVIEW
#include "Math/Coord2D.h"
#include "Math/Size2D.h"
#include "Media/DrawEngine.h"

namespace Map
{
	class MapView
	{
	protected:
		Math::Size2D<Double> scnSize;
		
	public:
		MapView(Math::Size2D<Double> scnSize);
		virtual ~MapView();

		virtual void ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale) = 0;
		virtual void SetCenterXY(Math::Coord2DDbl mapPos) = 0;
		virtual void SetMapScale(Double scale) = 0;
		virtual void UpdateSize(Math::Size2D<Double> scnSize) = 0;
		virtual void SetDPI(Double hdpi, Double ddpi) = 0;

		virtual Math::Quadrilateral GetBounds() = 0;
		virtual Math::RectAreaDbl GetVerticalRect() = 0;
		virtual Double GetMapScale() = 0;
		virtual Double GetViewScale() = 0;
		virtual Math::Coord2DDbl GetCenter() = 0;
		virtual Double GetHDPI() = 0;
		virtual Double GetDDPI() = 0;

		virtual Bool InViewXY(Math::Coord2DDbl mapPos) = 0;
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) = 0; // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst) = 0; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) = 0; // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos) = 0;
		virtual Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos) = 0;
		virtual Map::MapView *Clone() = 0;

		Double GetScnWidth();
		Double GetScnHeight();
		Math::Size2D<Double> GetScnSize();
		void SetVAngle(Double angleRad);

		void SetDestImage(Media::DrawImage *img);
		void ToPointCnt(Int32 *parts, Int32 nParts, Int32 nPoints);
		void SetViewBounds(Double x1, Double y1, Double x2, Double y2);
	};
}
#endif
