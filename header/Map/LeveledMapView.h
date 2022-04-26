#ifndef _SM_MAP_LEVELEDMAPVIEW
#define _SM_MAP_LEVELEDMAPVIEW
#include "Data/ArrayListDbl.h"
#include "Map/MapView.h"

namespace Map
{
	class LeveledMapView : public MapView
	{
	private:
		Math::Coord2D<Double> centMap;
		Double hdpi;
		Double ddpi;
		UInt32 level;
		Double leftX;
		Double topY;
		Double rightX;
		Double bottomY;

		Data::ArrayListDbl *scales;

	private:
		void UpdateVals();

	public:
		LeveledMapView(Double scnWidth, Double scnHeight, Double centLat, Double centLon, Data::ArrayListDbl *scales);
		virtual ~LeveledMapView();

		virtual void ChangeViewXY(Double scnWidth, Double scnHeight, Math::Coord2D<Double> centMap, Double scale);
		virtual void SetCenterXY(Math::Coord2D<Double> mapPos);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(Double width, Double height);
		virtual void SetDPI(Double hdpi, Double ddpi);

		virtual Double GetLeftX();
		virtual Double GetTopY();
		virtual Double GetRightX();
		virtual Double GetBottomY();
		virtual Double GetMapScale();
		virtual Double GetViewScale();
		virtual Double GetCenterY();
		virtual Double GetCenterX();
		virtual Double GetHDPI();
		virtual Double GetDDPI();

		virtual Bool InViewXY(Double x, Double y);
		virtual Bool MapXYToScnXY(const Double *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Bool MapXYToScnXY(const Math::Coord2D<Double> *srcArr, Math::Coord2D<Double> *destArr, UOSInt nPoints, Math::Coord2D<Double> ofst); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual Math::Coord2D<Double> MapXYToScnXY(Math::Coord2D<Double> mapPos);
		virtual Math::Coord2D<Double> ScnXYToMapXY(Math::Coord2D<Double> scnPos);
		virtual Map::MapView *Clone();
	};
}
#endif
