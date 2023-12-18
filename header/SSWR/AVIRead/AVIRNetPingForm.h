#ifndef _SM_SSWR_AVIREAD_AVIRNETPINGFORM
#define _SM_SSWR_AVIREAD_AVIRNETPINGFORM

#include "IO/LogTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetPingForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SocketUtil::AddressInfo targetAddr;
			IO::LogTool *log;
			NotNullPtr<UI::ListBoxLogger> logger;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			UI::GUIVSplitter *vspRequest;
			UI::GUILabel *lblTarget;
			UI::GUITextBox *txtTarget;
			UI::GUILabel *lblRepeat;
			UI::GUICheckBox *chkRepeat;
			NotNullPtr<UI::GUIButton> btnPing;
			UI::GUIRealtimeLineChart *rlcPing;
			UI::GUIVSplitter *vspPing;
			NotNullPtr<UI::GUIListBox> lbLog;

			static void __stdcall OnPingClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRNetPingForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetPingForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
