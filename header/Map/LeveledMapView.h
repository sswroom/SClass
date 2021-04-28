#ifndef _SM_MAP_LEVELEDMAPVIEW
#define _SM_MAP_LEVELEDMAPVIEW
#include "Data/ArrayListDbl.h"
#include "Map/MapView.h"

namespace Map
{
	class LeveledMapView : public MapView
	{
	private:
		Double centX;
		Double centY;
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
		LeveledMapView(UOSInt scnWidth, UOSInt scnHeight, Double centLat, Double centLon, Data::ArrayListDbl *scales);
		virtual ~LeveledMapView();

		virtual void ChangeViewXY(UOSInt scnWidth, UOSInt scnHeight, Double centX, Double centY, Double scale);
		virtual void SetCenterXY(Double x, Double y);
		virtual void SetMapScale(Double scale);
		virtual void UpdateSize(UOSInt width, UOSInt height);
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
		virtual Bool MapXYToScnXY(const Double *srcArr, Double *destArr, UOSInt nPoints, Double ofstX, Double ofstY); // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, const Int32 *srcArr, Int32 *destArr, UOSInt nPoints, Int32 ofstX, Int32 ofstY); // return inScreen
		virtual void MapXYToScnXY(Double mapX, Double mapY, Double *scnX, Double *scnY);
		virtual void ScnXYToMapXY(Double scnX, Double scnY, Double *mapX, Double *mapY);
		virtual Map::MapView *Clone();
	};
}
#endif
