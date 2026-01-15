#ifndef _SM_UI_GUICUSTOMDRAW
#define _SM_UI_GUICUSTOMDRAW
#include "IO/Library.h"
#include "Map/DrawMapRenderer.h"
#include "Map/MapView.h"
#include "Math/Geometry/Vector2D.h"
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUICustomDraw : public GUIControl
	{
	public:
		struct ClassData;
		NN<Media::DrawEngine> eng;
		Optional<Media::ColorSess> colorSess;
	private:
		NN<IO::Library> lib;
		NN<ClassData> clsData;

		Bool focusing;
		UInt32 jsLastButtons;
		IntOS joystickId;


	private:
		static Int32 useCnt;
		static IntOS __stdcall FormWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
		static void Init(Optional<InstanceHandle> hInst);
		static void Deinit(Optional<InstanceHandle> hInst);
		void InitJS();

	public:
		GUICustomDraw(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Optional<Media::ColorSess> colorSess);
		virtual ~GUICustomDraw();

		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);

		virtual UI::EventState OnMouseDown(Math::Coord2D<IntOS> scnPos, MouseButton btn);
		virtual UI::EventState OnMouseUp(Math::Coord2D<IntOS> scnPos, MouseButton btn);
		virtual void OnMouseMove(Math::Coord2D<IntOS> scnPos);
		virtual UI::EventState OnMouseWheel(Math::Coord2D<IntOS> scnPos, Int32 delta);
		virtual void OnGestureBegin(Math::Coord2D<IntOS> scnPos, UInt64 dist);
		virtual void OnGestureStep(Math::Coord2D<IntOS> scnPos, UInt64 dist);
		virtual void OnGestureEnd(Math::Coord2D<IntOS> scnPos, UInt64 dist);
		virtual void OnJSButtonDown(IntOS buttonId);
		virtual void OnJSButtonUp(IntOS buttonId);
		virtual void OnJSAxis(IntOS axis1, IntOS axis2, IntOS axis3, IntOS axis4);
		virtual UI::EventState OnKeyDown(UI::GUIControl::GUIKey key); //true = handled

		virtual void OnTimerTick();
		virtual void OnDraw(NN<Media::DrawImage> img);
	};
}
#endif
