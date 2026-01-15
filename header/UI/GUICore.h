#ifndef _SM_UI_GUICORE
#define _SM_UI_GUICORE
#include "AnyType.h"
#include "Math/Coord2D.h"
#include "Math/Size2D.h"
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"

namespace UI
{
	enum class EventState
	{
		ContinueEvent,
		StopEvent
	};

	typedef void (CALLBACKFUNC UIEvent)(AnyType userObj);
	typedef UI::EventState (CALLBACKFUNC KeyEvent)(AnyType userObj, UInt32 osKey);

	struct InstanceHandle;
	
	class GUIButton;
	class GUICheckBox;
	class GUICheckedListBox;
	class GUIClientControl;
	class GUIComboBox;
	class GUIControl;
	class GUIDateTimePicker;
	class GUIFileDialog;
	class GUIFolderDialog;
	class GUIFontDialog;
	class GUIGroupBox;
	class GUIHScrollBar;
	class GUIHSplitter;
	class GUILabel;
	class GUIListBox;
	class GUIListView;
	class GUIPanel;
	class GUIPanelBase;
	class GUIPictureBox;
	class GUIPictureBoxDD;
	class GUIPictureBoxSimple;
	class GUIProgressBar;
	class GUIRadioButton;
	class GUIRealtimeLineChart;
	class GUITabControl;
	class GUITextBox;
	class GUITrackBar;
	class GUIVSplitter;

	enum class ListViewStyle
	{
		Icon,
		List,
		Table,
		SmallIcon
	};

	class GUICore
	{
	public:
		
		typedef enum
		{
			DR_NONE,
			DR_LANDSCAPE,
			DR_PORT
		} DisplayRotation;
	public:
		virtual ~GUICore();

		virtual void Run() = 0;
		virtual void ProcessMessages() = 0;
		virtual void WaitForMessages() = 0;
		virtual void Exit() = 0;
		virtual NN<Media::DrawEngine> CreateDrawEngine() = 0;
		virtual Double GetMagnifyRatio(Optional<MonitorHandle> hMonitor) = 0;
		virtual void UseDevice(Bool useSystem, Bool useDisplay) = 0;
		virtual void SetNoDisplayOff(Bool noDispOff) = 0;
		virtual void DisplayOff() = 0;
		virtual void Suspend() = 0;
		virtual Math::Size2D<UIntOS> GetDesktopSize() = 0;
		virtual Math::Coord2D<IntOS> GetCursorPos() = 0;
		virtual void SetDisplayRotate(Optional<MonitorHandle> hMonitor, DisplayRotation rot) = 0;
		virtual void GetMonitorDPIs(Optional<MonitorHandle> hMonitor, OutParam<Double> hdpi, OutParam<Double> ddpi) = 0;
		virtual void SetMonitorMgr(Optional<Media::MonitorMgr> monMgr) = 0;
		virtual Optional<Media::MonitorMgr> GetMonitorMgr() = 0;
		virtual Bool IsForwarded() = 0;
		virtual Int32 GetScrollBarSize() = 0;

		virtual void ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl) = 0;
		virtual Bool ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl) = 0;
		virtual NN<GUIButton> NewButton(NN<GUIClientControl> parent, Text::CStringNN text) = 0;
		virtual NN<GUICheckBox> NewCheckBox(NN<GUIClientControl> parent, Text::CStringNN text, Bool checked) = 0;
		virtual NN<GUICheckedListBox> NewCheckedListBox(NN<GUIClientControl> parent) = 0;
		virtual NN<GUIComboBox> NewComboBox(NN<GUIClientControl> parent, Bool allowEdit) = 0;
		virtual NN<GUIDateTimePicker> NewDateTimePicker(NN<UI::GUIClientControl> parent, Bool calendarSelect) = 0;
		virtual NN<GUIGroupBox> NewGroupBox(NN<GUIClientControl> parent, Text::CStringNN text) = 0;
		virtual NN<GUIHScrollBar> NewHScrollBar(NN<GUIClientControl> parent, Double width) = 0;
		virtual NN<GUILabel> NewLabel(NN<GUIClientControl> parent, Text::CStringNN text) = 0;
		virtual NN<GUIListBox> NewListBox(NN<GUIClientControl> parent, Bool multiSelect) = 0;
		virtual NN<GUIPictureBox> NewPictureBox(NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) = 0;
		virtual NN<GUIPictureBoxSimple> NewPictureBoxSimple(NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder) = 0;
		virtual NN<GUIProgressBar> NewProgressBar(NN<UI::GUIClientControl> parent, UInt64 totalCnt) = 0;
		virtual NN<GUIRadioButton> NewRadioButton(NN<GUIClientControl> parent, Text::CStringNN initText, Bool selected) = 0;
		virtual NN<GUIRealtimeLineChart> NewRealtimeLineChart(NN<GUIClientControl> parent, NN<Media::DrawEngine> eng, UIntOS lineCnt, UIntOS sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess) = 0;
		virtual NN<GUITabControl> NewTabControl(NN<GUIClientControl> parent) = 0;
		virtual NN<GUITextBox> NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText) = 0;
		virtual NN<GUITextBox> NewTextBox(NN<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine) = 0;
		virtual NN<GUITrackBar> NewTrackBar(NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal) = 0;
		virtual NN<GUIHSplitter> NewHSplitter(NN<UI::GUIClientControl> parent, Int32 width, Bool isRight) = 0;
		virtual NN<GUIVSplitter> NewVSplitter(NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom) = 0;
		virtual NN<GUIFileDialog> NewFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave) = 0;
		virtual NN<GUIFolderDialog> NewFolderDialog() = 0;
		virtual NN<GUIFontDialog> NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) = 0;
		virtual NN<GUIFontDialog> NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) = 0;

		virtual NN<GUIPanelBase> NewPanelBase(NN<UI::GUIPanel> master, Optional<ControlHandle> parentHWnd) = 0;
		virtual NN<GUIPanelBase> NewPanelBase(NN<UI::GUIPanel> master, NN<UI::GUIClientControl> parent) = 0;

		NN<GUIListView> NewListView(NN<UI::GUIClientControl> parent, ListViewStyle lvstyle, UIntOS colCount);
		NN<GUIPanel> NewPanel(NN<UI::GUIClientControl> parent);
		NN<GUIPictureBoxDD> NewPictureBoxDD(NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, Bool allowEnlarge, Bool directMode);
	};
}
#endif
