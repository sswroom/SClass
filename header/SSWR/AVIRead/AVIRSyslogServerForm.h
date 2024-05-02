#ifndef _SM_SSWR_AVIREAD_AVIRSYSLOGSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRSYSLOGSERVERFORM
#include "Data/ArrayListNN.h"
#include "Data/FastMapNN.h"
#include "Net/SyslogServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSyslogServerForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt32 ip;
				Data::ArrayListStringNN *logMessage;
			} IPLog;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::SyslogServer *svr;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIListBox> lbClient;
			NN<UI::GUIHSplitter> hspClient;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			UInt32 currIP;
			Bool ipListUpd;
			Bool msgListUpd;
			Sync::Mutex ipMut;
			Data::FastMapNN<UInt32, IPLog> ipMap;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnClientSelChg(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnClientLog(AnyType userObj, UInt32 ip, Text::CString message);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRSyslogServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSyslogServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
