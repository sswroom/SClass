#ifndef _SM_UI_GUIDDRAWCONTROL
#define _SM_UI_GUIDDRAWCONTROL
#include "IO/Writer.h"
#include "IO/Library.h"
#include "Media/DDrawManager.h"
#include "Media/ImageCopy.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIForm;

	class GUIDDrawControl : public GUIControl
	{
	public:
		typedef enum
		{
			SM_WINDOWED,
			SM_VFS,
			SM_FS,
			SM_WINDOWED_DIR
		} ScreenMode;
	private:
		UI::GUIForm *rootForm;
		Media::ImageCopy *imgCopy;

		Sync::Event *drawEvt;
		Media::DDrawManager *surfaceMgr;
		MonitorHandle *surfaceMon;
		void *pSurface;
		Bool pSurfaceUpdated;
		void *surfaceBuff2;
		void *clipper;
		IO::Stream *debugFS;
		IO::Writer *debugWriter;
		UInt32 joystickId;
		UInt32 jsLastButtons;
		Bool focusing;
	protected:
		void *surfaceBuff;
		Bool surfaceNoRelease;
		Sync::Mutex *surfaceMut;
		UOSInt surfaceW;
		UOSInt surfaceH;
		UInt32 bitDepth;
		OSInt scnX;
		OSInt scnY;
		UOSInt scnW;
		UOSInt scnH;
		MonitorHandle *currMon;
		ScreenMode currScnMode;
		ScreenMode fullScnMode;
		Bool directMode;
		Bool switching;
		Bool inited;
		IO::Library *lib;

	private:
		static Int32 useCnt;
		static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(void *hInst);
		static void Deinit(void *hInst);
		static void JSTimerTick(void *userObj);
	public:
		static void __stdcall OnResized(void *userObj);

	protected:
		void GetDrawingRect(void *rc);
		virtual void OnPaint();
		Bool CreateSurface();
		void ReleaseSurface();
		void CreateSubSurface();
		void ReleaseSubSurface();
		Bool CreateClipper(void *lpDD);

		UInt8 *LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, UOSInt *bpl);
		void LockSurfaceEnd();
		UInt8 *LockSurfaceDirect(UOSInt *bpl);
		void LockSurfaceUnlock();
		Media::PixelFormat GetPixelFormat();

	public:
		GUIDDrawControl(GUICore *ui, UI::GUIClientControl *parent, Bool directMode, Media::ColorManagerSess *colorSess);
		virtual ~GUIDDrawControl();

		void SetUserFSMode(ScreenMode fullScnMode);
		void DrawToScreen();
		void DrawFromBuff(UInt8 *buff, OSInt lineAdd, OSInt tlx, OSInt tly, UOSInt drawW, UOSInt drawH, Bool clearScn);
		void SwitchFullScreen(Bool fullScn, Bool vfs);
		Bool IsFullScreen();
		virtual void ChangeMonitor(MonitorHandle *hMon);
		UInt32 GetRefreshRate();
	public:
		virtual void OnSurfaceCreated() = 0;
		virtual void OnMouseWheel(OSInt x, OSInt y, Int32 amount);
		virtual void OnMouseMove(OSInt x, OSInt y);
		virtual void OnMouseDown(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseUp(OSInt x, OSInt y, MouseButton button);
		virtual void OnMouseDblClick(OSInt x, OSInt y, MouseButton button);
		virtual void OnGZoomBegin(OSInt x, OSInt y, UInt64 dist);
		virtual void OnGZoomStep(OSInt x, OSInt y, UInt64 dist);
		virtual void OnGZoomEnd(OSInt x, OSInt y, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);

		void *GetSurface();
		void UseDrawSurface(Sync::MutexUsage *mut);
		void UnuseDrawSurface(Sync::MutexUsage *mut);
	};
}
#endif
