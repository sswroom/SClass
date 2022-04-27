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
		MercatorMapView(Math::Size2D<Double> scnSize, Double centLat, Double centLon, UOSInt maxLevel, UOSInt tileSize);
		virtual ~MercatorMapView();

		virtual void ChangeViewXY(Math::Size2D<Double> scnSize, Math::Coord2DDbl centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2DDbl mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Math::Size2D<Double> scnSize);
		virtual void SetDPI(Double hdpi, Double ddpi);
		void UpdateXY();

		virtual Double GetLeftX();
		virtual Double GetTopY();
		virtual Double GetRightX();
		virtual Double GetBottomY();
		virtual Double GetMapScale();
		virtual Double GetViewScale();
		virtual Math::Coord2DDbl GetCenter();
		virtual Double GetHDPI();
		virtual Double GetDDPI();

		virtual Bool InViewXY(Math::Coord2DDbl mapPos);
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints, Math::Coord2DDbl ofst); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXY(Math::Coord2DDbl mapPos);
		virtual Math::Coord2DDbl ScnXYToMapXY(Math::Coord2DDbl scnPos);
		virtual Map::MapView *Clone();

	private:
		Double Lon2PixelX(Double lon);
		Double Lat2PixelY(Double lat);
		Double PixelX2Lon(Double x);
		Double PixelY2Lat(Double y);
	};
}
#endif
