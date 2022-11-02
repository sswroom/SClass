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

			UI::GUIPanel *pnlDevice;
			UI::GUILabel *lblDeviceSerial;
			UI::GUIComboBox *cboDeviceSerial;
			UI::GUIButton *btnDeviceSerial;
			UI::GUILabel *lblDeviceStatus;
			UI::GUITextBox *txtDeviceStatus;
			UI::GUIButton *btnDeviceOther;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpInfo;
			UI::GUILabel *lblModemManu;
			UI::GUITextBox *txtModemManu;
			UI::GUILabel *lblModemModel;
			UI::GUITextBox *txtModemModel;
			UI::GUILabel *lblModemVer;
			UI::GUITextBox *txtModemVer;
			UI::GUILabel *lblModemIMEI;
			UI::GUITextBox *txtModemIMEI;
			UI::GUILabel *lblIMSI;
			UI::GUITextBox *txtIMSI;
			UI::GUILabel *lblTECharset;
			UI::GUITextBox *txtTECharset;
			UI::GUILabel *lblOperator;
			UI::GUITextBox *txtOperator;
			UI::GUILabel *lblRegStatus;
			UI::GUITextBox *txtRegStatus;
			UI::GUILabel *lblLAC;
			UI::GUITextBox *txtLAC;
			UI::GUILabel *lblCI;
			UI::GUITextBox *txtCI;
			UI::GUILabel *lblACT;
			UI::GUITextBox *txtACT;
			UI::GUILabel *lblSignalQuality;
			UI::GUITextBox *txtSignalQuality;

			UI::GUITabPage *tpPhoneBook;
			UI::GUIPanel *pnlPhone;
			UI::GUIComboBox *cboPhoneStorage;
			UI::GUIButton *btnPhoneRead;
			UI::GUILabel *lblPhoneStatus;
			UI::GUIListView *lvPhone;

			UI::GUITabPage *tpSMS;
			UI::GUIPanel *pnlSMS;
			UI::GUIComboBox *cboSMSStorage;
			UI::GUIButton *btnSMSRead;
			UI::GUILabel *lblSMSC;
			UI::GUITextBox *txtSMSC;
			UI::GUIButton *btnSMSSave;
			UI::GUIButton *btnSMSDelete;
			UI::GUIButton *btnSMSSaveAll;
			UI::GUIListView *lvSMS;

			UI::GUITabPage *tpPDPContext;
			UI::GUIPanel *pnlPDPContext;
			UI::GUIButton *btnPDPContextLoad;
			UI::GUILabel *lblPDPContextStatus;
			UI::GUITextBox *txtPDPContextStatus;
			UI::GUIButton *btnPDPAttach;
			UI::GUIButton *btnPDPDetach;
			UI::GUILabel *lblPDPCID;
			UI::GUITextBox *txtPDPCID;
			UI::GUILabel *lblPDPType;
			UI::GUITextBox *txtPDPType;
			UI::GUILabel *lblPDPAPN;
			UI::GUITextBox *txtPDPAPN;
			UI::GUIButton *btnPDPContextSet;
			UI::GUIListView *lvPDPContext;

			UI::GUITabPage *tpATCommand;
			UI::GUIPanel *pnlATCommand;
			UI::GUILabel *lblATCommand;
			UI::GUITextBox *txtATCommand;
			UI::GUIButton *btnATCommand;
			UI::GUITextBox *txtATCommandRep;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

			Bool initStrs;
			Text::String *initModemManu;
			Text::String *initModemModel;
			Text::String *initModemVer;
			Text::String *initIMEI;
			Bool simChanged;
			Bool simInfoUpdated;
			Text::String *simIMSI;
			Bool cfgTECharsetUpd;
			Text::String *cfgTECharset;
			Bool regNetUpdated;
			IO::GSMModemController::NetworkResult regNetN;
			IO::GSMModemController::RegisterStatus regNetStat;
			UInt16 regNetLAC;
			UInt32 regNetCI;
			IO::GSMModemController::AccessTech regNetACT;
			Data::ArrayList<IO::GSMModemController::SMSMessage *> msgList;

			IO::GSMModemController::RSSI signalQuality;
			Bool operUpdated;
			Text::String *operName;
			Data::Timestamp operNextTime;

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
			static void __stdcall OnDeviceSerialClk(void *userObj);
			static void __stdcall OnDeviceOtherClk(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnATCommandClicked(void *userObj);
			static Bool __stdcall OnATCommandKeyDown(void *userObj, UInt32 osKey);
			static void __stdcall OnPDPContextLoadClicked(void *userObj);
			static void __stdcall OnPDPAttachClicked(void *userObj);
			static void __stdcall OnPDPDetachClicked(void *userObj);
			static void __stdcall OnPDPContextSetClicked(void *userObj);

			void LoadPhoneBook();
			void LoadSMS();
			void LoadPDPContext();
			void InitStream(IO::Stream *stm, Bool updateSerial);
			void CloseStream(Bool updateUI);
		public:
			AVIRGSMModemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Stream *port);
			virtual ~AVIRGSMModemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
