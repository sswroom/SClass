#ifndef _SM_SSWR_AVIREAD_AVIRRAWMONITORFORM
#define _SM_SSWR_AVIREAD_AVIRRAWMONITORFORM
#include "IO/LogTool.h"
#include "IO/PacketLogWriter.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/EthernetWebHandler.h"
#include "Net/SocketMonitor.h"
#include "Net/WhoisHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRAWMonitorForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt32 ip;
				Net::EthernetAnalyzer::IPTranStatus *sendStatus;
				Net::EthernetAnalyzer::IPTranStatus *recvStatus;
			} IPTranInfo;

			typedef struct
			{
				UInt32 ip;
				Int64 count;
				NotNullPtr<Text::String> name;
				NotNullPtr<Text::String> country;
			} PingIPInfo;

		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Net::WebServer::WebListener *listener;
			Net::EthernetWebHandler *webHdlr;
			Net::SocketMonitor *socMon;
			Net::EthernetAnalyzer *analyzer;
			Net::WhoisHandler whois;
			Sync::Mutex plogMut;
			IO::PacketLogWriter *plogWriter;
			Int32 linkType;
			UInt32 adapterIP;
			Bool adapterChanged;
			Bool dataUpdated;
			Sync::Mutex pingIPMut;
			Data::FastMap<UInt32, PingIPInfo*> pingIPMap;
			Bool pingIPListUpdated;
			Bool pingIPContUpdated;
			PingIPInfo *currPingIP;
			Data::FastMap<UInt32, IPTranInfo*> ipTranMap;
			UOSInt ipTranCnt;
			UOSInt tcp4synLastIndex;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblInfo;
			NotNullPtr<UI::GUITextBox> txtInfo;
			NotNullPtr<UI::GUIButton> btnInfo;
			NotNullPtr<UI::GUILabel> lblIP;
			NotNullPtr<UI::GUIComboBox> cboIP;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblPLog;
			NotNullPtr<UI::GUIComboBox> cboPLog;
			NotNullPtr<UI::GUIButton> btnPLog;
			NotNullPtr<UI::GUITextBox> txtPLog;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpDevice;
			NotNullPtr<UI::GUIListView> lvDevice;
			NotNullPtr<UI::GUIVSplitter> vspDevice;
			NotNullPtr<UI::GUITextBox> txtDevice;

			NotNullPtr<UI::GUITabPage> tpIPTran;
			NotNullPtr<UI::GUIListBox> lbIPTran;
			NotNullPtr<UI::GUIHSplitter> hspIPTran;
			NotNullPtr<UI::GUITabControl> tcIPTran;
			NotNullPtr<UI::GUITabPage> tpIPTranInfo;
			NotNullPtr<UI::GUIListView> lvIPTranInfo;
			NotNullPtr<UI::GUITabPage> tpIPTranWhois;
			NotNullPtr<UI::GUITextBox> txtIPTranWhois;

			NotNullPtr<UI::GUITabPage> tpDNSReqv4;
			NotNullPtr<UI::GUIListBox> lbDNSReqv4;
			NotNullPtr<UI::GUIHSplitter> hspDNSReqv4;
			NotNullPtr<UI::GUIPanel> pnlDNSReqv4;
			NotNullPtr<UI::GUILabel> lblDNSReqv4Name;
			NotNullPtr<UI::GUITextBox> txtDNSReqv4Name;
			NotNullPtr<UI::GUILabel> lblDNSReqv4ReqTime;
			NotNullPtr<UI::GUITextBox> txtDNSReqv4ReqTime;
			NotNullPtr<UI::GUILabel> lblDNSReqv4TTL;
			NotNullPtr<UI::GUITextBox> txtDNSReqv4TTL;
			NotNullPtr<UI::GUIListView> lvDNSReqv4;

			NotNullPtr<UI::GUITabPage> tpDNSReqv6;
			NotNullPtr<UI::GUIListBox> lbDNSReqv6;
			NotNullPtr<UI::GUIHSplitter> hspDNSReqv6;
			NotNullPtr<UI::GUIPanel> pnlDNSReqv6;
			NotNullPtr<UI::GUILabel> lblDNSReqv6Name;
			NotNullPtr<UI::GUITextBox> txtDNSReqv6Name;
			NotNullPtr<UI::GUILabel> lblDNSReqv6ReqTime;
			NotNullPtr<UI::GUITextBox> txtDNSReqv6ReqTime;
			NotNullPtr<UI::GUILabel> lblDNSReqv6TTL;
			NotNullPtr<UI::GUITextBox> txtDNSReqv6TTL;
			NotNullPtr<UI::GUIListView> lvDNSReqv6;

			NotNullPtr<UI::GUITabPage> tpDNSReqOth;
			NotNullPtr<UI::GUIListBox> lbDNSReqOth;
			NotNullPtr<UI::GUIHSplitter> hspDNSReqOth;
			NotNullPtr<UI::GUIPanel> pnlDNSReqOth;
			NotNullPtr<UI::GUILabel> lblDNSReqOthName;
			NotNullPtr<UI::GUITextBox> txtDNSReqOthName;
			NotNullPtr<UI::GUILabel> lblDNSReqOthReqTime;
			NotNullPtr<UI::GUITextBox> txtDNSReqOthReqTime;
			NotNullPtr<UI::GUILabel> lblDNSReqOthTTL;
			NotNullPtr<UI::GUITextBox> txtDNSReqOthTTL;
			NotNullPtr<UI::GUIListView> lvDNSReqOth;

			NotNullPtr<UI::GUITabPage> tpDNSTarget;
			NotNullPtr<UI::GUIListBox> lbDNSTarget;
			NotNullPtr<UI::GUIHSplitter> hspDNSTarget;
			NotNullPtr<UI::GUITabControl> tcDNSTarget;
			NotNullPtr<UI::GUITabPage> tpDNSTargetDomain;
			NotNullPtr<UI::GUIListBox> lbDNSTargetDomain;
			NotNullPtr<UI::GUITabPage> tpDNSTargetWhois;
			NotNullPtr<UI::GUITextBox> txtDNSTargetWhois;

			NotNullPtr<UI::GUITabPage> tpMDNS;
			NotNullPtr<UI::GUIListBox> lbMDNS;
			NotNullPtr<UI::GUIHSplitter> hspMDNS;
			NotNullPtr<UI::GUIPanel> pnlMDNS;
			NotNullPtr<UI::GUILabel> lblMDNSName;
			NotNullPtr<UI::GUITextBox> txtMDNSName;
			NotNullPtr<UI::GUILabel> lblMDNSType;
			NotNullPtr<UI::GUITextBox> txtMDNSType;
			NotNullPtr<UI::GUILabel> lblMDNSClass;
			NotNullPtr<UI::GUITextBox> txtMDNSClass;
			NotNullPtr<UI::GUILabel> lblMDNSTTL;
			NotNullPtr<UI::GUITextBox> txtMDNSTTL;
			NotNullPtr<UI::GUILabel> lblMDNSResult;
			NotNullPtr<UI::GUITextBox> txtMDNSResult;

			NotNullPtr<UI::GUITabPage> tpDNSClient;
			NotNullPtr<UI::GUIListBox> lbDNSClient;
			NotNullPtr<UI::GUIHSplitter> hspDNSClient;
			NotNullPtr<UI::GUIListView> lvDNSClient;

			NotNullPtr<UI::GUITabPage> tpDHCP;
			NotNullPtr<UI::GUIListView> lvDHCP;
			
			NotNullPtr<UI::GUITabPage> tpIPLog;
			NotNullPtr<UI::GUIListBox> lbIPLog;
			NotNullPtr<UI::GUIHSplitter> hspIPLog;
			NotNullPtr<UI::GUIListBox> lbIPLogVal;

			NotNullPtr<UI::GUITabPage> tpPingIP;
			NotNullPtr<UI::GUIListBox> lbPingIP;
			NotNullPtr<UI::GUIHSplitter> hspPingIP;
			NotNullPtr<UI::GUITabControl> tcPingIP;
			NotNullPtr<UI::GUITabPage> tpPingIPInfo;
			NotNullPtr<UI::GUILabel> lblPingIPCount;
			NotNullPtr<UI::GUITextBox> txtPingIPCount;
			NotNullPtr<UI::GUILabel> lblPingIPName;
			NotNullPtr<UI::GUITextBox> txtPingIPName;
			NotNullPtr<UI::GUILabel> lblPingIPCountry;
			NotNullPtr<UI::GUITextBox> txtPingIPCountry;
			NotNullPtr<UI::GUITabPage> tpPingIPWhois;
			NotNullPtr<UI::GUITextBox> txtPingIPWhois;

			NotNullPtr<UI::GUITabPage> tpTCP4SYN;
			NotNullPtr<UI::GUIListView> lvTCP4SYN;
			
			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUITextBox> txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;

			static void __stdcall OnPingPacket(void *userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize);
			static void __stdcall OnRAWData(void *userData, const UInt8 *rawData, UOSInt packetSize);
			static void __stdcall OnIPv4Data(void *userData, const UInt8 *rawData, UOSInt packetSize);
			static void __stdcall OnInfoClicked(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnPLogClicked(void *userObj);
			static void __stdcall OnIPSelChg(void *userObj);
			static void __stdcall OnIPTranSelChg(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnPingIPSelChg(void *userObj);
			static void __stdcall OnDNSReqv4SelChg(void *userObj);
			static void __stdcall OnDNSReqv6SelChg(void *userObj);
			static void __stdcall OnDNSReqOthSelChg(void *userObj);
			static void __stdcall OnDNSTargetSelChg(void *userObj);
			static void __stdcall OnMDNSSelChg(void *userObj);
			static void __stdcall OnDNSClientSelChg(void *userObj);
			static void __stdcall OnIPLogSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDeviceSelChg(void *userObj);

		public:
			AVIRRAWMonitorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Net::EthernetAnalyzer *analyzer);
			virtual ~AVIRRAWMonitorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
