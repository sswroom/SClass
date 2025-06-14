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

		NN<ClassData> clsData;
		Optional<UI::GUIForm> rootForm;
		Media::ImageCopy *imgCopy;

		Sync::Event drawEvt;
		Optional<MonitorHandle> surfaceMon;
		Optional<Media::MonitorSurface> primarySurface;
		Optional<Media::MonitorSurface> buffSurface;
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
		NN<Media::MonitorSurfaceMgr> surfaceMgr;
		Sync::Mutex surfaceMut;
		UInt32 bitDepth;
		OSInt scnX;
		OSInt scnY;
		UOSInt scnW;
		UOSInt scnH;
		Bool switching;
		Optional<MonitorHandle> currMon;
		ScreenMode currScnMode;
		ScreenMode fullScnMode;
		NN<IO::Library> lib;
		Media::RotateType rotType;

	private:
		static Int32 useCnt;
		static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(Optional<InstanceHandle> hInst);
		static void Deinit(Optional<InstanceHandle> hInst);
		static void JSTimerTick(AnyType userObj);
	public:
		static void __stdcall OnResized(AnyType userObj);

	protected:
		void GetDrawingRect(void *rc);
		virtual void OnPaint();
		Bool CreateSurface();
		void ReleaseSurface();
		void CreateSubSurface();
		void ReleaseSubSurface();

		UnsafeArrayOpt<UInt8> LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, OutParam<OSInt> bpl);
		void LockSurfaceEnd();
		Media::PixelFormat GetPixelFormat();

		virtual void BeginUpdateSize();
		virtual void EndUpdateSize();

	public:
		GUIDDrawControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool directMode, NN<Media::ColorManagerSess> colorSess);
		virtual ~GUIDDrawControl();

		void SetUserFSMode(ScreenMode fullScnMode);
		void DrawToScreen();
		void DisplayFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> drawSize, Bool clearScn);
		void SwitchFullScreen(Bool fullScn, Bool vfs);
		Bool IsFullScreen();
		virtual void ChangeMonitor(Optional<MonitorHandle> hMon);
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
		void UseDrawSurface(NN<Sync::MutexUsage> mut);
		void UnuseDrawSurface(NN<Sync::MutexUsage> mut);
	};
}
#endif
