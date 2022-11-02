#ifndef _SM_SSWR_AVIREAD_AVIRGSMMODEMFORM
#define _SM_SSWR_AVIREAD_AVIRGSMMODEMFORM
#include "IO/GSMModemController.h"
#include "IO/LogTool.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGSMModemForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			IO::GSMModemController *modem;
			IO::ATCommandChannel *channel;
			IO::Stream *port;
			IO::LogTool log;
			UI::ListBoxLogger *logger;

			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpInfo;
			UI::GUITabPage *tpPhoneBook;
			UI::GUITabPage *tpSMS;

			UI::GUILabel *lblModemManu;
			UI::GUITextBox *txtModemManu;
			UI::GUILabel *lblModemModel;
			UI::GUITextBox *txtModemModel;
			UI::GUILabel *lblModemVer;
			UI::GUITextBox *txtModemVer;
			UI::GUILabel *lblModemIMEI;
			UI::GUITextBox *txtModemIMEI;
			UI::GUILabel *lblOperator;
			UI::GUITextBox *txtOperator;
			UI::GUILabel *lblSignalQuality;
			UI::GUITextBox *txtSignalQuality;

			UI::GUIPanel *pnlPhone;
			UI::GUIComboBox *cboPhoneStorage;
			UI::GUIButton *btnPhoneRead;
			UI::GUIListView *lvPhone;

			UI::GUIPanel *pnlSMS;
			UI::GUIComboBox *cboSMSStorage;
			UI::GUIButton *btnSMSRead;
			UI::GUILabel *lblSMSC;
			UI::GUITextBox *txtSMSC;
			UI::GUIButton *btnSMSSave;
			UI::GUIButton *btnSMSDelete;
			UI::GUIButton *btnSMSSaveAll;
			UI::GUIListView *lvSMS;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

			Bool initStrs;
			Text::String *initModemManu;
			Text::String *initModemModel;
			Text::String *initModemVer;
			Text::String *initIMEI;
			Data::ArrayList<IO::GSMModemController::SMSMessage *> msgList;

			IO::GSMModemController::RSSI signalQuality;
			Bool operUpdated;
			Text::String *operName;
			Data::DateTime operNextTime;

			Bool toStop;
			Bool running;
			Sync::Event modemEvt;

			static UInt32 __stdcall ModemThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPhoneRClick(void *userObj);
			static void __stdcall OnSMSRClick(void *userObj);
			static void __stdcall OnSMSSaveClick(void *userObj);
			static void __stdcall OnSMSDeleteClick(void *userObj);
			static void __stdcall OnSMSSaveAllClick(void *userObj);

			void LoadPhoneBook();
			void LoadSMS();
			void InitStream(IO::Stream *stm);
			void CloseStream();
		public:
			AVIRGSMModemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Stream *port);
			virtual ~AVIRGSMModemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
