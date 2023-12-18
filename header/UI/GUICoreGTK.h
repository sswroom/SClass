#ifndef _SM_UI_GUICOREGTK
#define _SM_UI_GUICOREGTK
#include "UI/GUICore.h"

namespace UI
{
	class GUICoreGTK : public GUICore
	{
	private:
		Media::MonitorMgr *monMgr;
	public:
		GUICoreGTK();
		virtual ~GUICoreGTK();

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
		virtual NotNullPtr<GUIComboBox> NewComboBox(NotNullPtr<GUIClientControl> parent, Bool allowEdit);
		virtual NotNullPtr<GUIGroupBox> NewGroupBox(NotNullPtr<GUIClientControl> parent, Text::CStringNN text);
		virtual NotNullPtr<GUILabel> NewLabel(NotNullPtr<GUIClientControl> parent, Text::CStringNN text);
		virtual NotNullPtr<GUIHSplitter> NewHSplitter(NotNullPtr<UI::GUIClientControl> parent, Int32 width, Bool isRight);
		virtual NotNullPtr<GUIVSplitter> NewVSplitter(NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
	};
}
#endif
