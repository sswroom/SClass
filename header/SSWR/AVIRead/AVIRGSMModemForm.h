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

			NotNullPtr<UI::GUIPanel> pnlDevice;
			NotNullPtr<UI::GUILabel> lblDeviceSerial;
			NotNullPtr<UI::GUIComboBox> cboDeviceSerial;
			NotNullPtr<UI::GUIButton> btnDeviceSerial;
			NotNullPtr<UI::GUILabel> lblDeviceStatus;
			UI::GUITextBox *txtDeviceStatus;
			NotNullPtr<UI::GUIButton> btnDeviceOther;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUIPanel> pnlInfo;
			NotNullPtr<UI::GUILabel> lblModemManu;
			UI::GUITextBox *txtModemManu;
			NotNullPtr<UI::GUILabel> lblModemModel;
			UI::GUITextBox *txtModemModel;
			NotNullPtr<UI::GUILabel> lblModemVer;
			UI::GUITextBox *txtModemVer;
			NotNullPtr<UI::GUILabel> lblModemIMEI;
			UI::GUITextBox *txtModemIMEI;
			NotNullPtr<UI::GUILabel> lblIMSI;
			UI::GUITextBox *txtIMSI;
			NotNullPtr<UI::GUILabel> lblTECharset;
			UI::GUITextBox *txtTECharset;
			NotNullPtr<UI::GUILabel> lblOperator;
			UI::GUITextBox *txtOperator;
			NotNullPtr<UI::GUILabel> lblRegStatus;
			UI::GUITextBox *txtRegStatus;
			NotNullPtr<UI::GUILabel> lblLAC;
			UI::GUITextBox *txtLAC;
			NotNullPtr<UI::GUILabel> lblCI;
			UI::GUITextBox *txtCI;
			NotNullPtr<UI::GUILabel> lblACT;
			UI::GUITextBox *txtACT;
			NotNullPtr<UI::GUILabel> lblSignalQuality;
			UI::GUITextBox *txtSignalQuality;
			UI::GUIRealtimeLineChart *rlcRSSI;

			NotNullPtr<UI::GUITabPage> tpPhoneBook;
			NotNullPtr<UI::GUIPanel> pnlPhone;
			NotNullPtr<UI::GUIComboBox> cboPhoneStorage;
			NotNullPtr<UI::GUIButton> btnPhoneRead;
			NotNullPtr<UI::GUILabel> lblPhoneStatus;
			UI::GUIListView *lvPhone;

			NotNullPtr<UI::GUITabPage> tpSMS;
			NotNullPtr<UI::GUIPanel> pnlSMS;
			NotNullPtr<UI::GUIComboBox> cboSMSStorage;
			NotNullPtr<UI::GUIButton> btnSMSRead;
			NotNullPtr<UI::GUILabel> lblSMSC;
			UI::GUITextBox *txtSMSC;
			NotNullPtr<UI::GUIButton> btnSMSSave;
			NotNullPtr<UI::GUIButton> btnSMSDelete;
			NotNullPtr<UI::GUIButton> btnSMSSaveAll;
			UI::GUIListView *lvSMS;

			NotNullPtr<UI::GUITabPage> tpPDPContext;
			NotNullPtr<UI::GUIPanel> pnlPDPContext;
			NotNullPtr<UI::GUIButton> btnPDPContextLoad;
			NotNullPtr<UI::GUILabel> lblPDPContextStatus;
			UI::GUITextBox *txtPDPContextStatus;
			NotNullPtr<UI::GUIButton> btnPDPAttach;
			NotNullPtr<UI::GUIButton> btnPDPDetach;
			NotNullPtr<UI::GUILabel> lblPDPCID;
			UI::GUITextBox *txtPDPCID;
			NotNullPtr<UI::GUILabel> lblPDPType;
			UI::GUITextBox *txtPDPType;
			NotNullPtr<UI::GUILabel> lblPDPAPN;
			UI::GUITextBox *txtPDPAPN;
			NotNullPtr<UI::GUIButton> btnPDPContextSet;
			NotNullPtr<UI::GUIButton> btnPDPContextActiveAll;
			NotNullPtr<UI::GUIButton> btnPDPContextActiveSelected;
			NotNullPtr<UI::GUIButton> btnPDPContextDeactiveAll;
			NotNullPtr<UI::GUIButton> btnPDPContextDeactiveSelected;
			UI::GUIListView *lvPDPContext;

			NotNullPtr<UI::GUITabPage> tpATCommand;
			NotNullPtr<UI::GUIPanel> pnlATCommand;
			NotNullPtr<UI::GUILabel> lblATCommand;
			UI::GUITextBox *txtATCommand;
			NotNullPtr<UI::GUIButton> btnATCommand;
			UI::GUITextBox *txtATCommandRep;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

			NotNullPtr<UI::GUITabPage> tpHuawei;
			NotNullPtr<UI::GUILabel> lblHuaweiICCID;
			UI::GUITextBox *txtHuaweiICCID;
			NotNullPtr<UI::GUILabel> lblHuaweiSIMType;
			UI::GUITextBox *txtHuaweiSIMType;
			NotNullPtr<UI::GUILabel> lblHuaweiSrvStatus;
			UI::GUITextBox *txtHuaweiSrvStatus;
			NotNullPtr<UI::GUILabel> lblHuaweiSrvDomain;
			UI::GUITextBox *txtHuaweiSrvDomain;
			NotNullPtr<UI::GUILabel> lblHuaweiRoamStatus;
			UI::GUITextBox *txtHuaweiRoamStatus;
			NotNullPtr<UI::GUILabel> lblHuaweiSIMState;
			UI::GUITextBox *txtHuaweiSIMState;
			NotNullPtr<UI::GUILabel> lblHuaweiLockState;
			UI::GUITextBox *txtHuaweiLockState;
			NotNullPtr<UI::GUILabel> lblHuaweiSysMode;
			UI::GUITextBox *txtHuaweiSysMode;
			NotNullPtr<UI::GUILabel> lblHuaweiSubMode;
			UI::GUITextBox *txtHuaweiSubMode;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ1;
			UI::GUITextBox *txtHuaweiCSQ1;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ2;
			UI::GUITextBox *txtHuaweiCSQ2;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ3;
			UI::GUITextBox *txtHuaweiCSQ3;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ4;
			UI::GUITextBox *txtHuaweiCSQ4;
			NotNullPtr<UI::GUIGroupBox> grpHuaweiDHCP;
			NotNullPtr<UI::GUIButton> btnHuaweiDHCP;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPClientIP;
			UI::GUITextBox *txtHuaweiDHCPClientIP;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPNetmask;
			UI::GUITextBox *txtHuaweiDHCPNetmask;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPGateway;
			UI::GUITextBox *txtHuaweiDHCPGateway;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPServer;
			UI::GUITextBox *txtHuaweiDHCPServer;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPPriDNS;
			UI::GUITextBox *txtHuaweiDHCPPriDNS;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPSecDNS;
			UI::GUITextBox *txtHuaweiDHCPSecDNS;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPMaxRXbps;
			UI::GUITextBox *txtHuaweiDHCPMaxRXbps;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPMaxTXbps;
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
