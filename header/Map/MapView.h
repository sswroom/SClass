#ifndef _SM_MAP_MAPVIEW
#define _SM_MAP_MAPVIEW
#include "Math/Coord2D.h"
#include "Math/CoordinateConverter.h"
#include "Math/Size2DDbl.h"
#include "Media/DrawEngine.h"

#define SRID_SCREEN 999999999

namespace Map
{
	class MapView : public Math::CoordinateConverter
	{
	private:
		Math::Coord2DDbl converterOfst;
	protected:
		Math::Size2DDbl scnSize;
		Double hAngle;
		Double hSin;
		Double hCos;
		Double hISin;
		Double hICos;
		
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
		virtual Bool MapXYToScnXY(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2D<Int32>> destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const = 0; // return inScreen
		virtual Bool MapXYToScnXY(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UOSInt nPoints, Math::Coord2DDbl ofst) const = 0; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, UnsafeArray<const Math::Coord2D<Int32>> srcArr, UnsafeArray<Math::Coord2D<Int32>> destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const = 0; // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXYNoDir(Math::Coord2DDbl mapPos) const = 0;
		virtual Math::Coord2DDbl ScnXYNoDirToMapXY(Math::Coord2DDbl scnPos) const = 0;
		virtual NN<Map::MapView> Clone() const = 0;
		Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos) const;
		Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos) const;

		Double GetScnWidth() const;
		Double GetScnHeight() const;
		Math::Size2DDbl GetScnSize() const;
		void SetVAngle(Double angleRad);
		void SetHAngle(Double angleRad);

		void SetDestImage(NN<Media::DrawImage> img);
		void ToPointCnt(UnsafeArray<Int32> parts, Int32 nParts, Int32 nPoints) const;
		void SetViewBounds(Math::RectAreaDbl bounds);

		void SetConverterOfst(Math::Coord2DDbl ofst);
		virtual UInt32 GetOutputSRID() const;
		virtual Math::Coord2DDbl Convert2D(Math::Coord2DDbl coord) const;
		virtual Math::Vector3 Convert3D(Math::Vector3 vec3) const;
		virtual void Convert2DArr(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UOSInt nPoints) const;
	};
}
#endif
