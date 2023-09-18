#ifndef _SM_SSWR_AVIREAD_AVIRGSMMODEMFORM
#define _SM_SSWR_AVIREAD_AVIRGSMMODEMFORM
#include "IO/HuaweiGSMModemController.h"
#include "IO/LogTool.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRealtimeLineChart.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::GSMModemController *modem;
			IO::HuaweiGSMModemController *huawei;
			IO::ATCommandChannel *channel;
			IO::Stream *port;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;

			UI::GUIPanel *pnlDevice;
			UI::GUILabel *lblDeviceSerial;
			UI::GUIComboBox *cboDeviceSerial;
			UI::GUIButton *btnDeviceSerial;
			UI::GUILabel *lblDeviceStatus;
			UI::GUITextBox *txtDeviceStatus;
			UI::GUIButton *btnDeviceOther;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpInfo;
			UI::GUIPanel *pnlInfo;
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
			UI::GUIRealtimeLineChart *rlcRSSI;

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
			UI::GUIButton *btnPDPContextActiveAll;
			UI::GUIButton *btnPDPContextActiveSelected;
			UI::GUIButton *btnPDPContextDeactiveAll;
			UI::GUIButton *btnPDPContextDeactiveSelected;
			UI::GUIListView *lvPDPContext;

			UI::GUITabPage *tpATCommand;
			UI::GUIPanel *pnlATCommand;
			UI::GUILabel *lblATCommand;
			UI::GUITextBox *txtATCommand;
			UI::GUIButton *btnATCommand;
			UI::GUITextBox *txtATCommandRep;

			UI::GUITabPage *tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

			UI::GUITabPage *tpHuawei;
			UI::GUILabel *lblHuaweiICCID;
			UI::GUITextBox *txtHuaweiICCID;
			UI::GUILabel *lblHuaweiSIMType;
			UI::GUITextBox *txtHuaweiSIMType;
			UI::GUILabel *lblHuaweiSrvStatus;
			UI::GUITextBox *txtHuaweiSrvStatus;
			UI::GUILabel *lblHuaweiSrvDomain;
			UI::GUITextBox *txtHuaweiSrvDomain;
			UI::GUILabel *lblHuaweiRoamStatus;
			UI::GUITextBox *txtHuaweiRoamStatus;
			UI::GUILabel *lblHuaweiSIMState;
			UI::GUITextBox *txtHuaweiSIMState;
			UI::GUILabel *lblHuaweiLockState;
			UI::GUITextBox *txtHuaweiLockState;
			UI::GUILabel *lblHuaweiSysMode;
			UI::GUITextBox *txtHuaweiSysMode;
			UI::GUILabel *lblHuaweiSubMode;
			UI::GUITextBox *txtHuaweiSubMode;
			UI::GUILabel *lblHuaweiCSQ1;
			UI::GUITextBox *txtHuaweiCSQ1;
			UI::GUILabel *lblHuaweiCSQ2;
			UI::GUITextBox *txtHuaweiCSQ2;
			UI::GUILabel *lblHuaweiCSQ3;
			UI::GUITextBox *txtHuaweiCSQ3;
			UI::GUILabel *lblHuaweiCSQ4;
			UI::GUITextBox *txtHuaweiCSQ4;
			UI::GUIGroupBox *grpHuaweiDHCP;
			UI::GUIButton *btnHuaweiDHCP;
			UI::GUILabel *lblHuaweiDHCPClientIP;
			UI::GUITextBox *txtHuaweiDHCPClientIP;
			UI::GUILabel *lblHuaweiDHCPNetmask;
			UI::GUITextBox *txtHuaweiDHCPNetmask;
			UI::GUILabel *lblHuaweiDHCPGateway;
			UI::GUITextBox *txtHuaweiDHCPGateway;
			UI::GUILabel *lblHuaweiDHCPServer;
			UI::GUITextBox *txtHuaweiDHCPServer;
			UI::GUILabel *lblHuaweiDHCPPriDNS;
			UI::GUITextBox *txtHuaweiDHCPPriDNS;
			UI::GUILabel *lblHuaweiDHCPSecDNS;
			UI::GUITextBox *txtHuaweiDHCPSecDNS;
			UI::GUILabel *lblHuaweiDHCPMaxRXbps;
			UI::GUITextBox *txtHuaweiDHCPMaxRXbps;
			UI::GUILabel *lblHuaweiDHCPMaxTXbps;
			UI::GUITextBox *txtHuaweiDHCPMaxTXbps;

			Bool initStrs;
			Text::String *initModemManu;
			Text::String *initModemModel;
			Text::String *initModemVer;
			Text::String *initIMEI;
			Text::String *huaweiICCID;
			Bool huaweiSysInfoUpdated;
			IO::HuaweiGSMModemController::ServiceStatus huaweiSysInfoSrvStatus;
			IO::HuaweiGSMModemController::ServiceDomain huaweiSysInfoSrvDomain;
			Bool huaweiSysInfoRoamStatus;
			IO::HuaweiGSMModemController::SIMState huaweiSysInfoSIMState;
			Bool huaweiSysInfoLockState;
			IO::HuaweiGSMModemController::SysMode huaweiSysInfoSysMode;
			IO::HuaweiGSMModemController::SubMode huaweiSysInfoSubMode;
			IO::HuaweiGSMModemController::SIMCardType huaweiSIMType;
			Bool huaweiCSQUpdated;
			IO::HuaweiGSMModemController::SignalStrengthInfo huaweiCSQ;
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

			Bool signalUpdated;
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
			static void __stdcall OnPDPContextActiveAllClicked(void *userObj);
			static void __stdcall OnPDPContextDeactiveAllClicked(void *userObj);
			static void __stdcall OnPDPContextActiveSelectedClicked(void *userObj);
			static void __stdcall OnPDPContextDeactiveSelectedClicked(void *userObj);
			static void __stdcall OnHuaweiDHCPClicked(void *userObj);

			void LoadPhoneBook();
			void LoadSMS();
			void LoadPDPContext();
			void InitStream(NotNullPtr<IO::Stream> stm, Bool updateSerial);
			void CloseStream(Bool updateUI);
		public:
			AVIRGSMModemForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::Stream *port);
			virtual ~AVIRGSMModemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
