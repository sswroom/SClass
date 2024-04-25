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
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::GSMModemController *modem;
			IO::HuaweiGSMModemController *huawei;
			IO::ATCommandChannel *channel;
			IO::Stream *port;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUILabel> lblDeviceSerial;
			NN<UI::GUIComboBox> cboDeviceSerial;
			NN<UI::GUIButton> btnDeviceSerial;
			NN<UI::GUILabel> lblDeviceStatus;
			NN<UI::GUITextBox> txtDeviceStatus;
			NN<UI::GUIButton> btnDeviceOther;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIPanel> pnlInfo;
			NN<UI::GUILabel> lblModemManu;
			NN<UI::GUITextBox> txtModemManu;
			NN<UI::GUILabel> lblModemModel;
			NN<UI::GUITextBox> txtModemModel;
			NN<UI::GUILabel> lblModemVer;
			NN<UI::GUITextBox> txtModemVer;
			NN<UI::GUILabel> lblModemIMEI;
			NN<UI::GUITextBox> txtModemIMEI;
			NN<UI::GUILabel> lblIMSI;
			NN<UI::GUITextBox> txtIMSI;
			NN<UI::GUILabel> lblTECharset;
			NN<UI::GUITextBox> txtTECharset;
			NN<UI::GUILabel> lblOperator;
			NN<UI::GUITextBox> txtOperator;
			NN<UI::GUILabel> lblRegStatus;
			NN<UI::GUITextBox> txtRegStatus;
			NN<UI::GUILabel> lblLAC;
			NN<UI::GUITextBox> txtLAC;
			NN<UI::GUILabel> lblCI;
			NN<UI::GUITextBox> txtCI;
			NN<UI::GUILabel> lblACT;
			NN<UI::GUITextBox> txtACT;
			NN<UI::GUILabel> lblSignalQuality;
			NN<UI::GUITextBox> txtSignalQuality;
			NN<UI::GUIRealtimeLineChart> rlcRSSI;

			NN<UI::GUITabPage> tpPhoneBook;
			NN<UI::GUIPanel> pnlPhone;
			NN<UI::GUIComboBox> cboPhoneStorage;
			NN<UI::GUIButton> btnPhoneRead;
			NN<UI::GUILabel> lblPhoneStatus;
			NN<UI::GUIListView> lvPhone;

			NN<UI::GUITabPage> tpSMS;
			NN<UI::GUIPanel> pnlSMS;
			NN<UI::GUIComboBox> cboSMSStorage;
			NN<UI::GUIButton> btnSMSRead;
			NN<UI::GUILabel> lblSMSC;
			NN<UI::GUITextBox> txtSMSC;
			NN<UI::GUIButton> btnSMSSave;
			NN<UI::GUIButton> btnSMSDelete;
			NN<UI::GUIButton> btnSMSSaveAll;
			NN<UI::GUIListView> lvSMS;

			NN<UI::GUITabPage> tpPDPContext;
			NN<UI::GUIPanel> pnlPDPContext;
			NN<UI::GUIButton> btnPDPContextLoad;
			NN<UI::GUILabel> lblPDPContextStatus;
			NN<UI::GUITextBox> txtPDPContextStatus;
			NN<UI::GUIButton> btnPDPAttach;
			NN<UI::GUIButton> btnPDPDetach;
			NN<UI::GUILabel> lblPDPCID;
			NN<UI::GUITextBox> txtPDPCID;
			NN<UI::GUILabel> lblPDPType;
			NN<UI::GUITextBox> txtPDPType;
			NN<UI::GUILabel> lblPDPAPN;
			NN<UI::GUITextBox> txtPDPAPN;
			NN<UI::GUIButton> btnPDPContextSet;
			NN<UI::GUIButton> btnPDPContextActiveAll;
			NN<UI::GUIButton> btnPDPContextActiveSelected;
			NN<UI::GUIButton> btnPDPContextDeactiveAll;
			NN<UI::GUIButton> btnPDPContextDeactiveSelected;
			NN<UI::GUIListView> lvPDPContext;

			NN<UI::GUITabPage> tpATCommand;
			NN<UI::GUIPanel> pnlATCommand;
			NN<UI::GUILabel> lblATCommand;
			NN<UI::GUITextBox> txtATCommand;
			NN<UI::GUIButton> btnATCommand;
			NN<UI::GUITextBox> txtATCommandRep;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			NN<UI::GUITabPage> tpHuawei;
			NN<UI::GUILabel> lblHuaweiICCID;
			NN<UI::GUITextBox> txtHuaweiICCID;
			NN<UI::GUILabel> lblHuaweiSIMType;
			NN<UI::GUITextBox> txtHuaweiSIMType;
			NN<UI::GUILabel> lblHuaweiSrvStatus;
			NN<UI::GUITextBox> txtHuaweiSrvStatus;
			NN<UI::GUILabel> lblHuaweiSrvDomain;
			NN<UI::GUITextBox> txtHuaweiSrvDomain;
			NN<UI::GUILabel> lblHuaweiRoamStatus;
			NN<UI::GUITextBox> txtHuaweiRoamStatus;
			NN<UI::GUILabel> lblHuaweiSIMState;
			NN<UI::GUITextBox> txtHuaweiSIMState;
			NN<UI::GUILabel> lblHuaweiLockState;
			NN<UI::GUITextBox> txtHuaweiLockState;
			NN<UI::GUILabel> lblHuaweiSysMode;
			NN<UI::GUITextBox> txtHuaweiSysMode;
			NN<UI::GUILabel> lblHuaweiSubMode;
			NN<UI::GUITextBox> txtHuaweiSubMode;
			NN<UI::GUILabel> lblHuaweiCSQ1;
			NN<UI::GUITextBox> txtHuaweiCSQ1;
			NN<UI::GUILabel> lblHuaweiCSQ2;
			NN<UI::GUITextBox> txtHuaweiCSQ2;
			NN<UI::GUILabel> lblHuaweiCSQ3;
			NN<UI::GUITextBox> txtHuaweiCSQ3;
			NN<UI::GUILabel> lblHuaweiCSQ4;
			NN<UI::GUITextBox> txtHuaweiCSQ4;
			NN<UI::GUIGroupBox> grpHuaweiDHCP;
			NN<UI::GUIButton> btnHuaweiDHCP;
			NN<UI::GUILabel> lblHuaweiDHCPClientIP;
			NN<UI::GUITextBox> txtHuaweiDHCPClientIP;
			NN<UI::GUILabel> lblHuaweiDHCPNetmask;
			NN<UI::GUITextBox> txtHuaweiDHCPNetmask;
			NN<UI::GUILabel> lblHuaweiDHCPGateway;
			NN<UI::GUITextBox> txtHuaweiDHCPGateway;
			NN<UI::GUILabel> lblHuaweiDHCPServer;
			NN<UI::GUITextBox> txtHuaweiDHCPServer;
			NN<UI::GUILabel> lblHuaweiDHCPPriDNS;
			NN<UI::GUITextBox> txtHuaweiDHCPPriDNS;
			NN<UI::GUILabel> lblHuaweiDHCPSecDNS;
			NN<UI::GUITextBox> txtHuaweiDHCPSecDNS;
			NN<UI::GUILabel> lblHuaweiDHCPMaxRXbps;
			NN<UI::GUITextBox> txtHuaweiDHCPMaxRXbps;
			NN<UI::GUILabel> lblHuaweiDHCPMaxTXbps;
			NN<UI::GUITextBox> txtHuaweiDHCPMaxTXbps;

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
			Data::ArrayListNN<IO::GSMModemController::SMSMessage> msgList;

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
			void InitStream(NN<IO::Stream> stm, Bool updateSerial);
			void CloseStream(Bool updateUI);
		public:
			AVIRGSMModemForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::Stream *port);
			virtual ~AVIRGSMModemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
