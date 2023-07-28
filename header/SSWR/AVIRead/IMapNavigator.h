#ifndef _SM_SSWR_AVIREAD_IMAPNAVIGATOR
#define _SM_SSWR_AVIREAD_IMAPNAVIGATOR
#include "Map/MapDrawLayer.h"
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/Vector2D.h"
#include "UI/GUIControl.h"

namespace SSWR
{
	namespace AVIRead
	{
		class IMapNavigator
		{
		public:
			typedef Bool (__stdcall *MouseEvent)(void *userObj, Math::Coord2D<OSInt> scnPos);

			virtual UInt32 GetSRID() = 0;
			virtual Bool InMap(Math::Coord2DDbl pos) = 0;
			virtual void PanToMap(Math::Coord2DDbl pos) = 0;
			virtual void ShowMarker(Math::Coord2DDbl pos) = 0;
			virtual void ShowMarkerDir(Math::Coord2DDbl pos, Double dir, Math::Unit::Angle::AngleUnit unit) = 0;
			virtual void HideMarker() = 0;
			virtual void AddLayer(Map::MapDrawLayer *layer) = 0;
			virtual void SetSelectedVector(Math::Geometry::Vector2D *vec) = 0;
			virtual void SetSelectedVectors(NotNullPtr<Data::ArrayList<Math::Geometry::Vector2D*>> vecList) = 0;
			virtual void RedrawMap() = 0;
			virtual Math::CoordinateSystem *GetCoordinateSystem() = 0;

			virtual Math::Coord2DDbl ScnXY2MapXY(Math::Coord2D<OSInt> scnPos) = 0;
			virtual Math::Coord2D<OSInt> MapXY2ScnXY(Math::Coord2DDbl mapPos) = 0;
			virtual void SetMapCursor(UI::GUIControl::CursorType curType) = 0;
			virtual void HandleMapMouseDown(MouseEvent evt, void *userObj) = 0;
			virtual void HandleMapMouseUp(MouseEvent evt, void *userObj) = 0;
			virtual void HandleMapMouseMove(MouseEvent evt, void *userObj) = 0;
			virtual void UnhandleMapMouse(void *userObj) = 0;

			virtual void SetKMapEnv(const UTF8Char *kmapIP, Int32 kmapPort, Int32 lcid) = 0;
			virtual Bool HasKMap() = 0;
			virtual UTF8Char *ResolveAddress(UTF8Char *sbuff, Math::Coord2DDbl pos) = 0;

			virtual void PauseUpdate() = 0;
			virtual void ResumeUpdate() = 0;
		};
	}
}
#endif
