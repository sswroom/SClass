#ifndef _SM_SSWR_AVIREAD_AVIRMAPNAVIGATOR
#define _SM_SSWR_AVIREAD_AVIRMAPNAVIGATOR
#include "Map/MapDrawLayer.h"
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/Vector2D.h"
#include "UI/GUIControl.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMapNavigator
		{
		public:
			typedef Bool (CALLBACKFUNC MouseEvent)(AnyType userObj, Math::Coord2D<OSInt> scnPos);

			virtual UInt32 GetSRID() = 0;
			virtual Bool InMap(Math::Coord2DDbl pos) = 0;
			virtual void PanToMap(Math::Coord2DDbl pos) = 0;
			virtual void ShowMarker(Math::Coord2DDbl pos) = 0;
			virtual void ShowMarkerDir(Math::Coord2DDbl pos, Double dir, Math::Unit::Angle::AngleUnit unit) = 0;
			virtual void HideMarker() = 0;
			virtual void AddLayerFromFile(Text::CStringNN fileName) = 0;
			virtual void AddLayer(NN<Map::MapDrawLayer> layer) = 0;
			virtual void SetSelectedVector(Optional<Math::Geometry::Vector2D> vec) = 0;
			virtual void SetSelectedVectors(NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList) = 0;
			virtual void RedrawMap() = 0;
			virtual Double GetScnDPI() const = 0;
			virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const = 0;

			virtual Math::Coord2DDbl ScnXY2MapXY(Math::Coord2D<OSInt> scnPos) = 0;
			virtual Math::Coord2D<OSInt> MapXY2ScnXY(Math::Coord2DDbl mapPos) = 0;
			virtual void SetMapCursor(UI::GUIControl::CursorType curType) = 0;
			virtual void HandleMapMouseLDown(MouseEvent evt, AnyType userObj) = 0;
			virtual void HandleMapMouseLUp(MouseEvent evt, AnyType userObj) = 0;
			virtual void HandleMapMouseRDown(MouseEvent evt, AnyType userObj) = 0;
			virtual void HandleMapMouseRUp(MouseEvent evt, AnyType userObj) = 0;
			virtual void HandleMapMouseMove(MouseEvent evt, AnyType userObj) = 0;
			virtual void UnhandleMapMouse(AnyType userObj) = 0;

			virtual void SetKMapEnv(UnsafeArray<const UTF8Char> kmapIP, Int32 kmapPort, Int32 lcid) = 0;
			virtual Bool HasKMap() = 0;
			virtual UnsafeArrayOpt<UTF8Char> ResolveAddress(UnsafeArray<UTF8Char> sbuff, Math::Coord2DDbl pos) = 0;

			virtual void PauseUpdate() = 0;
			virtual void ResumeUpdate() = 0;
		};
	}
}
#endif
