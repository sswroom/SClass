#ifndef _SM_SSWR_AVIREAD_AVIRLOGSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRLOGSERVERFORM
#include "Data/FastMap.h"
#include "Net/LogServer.h"
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
		class AVIRLogServerForm : public UI::GUIForm
		{
		private:
			struct IPLog
			{
				UInt32 ip;
				Data::ArrayListStringNN logMessage;
			};
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::LogServer *svr;

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
			IO::LogTool log;
			Data::FastMap<UInt32, IPLog*> ipMap;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnClientSelChg(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnClientLog(AnyType userObj, UInt32 ip, Text::CString message);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRLogServerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
