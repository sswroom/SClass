#ifndef _SM_MAP_MERCATORMAPVIEW
#define _SM_MAP_MERCATORMAPVIEW
#include "Data/ArrayListDbl.h"
#include "Map/MapView.h"

namespace Map
{
	class MercatorMapView : public MapView
	{
	private:
		Math::Coord2D<Double> centMap;
		Math::Coord2D<Double> centPixel;
		Double hdpi;
		Double ddpi;
		UOSInt level;
		UOSInt maxLevel;
		Double dtileSize;

	public:
		MercatorMapView(Double scnWidth, Double scnHeight, Double centLat, Double centLon, UOSInt maxLevel, UOSInt tileSize);
		virtual ~MercatorMapView();

		virtual void ChangeViewXY(Double scnWidth, Double scnHeight, Math::Coord2D<Double> centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2D<Double> mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Double width, Double height);
		virtual void SetDPI(Double hdpi, Double ddpi);
		void UpdateXY();

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

		virtual Bool InViewXY(Double x, Double y);
		virtual Bool MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2D<Double> *srcArr, Math::Coord2D<Double> *destArr, UOSInt nPoints, Math::Coord2D<Double> ofst); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Math::Coord2D<Double> MapXYToScnXY(Math::Coord2D<Double> mapPos);
		virtual Math::Coord2D<Double> ScnXYToMapXY(Math::Coord2D<Double> scnPos);
		virtual Map::MapView *Clone();

	private:
		Double Lon2PixelX(Double lon);
		Double Lat2PixelY(Double lat);
		Double PixelX2Lon(Double x);
		Double PixelY2Lat(Double y);
	};
};
#endif
