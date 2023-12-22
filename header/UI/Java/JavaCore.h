#ifndef _SM_UI_JAVA_JAVACORE
#define _SM_UI_JAVA_JAVACORE
#include "Sync/Event.h"
#include "UI/GUICore.h"

namespace UI
{
	namespace Java
	{
		class JavaCore : public GUICore
		{
		private:
			Bool toExit;
			Sync::Event waitEvt;
			Media::MonitorMgr *monMgr;
		public:
			JavaCore();
			virtual ~JavaCore();

			virtual void Run();
			virtual void ProcessMessages();
			virtual void WaitForMessages();
			virtual void Exit();
			virtual NotNullPtr<Media::DrawEngine> CreateDrawEngine();
			virtual Double GetMagnifyRatio(MonitorHandle *hMonitor);
			virtual void UseDevice(Bool useSystem, Bool useDisplay);
			virtual void SetNoDisplayOff(Bool noDispOff);
			virtual void DisplayOff();
			virtual void Suspend();
			virtual Math::Size2D<UOSInt> GetDesktopSize();
			virtual Math::Coord2D<OSInt> GetCursorPos();
			virtual void SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot);
			virtual void GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi);
			virtual void SetMonitorMgr(Media::MonitorMgr *monMgr);
			virtual Media::MonitorMgr *GetMonitorMgr();
			virtual Bool IsForwarded();

			virtual void ShowMsgOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			virtual Bool ShowMsgYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			virtual NotNullPtr<GUIButton> NewButton(NotNullPtr<GUIClientControl> parent, Text::CStringNN text);
			virtual NotNullPtr<GUICheckBox> NewCheckBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text, Bool checked);
			virtual NotNullPtr<GUICheckedListBox> NewCheckedListBox(NotNullPtr<GUIClientControl> parent);
			virtual NotNullPtr<GUIComboBox> NewComboBox(NotNullPtr<GUIClientControl> parent, Bool allowEdit);
			virtual NotNullPtr<GUIGroupBox> NewGroupBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text);
			virtual NotNullPtr<GUILabel> NewLabel(NotNullPtr<GUIClientControl> parent, Text::CStringNN text);
			virtual NotNullPtr<GUIListBox> NewListBox(NotNullPtr<GUIClientControl> parent, Bool multiSelect);
			virtual NotNullPtr<GUITabControl> NewTabControl(NotNullPtr<GUIClientControl> parent);
			virtual NotNullPtr<GUITextBox> NewTextBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN initText);
			virtual NotNullPtr<GUITextBox> NewTextBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN initText, Bool multiLine);
			virtual NotNullPtr<GUIHSplitter> NewHSplitter(NotNullPtr<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual NotNullPtr<GUIVSplitter> NewVSplitter(NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual NotNullPtr<GUIFileDialog> NewFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
			virtual NotNullPtr<GUIFolderDialog> NewFolderDialog();
			virtual NotNullPtr<GUIFontDialog> NewFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual NotNullPtr<GUIFontDialog> NewFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);

			virtual NotNullPtr<GUIPanelBase> NewPanelBase(NotNullPtr<UI::GUIPanel> master, ControlHandle *parentHWnd);
			virtual NotNullPtr<GUIPanelBase> NewPanelBase(NotNullPtr<UI::GUIPanel> master, NotNullPtr<UI::GUIClientControl> parent);
		};
	}
}
#endif
