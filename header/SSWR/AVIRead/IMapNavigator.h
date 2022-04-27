#ifndef _SM_SSWR_AVIREAD_IMAPNAVIGATOR
#define _SM_SSWR_AVIREAD_IMAPNAVIGATOR
#include "Map/IMapDrawLayer.h"
#include "Math/Vector2D.h"
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
			virtual Bool InMap(Double lat, Double lon) = 0;
			virtual void PanToMap(Double lat, Double lon) = 0;
			virtual void ShowMarker(Double lat, Double lon) = 0;
			virtual void ShowMarkerDir(Double lat, Double lon, Double dir, Math::Unit::Angle::AngleUnit unit) = 0;
			virtual void HideMarker() = 0;
			virtual void AddLayer(Map::IMapDrawLayer *layer) = 0;
			virtual void SetSelectedVector(Math::Vector2D *vec) = 0;
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
			virtual UTF8Char *ResolveAddress(UTF8Char *sbuff, Double lat, Double lon) = 0;

			virtual void PauseUpdate() = 0;
			virtual void ResumeUpdate() = 0;
		};
	}
}
#endif
