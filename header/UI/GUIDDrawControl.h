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
		struct ClassData;

		ClassData *clsData;
		UI::GUIForm *rootForm;
		Media::ImageCopy *imgCopy;

		Sync::Event drawEvt;
		MonitorHandle *surfaceMon;
		Media::MonitorSurface *primarySurface;
		Media::MonitorSurface *buffSurface;
		IO::Stream *debugFS;
		IO::Writer *debugWriter;
		UInt32 joystickId;
		UInt32 jsLastButtons;
		Bool focusing;
		Bool directMode;
		Bool inited;
	protected:
		Math::Size2D<UOSInt> bkBuffSize;
		Math::Size2D<UOSInt> dispSize;
		NotNullPtr<Media::MonitorSurfaceMgr> surfaceMgr;
		Sync::Mutex surfaceMut;
		UInt32 bitDepth;
		OSInt scnX;
		OSInt scnY;
		UOSInt scnW;
		UOSInt scnH;
		Bool switching;
		MonitorHandle *currMon;
		ScreenMode currScnMode;
		ScreenMode fullScnMode;
		IO::Library *lib;
		Media::RotateType rotType;

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

		UInt8 *LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, UOSInt *bpl);
		void LockSurfaceEnd();
		Media::PixelFormat GetPixelFormat();

		virtual void BeginUpdateSize();
		virtual void EndUpdateSize();

	public:
		GUIDDrawControl(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Bool directMode, NotNullPtr<Media::ColorManagerSess> colorSess);
		virtual ~GUIDDrawControl();

		void SetUserFSMode(ScreenMode fullScnMode);
		void DrawToScreen();
		void DisplayFromSurface(NotNullPtr<Media::MonitorSurface> surface, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> drawSize, Bool clearScn);
		void SwitchFullScreen(Bool fullScn, Bool vfs);
		Bool IsFullScreen();
		virtual void ChangeMonitor(MonitorHandle *hMon);
		UInt32 GetRefreshRate();
		Bool IsSurfaceReady();
		void SetRotateType(Media::RotateType rotType);
		Media::RotateType GetRotateType() const;
	public:
		virtual void OnSurfaceCreated() = 0;
		virtual void OnMouseWheel(Math::Coord2D<OSInt> pos, Int32 amount);
		virtual void OnMouseMove(Math::Coord2D<OSInt> pos);
		virtual void OnMouseDown(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnMouseUp(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnMouseDblClick(Math::Coord2D<OSInt> pos, MouseButton button);
		virtual void OnGZoomBegin(Math::Coord2D<OSInt> pos, UInt64 dist);
		virtual void OnGZoomStep(Math::Coord2D<OSInt> pos, UInt64 dist);
		virtual void OnGZoomEnd(Math::Coord2D<OSInt> pos, UInt64 dist);
		virtual void OnJSButtonDown(OSInt buttonId);
		virtual void OnJSButtonUp(OSInt buttonId);
		virtual void OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4);

		void *GetPixBuf();
		void UseDrawSurface(NotNullPtr<Sync::MutexUsage> mut);
		void UnuseDrawSurface(NotNullPtr<Sync::MutexUsage> mut);
	};
}
#endif
