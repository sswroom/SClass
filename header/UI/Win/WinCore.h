#ifndef _SM_UI_WIN_WINCORE
#define _SM_UI_WIN_WINCORE
#include "UI/GUICore.h"

namespace UI
{
	namespace Win
	{
		class WinCore : public UI::GUICore
		{
		private:
			Optional<InstanceHandle> hInst;
			Int32 frmCnt;
			Bool hasCommCtrl;
			Bool noDispOff;
			Optional<ControlHandle> focusWnd;
			void *focusHAcc;
			Optional<Media::MonitorMgr> monMgr;

		public:
			WinCore(Optional<InstanceHandle> hInst);
			virtual ~WinCore();

			virtual void Run();
			virtual void ProcessMessages();
			virtual void WaitForMessages();
			virtual void Exit();
			virtual NN<Media::DrawEngine> CreateDrawEngine();
			virtual Double GetMagnifyRatio(Optional<MonitorHandle> hMonitor);
			virtual void UseDevice(Bool useSystem, Bool useDisplay);
			virtual void SetNoDisplayOff(Bool noDispOff);
			virtual void DisplayOff();
			virtual void Suspend();
			virtual Math::Size2D<UIntOS> GetDesktopSize();
			virtual Math::Coord2D<IntOS> GetCursorPos();
			virtual void SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot);
			virtual void GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi);
			virtual void SetMonitorMgr(Optional<Media::MonitorMgr> monMgr);
			virtual Optional<Media::MonitorMgr> GetMonitorMgr();
			virtual Bool IsForwarded();
			virtual Int32 GetScrollBarSize();

			virtual void ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			virtual Bool ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			virtual NN<GUIButton> NewButton(NN<GUIClientControl> parent, Text::CStringNN text);
			virtual NN<GUICheckBox> NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked);
			virtual NN<GUICheckedListBox> NewCheckedListBox(NN<GUIClientControl> parent);
			virtual NN<GUIComboBox> NewComboBox(NN<GUIClientControl> parent, Bool allowEdit);
			virtual NN<GUIDateTimePicker> NewDateTimePicker(NN<UI::GUIClientControl> parent, Bool calendarSelect);
			virtual NN<GUIGroupBox> NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text);
			virtual NN<GUIHScrollBar> NewHScrollBar(NN<GUIClientControl> parent, Double width);
			virtual NN<GUILabel> NewLabel(NN<GUIClientControl> parent, Text::CStringNN text);
			virtual NN<GUIListBox> NewListBox(NN<GUIClientControl> parent, Bool multiSelect);
			virtual NN<GUIPictureBox> NewPictureBox(NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize);
			virtual NN<GUIPictureBoxSimple> NewPictureBoxSimple(NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder);
			virtual NN<GUIProgressBar> NewProgressBar(NN<UI::GUIClientControl> parent, UInt64 totalCnt);
			virtual NN<GUIRadioButton> NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected);
			virtual NN<GUIRealtimeLineChart> NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UIntOS lineCnt, UIntOS sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess);
			virtual NN<GUITabControl> NewTabControl(NN<GUIClientControl> parent);
			virtual NN<GUITextBox> NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText);
			virtual NN<GUITextBox> NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine);
			virtual NN<GUITrackBar> NewTrackBar(NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal);
			virtual NN<GUIHSplitter> NewHSplitter(NN<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual NN<GUIVSplitter> NewVSplitter(NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual NN<GUIFileDialog> NewFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave);
			virtual NN<GUIFolderDialog> NewFolderDialog();
			virtual NN<GUIFontDialog> NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual NN<GUIFontDialog> NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);

			virtual NN<GUIPanelBase> NewPanelBase(NN<UI::GUIPanel> master, Optional<ControlHandle> parentHWnd);
			virtual NN<GUIPanelBase> NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent);

			void SetFocusWnd(Optional<ControlHandle> hWnd, void *hAcc);
			Optional<InstanceHandle> GetHInst();

			static IntOS MSGetWindowObj(Optional<ControlHandle> hWnd, IntOS index);
			static IntOS MSSetWindowObj(Optional<ControlHandle> hWnd, IntOS index, IntOS value);
			static IntOS MSSetClassObj(Optional<ControlHandle> hWnd, IntOS index, IntOS value);
		};
	}
}
#endif
