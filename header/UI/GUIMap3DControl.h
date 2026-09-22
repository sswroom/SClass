#ifndef _SM_UI_GUIMAP3DCONTROL
#define _SM_UI_GUIMAP3DCONTROL
#include "Data/CallbackStorage.h"
#include "Map/Map3DView.h"
#include "Media/ColorManager.h"
#include "Media/Engine3D.h"
#include "Sync/Mutex.h"
#include "UI/GUICustomDraw.h"

namespace UI
{
	class GUIMap3DControl : public GUICustomDraw
	{
	public:
		typedef void (CALLBACKFUNC MapUpdatedHandler)(AnyType userObj, Math::Coord2DDbl center);
	private:
		NN<Media::Engine3D> eng3d;
		Optional<Media::Engine3DScene> scene3d;
		NN<Map::Map3DView> view3d;
		NN<Media::ColorManagerSess> colorSess;
		Bool mouseDown;
		MouseButton mouseBtn;
		Math::Coord2D<IntOS> lastMouse;
		Data::ArrayListObj<Data::CallbackStorage<MapUpdatedHandler>> mapUpdHdlrs;

	protected:
		virtual UI::EventState OnMouseDown(Math::Coord2D<IntOS> scnPos, MouseButton btn);
		virtual UI::EventState OnMouseUp(Math::Coord2D<IntOS> scnPos, MouseButton btn);
		virtual void OnMouseMove(Math::Coord2D<IntOS> scnPos);
		virtual UI::EventState OnMouseWheel(Math::Coord2D<IntOS> scnPos, Int32 delta);
		virtual void OnDraw(NN<Media::DrawImage> img);

	public:
		GUIMap3DControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, NN<Media::ColorManagerSess> colorSess);
		virtual ~GUIMap3DControl();

		void HandleMapUpdated(MapUpdatedHandler hdlr, AnyType userObj);
		void SetVAngle(Double vAngle);
		void SetHAngle(Double hAngle);
		Double GetVAngle() const;
		Double GetHAngle() const;
		Math::Coord2DDbl GetMapCenter() const;
	};
}
#endif