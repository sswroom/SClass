#ifndef _SM_SSWR_AVIREAD_AVIRSYSLOGSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRSYSLOGSERVERFORM
#include "Data/ArrayListNN.h"
#include "Data/FastMap.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SyslogServer *svr;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIListBox> lbClient;
			NotNullPtr<UI::GUIHSplitter> hspClient;
			NotNullPtr<UI::GUITextBox> txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;

			UInt32 currIP;
			Bool ipListUpd;
			Bool msgListUpd;
			Sync::Mutex ipMut;
			Data::FastMap<UInt32, IPLog*> ipMap;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnClientSelChg(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnClientLog(void *userObj, UInt32 ip, Text::CString message);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRSyslogServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSyslogServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
