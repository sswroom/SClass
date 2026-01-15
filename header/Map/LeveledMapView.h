#ifndef _SM_MAP_LEVELEDMAPVIEW
#define _SM_MAP_LEVELEDMAPVIEW
#include "Data/ArrayListDbl.h"
#include "Map/MapView.h"

namespace Map
{
	class LeveledMapView : public MapView
	{
	private:
		Math::Coord2DDbl centMap;
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;
		Double hdpi;
		Double ddpi;
		UInt32 level;
		Bool projected;

		Data::ArrayListDbl scales;

	private:
		void UpdateVals();

	public:
		LeveledMapView(Bool projected, Math::Size2DDbl scnSize, Math::Coord2DDbl center, NN<const Data::ArrayListDbl> scales);
		virtual ~LeveledMapView();

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
		virtual Bool MapXYToScnXYArr(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UIntOS nPoints, Math::Coord2DDbl ofst) const; // return inScreen
		virtual Bool IMapXYToScnXY(Double mapRate, UnsafeArray<const Math::Coord2D<Int32>> srcArr, UnsafeArray<Math::Coord2D<Int32>> destArr, UIntOS nPoints, Math::Coord2D<Int32> ofst) const; // return inScreen
		virtual Math::Coord2DDbl MapXYToScnXYNoDir(Math::Coord2DDbl mapPos) const;
		virtual Math::Coord2DDbl ScnXYNoDirToMapXY(Math::Coord2DDbl scnPos) const;
		virtual NN<Map::MapView> Clone() const;
	};
}
#endif
