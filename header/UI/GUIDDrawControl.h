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
		Math::Size2D<UIntOS> bkBuffSize;
		Math::Size2D<UIntOS> dispSize;
		NN<Media::MonitorSurfaceMgr> surfaceMgr;
		Sync::Mutex surfaceMut;
		UInt32 bitDepth;
		IntOS scnX;
		IntOS scnY;
		UIntOS scnW;
		UIntOS scnH;
		Bool switching;
		Optional<MonitorHandle> currMon;
		ScreenMode currScnMode;
		ScreenMode fullScnMode;
		NN<IO::Library> lib;
		Media::RotateType rotType;

	private:
		static Int32 useCnt;
		static IntOS __stdcall FormWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
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

		UnsafeArrayOpt<UInt8> LockSurfaceBegin(UIntOS targetWidth, UIntOS targetHeight, OutParam<IntOS> bpl);
		void LockSurfaceEnd();
		Media::PixelFormat GetPixelFormat();

		virtual void BeginUpdateSize();
		virtual void EndUpdateSize();

	public:
		GUIDDrawControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool directMode, NN<Media::ColorManagerSess> colorSess);
		virtual ~GUIDDrawControl();

		void SetUserFSMode(ScreenMode fullScnMode);
		void DrawToScreen();
		void DisplayFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> drawSize, Bool clearScn);
		void SwitchFullScreen(Bool fullScn, Bool vfs);
		Bool IsFullScreen();
		virtual void ChangeMonitor(Optional<MonitorHandle> hMon);
		UInt32 GetRefreshRate();
		Bool IsSurfaceReady();
		void SetRotateType(Media::RotateType rotType);
		Media::RotateType GetRotateType() const;
	public:
		virtual void OnSurfaceCreated() = 0;
		virtual void OnMouseWheel(Math::Coord2D<IntOS> pos, Int32 amount);
		virtual void OnMouseMove(Math::Coord2D<IntOS> pos);
		virtual void OnMouseDown(Math::Coord2D<IntOS> pos, MouseButton button);
		virtual void OnMouseUp(Math::Coord2D<IntOS> pos, MouseButton button);
		virtual void OnMouseDblClick(Math::Coord2D<IntOS> pos, MouseButton button);
		virtual void OnGZoomBegin(Math::Coord2D<IntOS> pos, UInt64 dist);
		virtual void OnGZoomStep(Math::Coord2D<IntOS> pos, UInt64 dist);
		virtual void OnGZoomEnd(Math::Coord2D<IntOS> pos, UInt64 dist);
		virtual void OnJSButtonDown(IntOS buttonId);
		virtual void OnJSButtonUp(IntOS buttonId);
		virtual void OnJSAxis(IntOS axis1, IntOS axis2, IntOS axis3, IntOS axis4);

		void *GetPixBuf();
		void UseDrawSurface(NN<Sync::MutexUsage> mut);
		void UnuseDrawSurface(NN<Sync::MutexUsage> mut);
	};
}
#endif
