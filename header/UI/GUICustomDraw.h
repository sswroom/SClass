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
		Media::DrawEngine *eng;
	private:
		IO::Library *lib;
		void *clsData;

		Bool focusing;
		UInt32 jsLastButtons;
		OSInt joystickId;


	private:
		static Int32 useCnt;
		static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(void *hInst);
		static void Deinit(void *hInst);
		void InitJS();

	public:
		GUICustomDraw(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng);
		virtual ~GUICustomDraw();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual Bool OnMouseDown(OSInt scnX, OSInt scnY, MouseButton btn);
		virtual Bool OnMouseUp(OSInt scnX, OSInt scnY, MouseButton btn);
		virtual void OnMouseMove(OSInt scnX, OSInt scnY);
		virtual Bool OnMouseWheel(OSInt scnX, OSInt scnY, Int32 delta);
		virtual void OnGestureBegin(OSInt scnX, OSInt scnY, UInt64 dist);
		virtual void OnGestureStep(OSInt scnX, OSInt scnY, UInt64 dist);
		virtual void OnGestureEnd(OSInt scnX, OSInt scnY, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);
		virtual Bool OnKeyDown(UI::GUIControl::GUIKey key); //true = handled

		virtual void OnTimerTick();
		virtual void OnDraw(Media::DrawImage *img);
	};
};
#endif
