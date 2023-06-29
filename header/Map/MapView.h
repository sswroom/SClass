#ifndef _SM_MAP_MAPVIEW
#define _SM_MAP_MAPVIEW
#include "Math/Coord2D.h"
#include "Math/Size2DDbl.h"
#include "Media/DrawEngine.h"

namespace Map
{
	class MapView
	{
	protected:
		Math::Size2DDbl scnSize;
		
	public:
		MapView(Math::Size2DDbl scnSize);
		virtual ~MapView();

		virtual void ChangeViewXY(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale) = 0;
		virtual void SetCenterXY(Math::Coord2DDbl mapPos) = 0;
		virtual void SetMapScale(Double scale) = 0;
		virtual void UpdateSize(Math::Size2DDbl scnSize) = 0;
		virtual void SetDPI(Double hdpi, Double ddpi) = 0;

		virtual Math::Quadrilateral GetBounds() const = 0;
		virtual Math::RectAreaDbl GetVerticalRect() const = 0;
		virtual Double GetMapScale() const = 0;
		virtual Double GetViewScale() const = 0;
		virtual Math::Coord2DDbl GetCenter() const = 0;
		virtual Double GetHDPI() const = 0;
		virtual Double GetDDPI() const = 0;

		virtual Bool InViewXY(Math::Coord2DDbl mapPos) const = 0;
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const = 0; // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst) const = 0; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const = 0; // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos) const = 0;
		virtual Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos) const = 0;
		virtual Map::MapView *Clone() const = 0;

		Double GetScnWidth() const;
		Double GetScnHeight() const;
		Math::Size2DDbl GetScnSize() const;
		void SetVAngle(Double angleRad);

		void SetDestImage(Media::DrawImage *img);
		void ToPointCnt(Int32 *parts, Int32 nParts, Int32 nPoints) const;
		void SetViewBounds(Math::RectAreaDbl bounds);
	};
}
#endif
