#ifndef _SM_UI_GUICORE
#define _SM_UI_GUICORE
#include "Math/Coord2D.h"
#include "Math/Size2D.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"

namespace UI
{
	typedef void (__stdcall *UIEvent)(void *userObj);
	typedef Bool (__stdcall *KeyEvent)(void *userObj, UInt32 osKey);

	class GUIButton;
	class GUICheckBox;
	class GUICheckedListBox;
	class GUIClientControl;
	class GUIComboBox;
	class GUIControl;
	class GUIFileDialog;
	class GUIFolderDialog;
	class GUIFontDialog;
	class GUIGroupBox;
	class GUIHSplitter;
	class GUILabel;
	class GUITabControl;
	class GUITextBox;
	class GUIVSplitter;
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
		GUICore() {};
		virtual ~GUICore(){};

		virtual void Run() = 0;
		virtual void ProcessMessages() = 0;
		virtual void WaitForMessages() = 0;
		virtual void Exit() = 0;
		virtual NotNullPtr<Media::DrawEngine> CreateDrawEngine() = 0;
		virtual Double GetMagnifyRatio(MonitorHandle *hMonitor) = 0;
		virtual void UseDevice(Bool useSystem, Bool useDisplay) = 0;
		virtual void SetNoDisplayOff(Bool noDispOff) = 0;
		virtual void DisplayOff() = 0;
		virtual void Suspend() = 0;
		virtual Math::Size2D<UOSInt> GetDesktopSize() = 0;
		virtual Math::Coord2D<OSInt> GetCursorPos() = 0;
		virtual void SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot) = 0;
		virtual void GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi) = 0;
		virtual void SetMonitorMgr(Media::MonitorMgr *monMgr) = 0;
		virtual Media::MonitorMgr *GetMonitorMgr() = 0;
		virtual Bool IsForwarded() = 0;

		virtual void ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl) = 0;
		virtual Bool ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl) = 0;
		virtual NotNullPtr<GUIButton> NewButton(NotNullPtr<GUIClientControl> parent, Text::CStringNN text) = 0;
		virtual NotNullPtr<GUICheckBox> NewCheckBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text, Bool checked) = 0;
		virtual NotNullPtr<GUICheckedListBox> NewCheckedListBox(NotNullPtr<GUIClientControl> parent) = 0;
		virtual NotNullPtr<GUIComboBox> NewComboBox(NotNullPtr<GUIClientControl> parent, Bool allowEdit) = 0;
		virtual NotNullPtr<GUIGroupBox> NewGroupBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text) = 0;
		virtual NotNullPtr<GUILabel> NewLabel(NotNullPtr<GUIClientControl> parent, Text::CStringNN text) = 0;
		virtual NotNullPtr<GUITabControl> NewTabControl(NotNullPtr<GUIClientControl> parent) = 0;
		virtual NotNullPtr<GUITextBox> NewTextBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN initText) = 0;
		virtual NotNullPtr<GUITextBox> NewTextBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine) = 0;
		virtual NotNullPtr<GUIHSplitter> NewHSplitter(NotNullPtr<UI::GUIClientControl> parent, Int32 width, Bool isRight) = 0;
		virtual NotNullPtr<GUIVSplitter> NewVSplitter(NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom) = 0;
		virtual NotNullPtr<GUIFileDialog> NewFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave) = 0;
		virtual NotNullPtr<GUIFolderDialog> NewFolderDialog() = 0;
		virtual NotNullPtr<GUIFontDialog> NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) = 0;
		virtual NotNullPtr<GUIFontDialog> NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) = 0;
	};
}
#endif
