#ifndef _SM_SSWR_AVIREAD_AVIRNETINFOFORM
#define _SM_SSWR_AVIREAD_AVIRNETINFOFORM
#include "Net/ConnectionInfo.h"
#include "Net/WirelessLAN.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetInfoForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpAdaptor;
			NN<UI::GUITabPage> tpIPInfo;
			NN<UI::GUITabPage> tpTCPInfo;
			NN<UI::GUITabPage> tpUDPInfo;
			NN<UI::GUITabPage> tpARPInfo;
			NN<UI::GUITabPage> tpWIFIInfo;
			NN<UI::GUITabPage> tpPortInfo;

			NN<UI::GUIListBox> lbAdaptors;
			NN<UI::GUIHSplitter> hSplitter;
			NN<UI::GUIPanel> pnlAdaptor;

			NN<UI::GUILabel> lblAdaptorName;
			NN<UI::GUITextBox> txtAdaptorName;
			NN<UI::GUILabel> lblAdaptorDesc;
			NN<UI::GUITextBox> txtAdaptorDesc;
			NN<UI::GUILabel> lblAdaptorDNSSuffix;
			NN<UI::GUITextBox> txtAdaptorDNSSuffix;
			NN<UI::GUILabel> lblAdaptorConnType;
			NN<UI::GUITextBox> txtAdaptorConnType;
			NN<UI::GUILabel> lblAdaptorMTU;
			NN<UI::GUITextBox> txtAdaptorMTU;
			NN<UI::GUILabel> lblAdaptorPhysicalAddr;
			NN<UI::GUITextBox> txtAdaptorPhysicalAddr;
			NN<UI::GUILabel> lblAdaptorMediaState;
			NN<UI::GUITextBox> txtAdaptorMediaState;
			NN<UI::GUIButton> btnAdaptorEnable;
			NN<UI::GUIButton> btnAdaptorDisable;
			NN<UI::GUILabel> lblAdaptorIP;
			NN<UI::GUIListBox> lbAdaptorIP;
			NN<UI::GUILabel> lblAdaptorGW;
			NN<UI::GUITextBox> txtAdaptorGW;
			NN<UI::GUILabel> lblAdaptorDNS;
			NN<UI::GUIListBox> lbAdaptorDNS;
			NN<UI::GUILabel> lblAdaptorDHCPEnable;
			NN<UI::GUITextBox> txtAdaptorDHCPEnable;
			NN<UI::GUILabel> lblAdaptorDHCPServer;
			NN<UI::GUITextBox> txtAdaptorDHCPServer;
			NN<UI::GUILabel> lblAdaptorDHCPLeaseTime;
			NN<UI::GUITextBox> txtAdaptorDHCPLeaseTime;
			NN<UI::GUILabel> lblAdaptorDHCPLeaseExpire;
			NN<UI::GUITextBox> txtAdaptorDHCPLeaseExpire;

			NN<UI::GUILabel> lblIPStatForwarding;
			NN<UI::GUITextBox> txtIPStatForwarding;
			NN<UI::GUILabel> lblIPStatDefTTL;
			NN<UI::GUITextBox> txtIPStatDefTTL;
			NN<UI::GUILabel> lblIPStatNRecv;
			NN<UI::GUITextBox> txtIPStatNRecv;
			NN<UI::GUILabel> lblIPStatNHdrError;
			NN<UI::GUITextBox> txtIPStatNHdrError;
			NN<UI::GUILabel> lblIPStatNAddrError;
			NN<UI::GUITextBox> txtIPStatNAddrError;
			NN<UI::GUILabel> lblIPStatNForwDatag;
			NN<UI::GUITextBox> txtIPStatNForwDatag;
			NN<UI::GUILabel> lblIPStatNUnkProtos;
			NN<UI::GUITextBox> txtIPStatNUnkProtos;
			NN<UI::GUILabel> lblIPStatNDiscard;
			NN<UI::GUITextBox> txtIPStatNDiscard;
			NN<UI::GUILabel> lblIPStatNDeliver;
			NN<UI::GUITextBox> txtIPStatNDeliver;
			NN<UI::GUILabel> lblIPStatNOutRequest;
			NN<UI::GUITextBox> txtIPStatNOutRequest;
			NN<UI::GUILabel> lblIPStatNRoutingDiscard;
			NN<UI::GUITextBox> txtIPStatNRoutingDiscard;
			NN<UI::GUILabel> lblIPStatNOutDiscard;
			NN<UI::GUITextBox> txtIPStatNOutDiscard;
			NN<UI::GUILabel> lblIPStatNOutNoRoute;
			NN<UI::GUITextBox> txtIPStatNOutNoRoute;
			NN<UI::GUILabel> lblIPStatReasmTimeout;
			NN<UI::GUITextBox> txtIPStatReasmTimeout;
			NN<UI::GUILabel> lblIPStatNReasmReqds;
			NN<UI::GUITextBox> txtIPStatNReasmReqds;
			NN<UI::GUILabel> lblIPStatNReasmOk;
			NN<UI::GUITextBox> txtIPStatNReasmOk;
			NN<UI::GUILabel> lblIPStatNReasmFail;
			NN<UI::GUITextBox> txtIPStatNReasmFail;
			NN<UI::GUILabel> lblIPStatNFragOk;
			NN<UI::GUITextBox> txtIPStatNFragOksl;
			NN<UI::GUILabel> lblIPStatNFragFail;
			NN<UI::GUITextBox> txtIPStatNFragFail;
			NN<UI::GUILabel> lblIPStatNFragCreate;
			NN<UI::GUITextBox> txtIPStatNFragCreate;
			NN<UI::GUILabel> lblIPStatNIf;
			NN<UI::GUITextBox> txtIPStatNIf;
			NN<UI::GUILabel> lblIPStatNAddr;
			NN<UI::GUITextBox> txtIPStatNAddr;
			NN<UI::GUILabel> lblIPStatNRoute;
			NN<UI::GUITextBox> txtIPStatNRoute;

			NN<UI::GUILabel> lblTCPStatRtoAlgorithm;
			NN<UI::GUITextBox> txtTCPStatRtoAlgorithm;
			NN<UI::GUILabel> lblTCPStatRtoMin;
			NN<UI::GUITextBox> txtTCPStatRtoMin;
			NN<UI::GUILabel> lblTCPStatRtoMax;
			NN<UI::GUITextBox> txtTCPStatRtoMax;
			NN<UI::GUILabel> lblTCPStatMaxConn;
			NN<UI::GUITextBox> txtTCPStatMaxConn;
			NN<UI::GUILabel> lblTCPStatActiveOpens;
			NN<UI::GUITextBox> txtTCPStatActiveOpens;
			NN<UI::GUILabel> lblTCPStatPassiveOpens;
			NN<UI::GUITextBox> txtTCPStatPassiveOpens;
			NN<UI::GUILabel> lblTCPStatAttemptFails;
			NN<UI::GUITextBox> txtTCPStatAttemptFails;
			NN<UI::GUILabel> lblTCPStatEstabResets;
			NN<UI::GUITextBox> txtTCPStatEstabResets;
			NN<UI::GUILabel> lblTCPStatCurrEstab;
			NN<UI::GUITextBox> txtTCPStatCurrEstab;
			NN<UI::GUILabel> lblTCPStatInSegs;
			NN<UI::GUITextBox> txtTCPStatInSegs;
			NN<UI::GUILabel> lblTCPStatOutSegs;
			NN<UI::GUITextBox> txtTCPStatOutSegs;
			NN<UI::GUILabel> lblTCPStatRetransSegs;
			NN<UI::GUITextBox> txtTCPStatRetransSeg;
			NN<UI::GUILabel> lblTCPStatInErrs;
			NN<UI::GUITextBox> txtTCPStatInErrs;
			NN<UI::GUILabel> lblTCPStatOutRsts;
			NN<UI::GUITextBox> txtTCPStatOutRsts;
			NN<UI::GUILabel> lblTCPStatNumConns;
			NN<UI::GUITextBox> txtTCPStatNumConns;

			NN<UI::GUILabel> lblUDPStatInDatagrams;
			NN<UI::GUITextBox> txtUDPStatInDatagrams;
			NN<UI::GUILabel> lblUDPStatNoPorts;
			NN<UI::GUITextBox> txtUDPStatNoPorts;
			NN<UI::GUILabel> lblUDPStatInErrors;
			NN<UI::GUITextBox> txtUDPStatInErrors;
			NN<UI::GUILabel> lblUDPStatOutDatagrams;
			NN<UI::GUITextBox> txtUDPStatOutDatagrams;
			NN<UI::GUILabel> lblUDPStatNumAddrs;
			NN<UI::GUITextBox> txtUDPStatNumAddrs;

			NN<UI::GUIListView> lvARPInfo;

			NN<UI::GUIListBox> lbWIFIIFs;
			NN<UI::GUIHSplitter> hspWIFI;
			NN<UI::GUITabControl> tcWIFI;
			NN<UI::GUITabPage> tpWIFIAP;
			NN<UI::GUIListView> lvWIFINetwork;
			NN<UI::GUITabPage> tpWIFIBSS;
			NN<UI::GUIListView> lvWIFIBSS;

			NN<UI::GUIPanel> pnlPortInfo;
			NN<UI::GUIButton> btnPortRefresh;
			NN<UI::GUICheckBox> chkPortAuto;
			NN<UI::GUIListView> lvPortInfo;

			NN<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListNN<Net::ConnectionInfo> conns;
			Data::ArrayListNN<Net::WirelessLAN::Interface> wlanIfs;
			Net::WirelessLAN wlan;
			OSInt wlanScanCnt;

			static void __stdcall OnAdaptorSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPortClicked(AnyType userObj);
			static void __stdcall OnAdaptorEnableClicked(AnyType userObj);
			static void __stdcall OnAdaptorDisableClicked(AnyType userObj);

			void UpdateIPStats();
			void UpdateTCPStats();
			void UpdateUDPStats();
			void UpdateARPStats();
			void ReleaseWIFIIFs();
			void UpdateWIFIIFs();
			void UpdateWIFINetworks();
			void ReleaseConns();
			void UpdateConns();
			void UpdatePortStats();
		public:
			AVIRNetInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
