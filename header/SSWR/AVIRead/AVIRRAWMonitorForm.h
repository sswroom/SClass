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
				Optional<Net::EthernetAnalyzer::IPTranStatus> sendStatus;
				Optional<Net::EthernetAnalyzer::IPTranStatus> recvStatus;
			} IPTranInfo;

			typedef struct
			{
				UInt32 ip;
				Int64 count;
				NN<Text::String> name;
				NN<Text::String> country;
			} PingIPInfo;

		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Net::WebServer::WebListener *listener;
			Net::EthernetWebHandler *webHdlr;
			Net::SocketMonitor *socMon;
			NN<Net::EthernetAnalyzer> analyzer;
			Net::WhoisHandler whois;
			Sync::Mutex plogMut;
			IO::PacketLogWriter *plogWriter;
			Int32 linkType;
			UInt32 adapterIP;
			Bool adapterChanged;
			Bool dataUpdated;
			Sync::Mutex pingIPMut;
			Data::FastMapNN<UInt32, PingIPInfo> pingIPMap;
			Bool pingIPListUpdated;
			Bool pingIPContUpdated;
			PingIPInfo *currPingIP;
			Data::FastMapNN<UInt32, IPTranInfo> ipTranMap;
			UOSInt ipTranCnt;
			UOSInt tcp4synLastIndex;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUITextBox> txtInfo;
			NN<UI::GUIButton> btnInfo;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblPLog;
			NN<UI::GUIComboBox> cboPLog;
			NN<UI::GUIButton> btnPLog;
			NN<UI::GUITextBox> txtPLog;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpDevice;
			NN<UI::GUIListView> lvDevice;
			NN<UI::GUIVSplitter> vspDevice;
			NN<UI::GUITextBox> txtDevice;

			NN<UI::GUITabPage> tpIPTran;
			NN<UI::GUIListBox> lbIPTran;
			NN<UI::GUIHSplitter> hspIPTran;
			NN<UI::GUITabControl> tcIPTran;
			NN<UI::GUITabPage> tpIPTranInfo;
			NN<UI::GUIListView> lvIPTranInfo;
			NN<UI::GUITabPage> tpIPTranWhois;
			NN<UI::GUITextBox> txtIPTranWhois;

			NN<UI::GUITabPage> tpDNSReqv4;
			NN<UI::GUIListBox> lbDNSReqv4;
			NN<UI::GUIHSplitter> hspDNSReqv4;
			NN<UI::GUIPanel> pnlDNSReqv4;
			NN<UI::GUILabel> lblDNSReqv4Name;
			NN<UI::GUITextBox> txtDNSReqv4Name;
			NN<UI::GUILabel> lblDNSReqv4ReqTime;
			NN<UI::GUITextBox> txtDNSReqv4ReqTime;
			NN<UI::GUILabel> lblDNSReqv4TTL;
			NN<UI::GUITextBox> txtDNSReqv4TTL;
			NN<UI::GUIListView> lvDNSReqv4;

			NN<UI::GUITabPage> tpDNSReqv6;
			NN<UI::GUIListBox> lbDNSReqv6;
			NN<UI::GUIHSplitter> hspDNSReqv6;
			NN<UI::GUIPanel> pnlDNSReqv6;
			NN<UI::GUILabel> lblDNSReqv6Name;
			NN<UI::GUITextBox> txtDNSReqv6Name;
			NN<UI::GUILabel> lblDNSReqv6ReqTime;
			NN<UI::GUITextBox> txtDNSReqv6ReqTime;
			NN<UI::GUILabel> lblDNSReqv6TTL;
			NN<UI::GUITextBox> txtDNSReqv6TTL;
			NN<UI::GUIListView> lvDNSReqv6;

			NN<UI::GUITabPage> tpDNSReqOth;
			NN<UI::GUIListBox> lbDNSReqOth;
			NN<UI::GUIHSplitter> hspDNSReqOth;
			NN<UI::GUIPanel> pnlDNSReqOth;
			NN<UI::GUILabel> lblDNSReqOthName;
			NN<UI::GUITextBox> txtDNSReqOthName;
			NN<UI::GUILabel> lblDNSReqOthReqTime;
			NN<UI::GUITextBox> txtDNSReqOthReqTime;
			NN<UI::GUILabel> lblDNSReqOthTTL;
			NN<UI::GUITextBox> txtDNSReqOthTTL;
			NN<UI::GUIListView> lvDNSReqOth;

			NN<UI::GUITabPage> tpDNSTarget;
			NN<UI::GUIListBox> lbDNSTarget;
			NN<UI::GUIHSplitter> hspDNSTarget;
			NN<UI::GUITabControl> tcDNSTarget;
			NN<UI::GUITabPage> tpDNSTargetDomain;
			NN<UI::GUIListBox> lbDNSTargetDomain;
			NN<UI::GUITabPage> tpDNSTargetWhois;
			NN<UI::GUITextBox> txtDNSTargetWhois;

			NN<UI::GUITabPage> tpMDNS;
			NN<UI::GUIListBox> lbMDNS;
			NN<UI::GUIHSplitter> hspMDNS;
			NN<UI::GUIPanel> pnlMDNS;
			NN<UI::GUILabel> lblMDNSName;
			NN<UI::GUITextBox> txtMDNSName;
			NN<UI::GUILabel> lblMDNSType;
			NN<UI::GUITextBox> txtMDNSType;
			NN<UI::GUILabel> lblMDNSClass;
			NN<UI::GUITextBox> txtMDNSClass;
			NN<UI::GUILabel> lblMDNSTTL;
			NN<UI::GUITextBox> txtMDNSTTL;
			NN<UI::GUILabel> lblMDNSResult;
			NN<UI::GUITextBox> txtMDNSResult;

			NN<UI::GUITabPage> tpDNSClient;
			NN<UI::GUIListBox> lbDNSClient;
			NN<UI::GUIHSplitter> hspDNSClient;
			NN<UI::GUIListView> lvDNSClient;

			NN<UI::GUITabPage> tpDHCP;
			NN<UI::GUIListView> lvDHCP;
			
			NN<UI::GUITabPage> tpIPLog;
			NN<UI::GUIListBox> lbIPLog;
			NN<UI::GUIHSplitter> hspIPLog;
			NN<UI::GUIListBox> lbIPLogVal;

			NN<UI::GUITabPage> tpPingIP;
			NN<UI::GUIListBox> lbPingIP;
			NN<UI::GUIHSplitter> hspPingIP;
			NN<UI::GUITabControl> tcPingIP;
			NN<UI::GUITabPage> tpPingIPInfo;
			NN<UI::GUILabel> lblPingIPCount;
			NN<UI::GUITextBox> txtPingIPCount;
			NN<UI::GUILabel> lblPingIPName;
			NN<UI::GUITextBox> txtPingIPName;
			NN<UI::GUILabel> lblPingIPCountry;
			NN<UI::GUITextBox> txtPingIPCountry;
			NN<UI::GUITabPage> tpPingIPWhois;
			NN<UI::GUITextBox> txtPingIPWhois;

			NN<UI::GUITabPage> tpTCP4SYN;
			NN<UI::GUIListView> lvTCP4SYN;
			
			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			static void __stdcall OnPingPacket(AnyType userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize);
			static void __stdcall OnRAWData(AnyType userData, const UInt8 *rawData, UOSInt packetSize);
			static void __stdcall OnIPv4Data(AnyType userData, const UInt8 *rawData, UOSInt packetSize);
			static void __stdcall OnInfoClicked(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnPLogClicked(AnyType userObj);
			static void __stdcall OnIPSelChg(AnyType userObj);
			static void __stdcall OnIPTranSelChg(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnPingIPSelChg(AnyType userObj);
			static void __stdcall OnDNSReqv4SelChg(AnyType userObj);
			static void __stdcall OnDNSReqv6SelChg(AnyType userObj);
			static void __stdcall OnDNSReqOthSelChg(AnyType userObj);
			static void __stdcall OnDNSTargetSelChg(AnyType userObj);
			static void __stdcall OnMDNSSelChg(AnyType userObj);
			static void __stdcall OnDNSClientSelChg(AnyType userObj);
			static void __stdcall OnIPLogSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDeviceSelChg(AnyType userObj);

		public:
			AVIRRAWMonitorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::EthernetAnalyzer> analyzer);
			virtual ~AVIRRAWMonitorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
