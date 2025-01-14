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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::SocketUtil::AddressInfo targetAddr;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUIVSplitter> vspRequest;
			NN<UI::GUILabel> lblTarget;
			NN<UI::GUITextBox> txtTarget;
			NN<UI::GUILabel> lblRepeat;
			NN<UI::GUICheckBox> chkRepeat;
			NN<UI::GUIButton> btnPing;
			NN<UI::GUIRealtimeLineChart> rlcPing;
			NN<UI::GUIVSplitter> vspPing;
			NN<UI::GUIListBox> lbLog;

			static void __stdcall OnPingClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRNetPingForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetPingForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
