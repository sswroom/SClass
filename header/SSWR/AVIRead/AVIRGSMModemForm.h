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
			NotNullPtr<UI::GUITextBox> txtDeviceStatus;
			NotNullPtr<UI::GUIButton> btnDeviceOther;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUIPanel> pnlInfo;
			NotNullPtr<UI::GUILabel> lblModemManu;
			NotNullPtr<UI::GUITextBox> txtModemManu;
			NotNullPtr<UI::GUILabel> lblModemModel;
			NotNullPtr<UI::GUITextBox> txtModemModel;
			NotNullPtr<UI::GUILabel> lblModemVer;
			NotNullPtr<UI::GUITextBox> txtModemVer;
			NotNullPtr<UI::GUILabel> lblModemIMEI;
			NotNullPtr<UI::GUITextBox> txtModemIMEI;
			NotNullPtr<UI::GUILabel> lblIMSI;
			NotNullPtr<UI::GUITextBox> txtIMSI;
			NotNullPtr<UI::GUILabel> lblTECharset;
			NotNullPtr<UI::GUITextBox> txtTECharset;
			NotNullPtr<UI::GUILabel> lblOperator;
			NotNullPtr<UI::GUITextBox> txtOperator;
			NotNullPtr<UI::GUILabel> lblRegStatus;
			NotNullPtr<UI::GUITextBox> txtRegStatus;
			NotNullPtr<UI::GUILabel> lblLAC;
			NotNullPtr<UI::GUITextBox> txtLAC;
			NotNullPtr<UI::GUILabel> lblCI;
			NotNullPtr<UI::GUITextBox> txtCI;
			NotNullPtr<UI::GUILabel> lblACT;
			NotNullPtr<UI::GUITextBox> txtACT;
			NotNullPtr<UI::GUILabel> lblSignalQuality;
			NotNullPtr<UI::GUITextBox> txtSignalQuality;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcRSSI;

			NotNullPtr<UI::GUITabPage> tpPhoneBook;
			NotNullPtr<UI::GUIPanel> pnlPhone;
			NotNullPtr<UI::GUIComboBox> cboPhoneStorage;
			NotNullPtr<UI::GUIButton> btnPhoneRead;
			NotNullPtr<UI::GUILabel> lblPhoneStatus;
			NotNullPtr<UI::GUIListView> lvPhone;

			NotNullPtr<UI::GUITabPage> tpSMS;
			NotNullPtr<UI::GUIPanel> pnlSMS;
			NotNullPtr<UI::GUIComboBox> cboSMSStorage;
			NotNullPtr<UI::GUIButton> btnSMSRead;
			NotNullPtr<UI::GUILabel> lblSMSC;
			NotNullPtr<UI::GUITextBox> txtSMSC;
			NotNullPtr<UI::GUIButton> btnSMSSave;
			NotNullPtr<UI::GUIButton> btnSMSDelete;
			NotNullPtr<UI::GUIButton> btnSMSSaveAll;
			NotNullPtr<UI::GUIListView> lvSMS;

			NotNullPtr<UI::GUITabPage> tpPDPContext;
			NotNullPtr<UI::GUIPanel> pnlPDPContext;
			NotNullPtr<UI::GUIButton> btnPDPContextLoad;
			NotNullPtr<UI::GUILabel> lblPDPContextStatus;
			NotNullPtr<UI::GUITextBox> txtPDPContextStatus;
			NotNullPtr<UI::GUIButton> btnPDPAttach;
			NotNullPtr<UI::GUIButton> btnPDPDetach;
			NotNullPtr<UI::GUILabel> lblPDPCID;
			NotNullPtr<UI::GUITextBox> txtPDPCID;
			NotNullPtr<UI::GUILabel> lblPDPType;
			NotNullPtr<UI::GUITextBox> txtPDPType;
			NotNullPtr<UI::GUILabel> lblPDPAPN;
			NotNullPtr<UI::GUITextBox> txtPDPAPN;
			NotNullPtr<UI::GUIButton> btnPDPContextSet;
			NotNullPtr<UI::GUIButton> btnPDPContextActiveAll;
			NotNullPtr<UI::GUIButton> btnPDPContextActiveSelected;
			NotNullPtr<UI::GUIButton> btnPDPContextDeactiveAll;
			NotNullPtr<UI::GUIButton> btnPDPContextDeactiveSelected;
			NotNullPtr<UI::GUIListView> lvPDPContext;

			NotNullPtr<UI::GUITabPage> tpATCommand;
			NotNullPtr<UI::GUIPanel> pnlATCommand;
			NotNullPtr<UI::GUILabel> lblATCommand;
			NotNullPtr<UI::GUITextBox> txtATCommand;
			NotNullPtr<UI::GUIButton> btnATCommand;
			NotNullPtr<UI::GUITextBox> txtATCommandRep;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

			NotNullPtr<UI::GUITabPage> tpHuawei;
			NotNullPtr<UI::GUILabel> lblHuaweiICCID;
			NotNullPtr<UI::GUITextBox> txtHuaweiICCID;
			NotNullPtr<UI::GUILabel> lblHuaweiSIMType;
			NotNullPtr<UI::GUITextBox> txtHuaweiSIMType;
			NotNullPtr<UI::GUILabel> lblHuaweiSrvStatus;
			NotNullPtr<UI::GUITextBox> txtHuaweiSrvStatus;
			NotNullPtr<UI::GUILabel> lblHuaweiSrvDomain;
			NotNullPtr<UI::GUITextBox> txtHuaweiSrvDomain;
			NotNullPtr<UI::GUILabel> lblHuaweiRoamStatus;
			NotNullPtr<UI::GUITextBox> txtHuaweiRoamStatus;
			NotNullPtr<UI::GUILabel> lblHuaweiSIMState;
			NotNullPtr<UI::GUITextBox> txtHuaweiSIMState;
			NotNullPtr<UI::GUILabel> lblHuaweiLockState;
			NotNullPtr<UI::GUITextBox> txtHuaweiLockState;
			NotNullPtr<UI::GUILabel> lblHuaweiSysMode;
			NotNullPtr<UI::GUITextBox> txtHuaweiSysMode;
			NotNullPtr<UI::GUILabel> lblHuaweiSubMode;
			NotNullPtr<UI::GUITextBox> txtHuaweiSubMode;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ1;
			NotNullPtr<UI::GUITextBox> txtHuaweiCSQ1;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ2;
			NotNullPtr<UI::GUITextBox> txtHuaweiCSQ2;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ3;
			NotNullPtr<UI::GUITextBox> txtHuaweiCSQ3;
			NotNullPtr<UI::GUILabel> lblHuaweiCSQ4;
			NotNullPtr<UI::GUITextBox> txtHuaweiCSQ4;
			NotNullPtr<UI::GUIGroupBox> grpHuaweiDHCP;
			NotNullPtr<UI::GUIButton> btnHuaweiDHCP;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPClientIP;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPClientIP;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPNetmask;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPNetmask;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPGateway;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPGateway;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPServer;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPServer;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPPriDNS;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPPriDNS;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPSecDNS;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPSecDNS;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPMaxRXbps;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPMaxRXbps;
			NotNullPtr<UI::GUILabel> lblHuaweiDHCPMaxTXbps;
			NotNullPtr<UI::GUITextBox> txtHuaweiDHCPMaxTXbps;

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

			static UInt32 __stdcall ModemThread(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPhoneRClick(AnyType userObj);
			static void __stdcall OnSMSRClick(AnyType userObj);
			static void __stdcall OnSMSSaveClick(AnyType userObj);
			static void __stdcall OnSMSDeleteClick(AnyType userObj);
			static void __stdcall OnSMSSaveAllClick(AnyType userObj);
			static void __stdcall OnDeviceSerialClk(AnyType userObj);
			static void __stdcall OnDeviceOtherClk(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnATCommandClicked(AnyType userObj);
			static Bool __stdcall OnATCommandKeyDown(AnyType userObj, UInt32 osKey);
			static void __stdcall OnPDPContextLoadClicked(AnyType userObj);
			static void __stdcall OnPDPAttachClicked(AnyType userObj);
			static void __stdcall OnPDPDetachClicked(AnyType userObj);
			static void __stdcall OnPDPContextSetClicked(AnyType userObj);
			static void __stdcall OnPDPContextActiveAllClicked(AnyType userObj);
			static void __stdcall OnPDPContextDeactiveAllClicked(AnyType userObj);
			static void __stdcall OnPDPContextActiveSelectedClicked(AnyType userObj);
			static void __stdcall OnPDPContextDeactiveSelectedClicked(AnyType userObj);
			static void __stdcall OnHuaweiDHCPClicked(AnyType userObj);

			void LoadPhoneBook();
			void LoadSMS();
			void LoadPDPContext();
			void InitStream(NotNullPtr<IO::Stream> stm, Bool updateSerial);
			void CloseStream(Bool updateUI);
		public:
			AVIRGSMModemForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::Stream *port);
			virtual ~AVIRGSMModemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
