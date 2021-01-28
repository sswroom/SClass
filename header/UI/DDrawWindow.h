#ifndef _SM_UI_DDRAWWINDOW
#define _SM_UI_DDRAWWINDOW
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Media/ImageCopy.h"
#include "Sync/Event.h"

namespace UI
{
	class DDrawUI;

	class DDrawWindow
	{
	public:
		typedef enum
		{
			UIM_NORMAL,
			UIM_FIXSIZE,
			UIM_ARATIO,
			UIM_DYNAMIC
		} UIMode;

		typedef enum
		{
			UPM_ONUPDATE,
			UPM_ALWAYS
		} UpdateMode;

		typedef struct
		{
			Int32 width;
			Int32 height;
			Int32 bpl;
			UInt8 *buffPtr;
		} AccessStatus;

		typedef enum
		{
			MBTN_LEFT = 1,
			MBTN_RIGHT = 2,
			MBTN_MIDDLE = 4,
			MBTN_X1 = 8,
			MBTN_X2 = 16
		} MouseButton;

		typedef void (__stdcall *UpdateHdlr)(UInt8 *lyrPtr, Int32 width, Int32 height, Int32 bpl, Int32 frameId);
		typedef void (__stdcall *MouseHdlr)(Int32 x, Int32 y, MouseButton button);
		typedef void (__stdcall *MouseWheelHdlr)(OSInt x, OSInt y, Int32 delta);
		typedef void (__stdcall *SizeChgHdlr)(Int32 newWidth, Int32 newHeight);
		typedef void (__stdcall *KeyInputHdlr)(Int32 keyCode);
		typedef void (__stdcall *ActionHdlr)();

	private:
		void *hWnd;
		UI::DDrawUI *ui;
		UI::DDrawWindow::UIMode mode;
		UI::DDrawWindow::UpdateMode upMode;
		UI::DDrawWindow::UpdateHdlr hdlr;
		UI::DDrawWindow::MouseHdlr mouseDownHdlr;
		UI::DDrawWindow::MouseHdlr mouseUpHdlr;
		UI::DDrawWindow::MouseHdlr mouseMoveHdlr;
		UI::DDrawWindow::MouseWheelHdlr mouseWheelHdlr;
		UI::DDrawWindow::KeyInputHdlr keyDownHdlr;
		UI::DDrawWindow::KeyInputHdlr keyUpHdlr;
		UI::DDrawWindow::SizeChgHdlr szChgHdlr;
		UI::DDrawWindow::ActionHdlr closeHdlr;

		Media::Resizer::LanczosResizerH8_8 *resizer;
		Media::ImageCopy *imgCopy;
		Bool quitOnClose;
		void *lpDD;
		void *pcClipper;
		Int32 bufferCnt;
		Int32 lastMouseBtn;

		Bool isFullScn;
		Bool isVisible;
		Int32 scnWidth;
		Int32 scnHeight;
		Int32 scnRate;
		Int32 frameId;
		Int32 initW;
		Int32 initH;
		Int32 targetW;
		Int32 targetH;
		Int32 fsWinX;
		Int32 fsWinY;
		Int32 fsWinW;
		Int32 fsWinH;

		Int32 secSurfW;
		Int32 secSurfH;
		Bool fastUI;

		void *primarySurface;
		void *fsbackSurface;
		void *secondarySurface1;
		void *secondarySurface2;

		Bool presentToStop;
		Bool presentNeeded;
		Bool presentBusy;
		Bool presentRunning;
		Int32 presentIndex;
		Sync::Event *presentEvt;

		static UInt32 __stdcall PresentThread(void *obj);
		Int32 CreatePrimarySurface();
		void DestroyPrimarySurface();
		Int32 CreateSecondarySurface();
		void DestroySecondarySurface();

		void UpdateSurface();
	public:
		DDrawWindow(void *hWnd, UI::DDrawUI *ui, UI::DDrawWindow::UIMode mode, UI::DDrawWindow::UpdateMode upMode, UI::DDrawWindow::UpdateHdlr hdlr, Bool fastUI);
		~DDrawWindow();

		void Show();
		void SwitchFullscreen(Bool fullScreen);
		OSInt WndProc(UInt32 msg, UInt32 wParam, Int32 lParam);
		void SetQuitOnClose(Bool quitOnClose);
		void *GetHWnd();
		void Redraw();
		void SetMouseDownHdlr(MouseHdlr hdlr);
		void SetMouseUpHdlr(MouseHdlr hdlr);
		void SetMouseMoveHdlr(MouseHdlr hdlr);
		void SetMouseWheelHdlr(MouseWheelHdlr hdlr);
		void SetKeyDownHdlr(KeyInputHdlr hdlr);
		void SetKeyUpHdlr(KeyInputHdlr hdlr);
		void SetSizeChangeHdlr(SizeChgHdlr hdlr);
		void SetCloseHdlr(ActionHdlr hdlr);

		void BeginAccessBuff(AccessStatus *status);
		void EndAccessBuff();
		void PresentBuff();
	};
};

#endif
