#ifndef _SM_MAP_SCALEDMAPVIEW
#define _SM_MAP_SCALEDMAPVIEW
#include "Map/MapView.h"

namespace Map
{
	class ScaledMapView : public MapView
	{
	private:
		Bool projected;
		Math::Coord2DDbl centMap;
		Double hdpi;
		Double ddpi;
		Double scale;
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;

	public:
		ScaledMapView(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale, Bool projected);
		virtual ~ScaledMapView();

		virtual void ChangeViewXY(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2DDbl mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Math::Size2DDbl scnSize);
		virtual void SetDPI(Double hdpi, Double ddpi);

		virtual Math::Quadrilateral GetBounds() const;
		virtual Math::RectAreaDbl GetVerticalRect() const;
		virtual Double GetMapScale() const;
		virtual Double GetViewScale() const;
		virtual Math::Coord2DDbl GetCenter() const;
		virtual Double GetHDPI() const;
		virtual Double GetDDPI() const;

		virtual Bool InViewXY(Math::Coord2DDbl mapPos) const;
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const; // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst) const; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Math::Coord2D<Int32> *srcArr, Math::Coord2D<Int32> *destArr, UOSInt nPoints, Math::Coord2D<Int32> ofst) const; // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos) const;
		virtual Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos) const;
		virtual NotNullPtr<Map::MapView> Clone() const;

		static Double CalcScale(Math::RectAreaDbl bounds, Math::Size2DDbl scnSize, Double dpi, Bool projected);
	};
}
#endif
