#ifndef _SM_UI_GTK_GTKCORE
#define _SM_UI_GTK_GTKCORE
#include "UI/GUICore.h"

namespace UI
{
	namespace GTK
	{
		class GTKCore : public GUICore
		{
		private:
			Media::MonitorMgr *monMgr;
		public:
			GTKCore();
			virtual ~GTKCore();

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
			virtual Math::Size2D<UOSInt> GetDesktopSize();
			virtual Math::Coord2D<OSInt> GetCursorPos();
			virtual void SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot);
			virtual void GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi);
			virtual void SetMonitorMgr(Media::MonitorMgr *monMgr);
			virtual Media::MonitorMgr *GetMonitorMgr();
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
			virtual NN<GUIRealtimeLineChart> NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS);
			virtual NN<GUITabControl> NewTabControl(NN<GUIClientControl> parent);
			virtual NN<GUITextBox> NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText);
			virtual NN<GUITextBox> NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine);
			virtual NN<GUITrackBar> NewTrackBar(NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal);
			virtual NN<GUIHSplitter> NewHSplitter(NN<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual NN<GUIVSplitter> NewVSplitter(NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual NN<GUIFileDialog> NewFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
			virtual NN<GUIFolderDialog> NewFolderDialog();
			virtual NN<GUIFontDialog> NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual NN<GUIFontDialog> NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);

			virtual NN<GUIPanelBase> NewPanelBase(NN<UI::GUIPanel> master, ControlHandle *parentHWnd);
			virtual NN<GUIPanelBase> NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent);
		};
	}
}
#endif
