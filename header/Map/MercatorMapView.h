#ifndef _SM_MAP_MERCATORMAPVIEW
#define _SM_MAP_MERCATORMAPVIEW
#include "Data/ArrayListDbl.h"
#include "Map/MapView.h"

namespace Map
{
	class MercatorMapView : public MapView
	{
	private:
		Math::Coord2DDbl centMap;
		Math::Coord2DDbl centPixel;
		Double hdpi;
		Double ddpi;
		UOSInt level;
		UOSInt maxLevel;
		Double dtileSize;

	public:
		MercatorMapView(Math::Size2DDbl scnSize, Math::Coord2DDbl center, UOSInt maxLevel, UOSInt tileSize);
		virtual ~MercatorMapView();

		virtual void ChangeViewXY(Math::Size2DDbl scnSize, Math::Coord2DDbl centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2DDbl mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Math::Size2DDbl scnSize);
		virtual void SetDPI(Double hdpi, Double ddpi);
		void UpdateXY();

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
		virtual Map::MapView *Clone() const;

	private:
		Double Lon2PixelX(Double lon) const;
		Double Lat2PixelY(Double lat) const;
		Double PixelX2Lon(Double x) const;
		Double PixelY2Lat(Double y) const;
	};
}
#endif
