#ifndef _SM_SSWR_AVIREAD_AVIRLOGSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRLOGSERVERFORM
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
			typedef struct
			{
				UInt32 ip;
				Data::ArrayList<Text::String*> *logMessage;
			} IPLog;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::LogServer *svr;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStart;
			UI::GUIListBox *lbClient;
			UI::GUIHSplitter *hspClient;
			UI::GUITextBox *txtLog;
			UI::GUIListBox *lbLog;

			UInt32 currIP;
			Bool ipListUpd;
			Bool msgListUpd;
			Sync::Mutex *ipMut;
			Data::UInt32Map<IPLog*> *ipMap;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnClientSelChg(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnClientLog(void *userObj, UInt32 ip, Text::CString message);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRLogServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRLogServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
