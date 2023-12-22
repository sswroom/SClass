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
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpAdaptor;
			NotNullPtr<UI::GUITabPage> tpIPInfo;
			NotNullPtr<UI::GUITabPage> tpTCPInfo;
			NotNullPtr<UI::GUITabPage> tpUDPInfo;
			NotNullPtr<UI::GUITabPage> tpARPInfo;
			NotNullPtr<UI::GUITabPage> tpWIFIInfo;
			NotNullPtr<UI::GUITabPage> tpPortInfo;

			NotNullPtr<UI::GUIListBox> lbAdaptors;
			NotNullPtr<UI::GUIHSplitter> hSplitter;
			NotNullPtr<UI::GUIPanel> pnlAdaptor;

			NotNullPtr<UI::GUILabel> lblAdaptorName;
			NotNullPtr<UI::GUITextBox> txtAdaptorName;
			NotNullPtr<UI::GUILabel> lblAdaptorDesc;
			NotNullPtr<UI::GUITextBox> txtAdaptorDesc;
			NotNullPtr<UI::GUILabel> lblAdaptorDNSSuffix;
			NotNullPtr<UI::GUITextBox> txtAdaptorDNSSuffix;
			NotNullPtr<UI::GUILabel> lblAdaptorConnType;
			NotNullPtr<UI::GUITextBox> txtAdaptorConnType;
			NotNullPtr<UI::GUILabel> lblAdaptorMTU;
			NotNullPtr<UI::GUITextBox> txtAdaptorMTU;
			NotNullPtr<UI::GUILabel> lblAdaptorPhysicalAddr;
			NotNullPtr<UI::GUITextBox> txtAdaptorPhysicalAddr;
			NotNullPtr<UI::GUILabel> lblAdaptorMediaState;
			NotNullPtr<UI::GUITextBox> txtAdaptorMediaState;
			NotNullPtr<UI::GUIButton> btnAdaptorEnable;
			NotNullPtr<UI::GUIButton> btnAdaptorDisable;
			NotNullPtr<UI::GUILabel> lblAdaptorIP;
			NotNullPtr<UI::GUIListBox> lbAdaptorIP;
			NotNullPtr<UI::GUILabel> lblAdaptorGW;
			NotNullPtr<UI::GUITextBox> txtAdaptorGW;
			NotNullPtr<UI::GUILabel> lblAdaptorDNS;
			NotNullPtr<UI::GUIListBox> lbAdaptorDNS;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPEnable;
			NotNullPtr<UI::GUITextBox> txtAdaptorDHCPEnable;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPServer;
			NotNullPtr<UI::GUITextBox> txtAdaptorDHCPServer;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPLeaseTime;
			NotNullPtr<UI::GUITextBox> txtAdaptorDHCPLeaseTime;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPLeaseExpire;
			NotNullPtr<UI::GUITextBox> txtAdaptorDHCPLeaseExpire;

			NotNullPtr<UI::GUILabel> lblIPStatForwarding;
			NotNullPtr<UI::GUITextBox> txtIPStatForwarding;
			NotNullPtr<UI::GUILabel> lblIPStatDefTTL;
			NotNullPtr<UI::GUITextBox> txtIPStatDefTTL;
			NotNullPtr<UI::GUILabel> lblIPStatNRecv;
			NotNullPtr<UI::GUITextBox> txtIPStatNRecv;
			NotNullPtr<UI::GUILabel> lblIPStatNHdrError;
			NotNullPtr<UI::GUITextBox> txtIPStatNHdrError;
			NotNullPtr<UI::GUILabel> lblIPStatNAddrError;
			NotNullPtr<UI::GUITextBox> txtIPStatNAddrError;
			NotNullPtr<UI::GUILabel> lblIPStatNForwDatag;
			NotNullPtr<UI::GUITextBox> txtIPStatNForwDatag;
			NotNullPtr<UI::GUILabel> lblIPStatNUnkProtos;
			NotNullPtr<UI::GUITextBox> txtIPStatNUnkProtos;
			NotNullPtr<UI::GUILabel> lblIPStatNDiscard;
			NotNullPtr<UI::GUITextBox> txtIPStatNDiscard;
			NotNullPtr<UI::GUILabel> lblIPStatNDeliver;
			NotNullPtr<UI::GUITextBox> txtIPStatNDeliver;
			NotNullPtr<UI::GUILabel> lblIPStatNOutRequest;
			NotNullPtr<UI::GUITextBox> txtIPStatNOutRequest;
			NotNullPtr<UI::GUILabel> lblIPStatNRoutingDiscard;
			NotNullPtr<UI::GUITextBox> txtIPStatNRoutingDiscard;
			NotNullPtr<UI::GUILabel> lblIPStatNOutDiscard;
			NotNullPtr<UI::GUITextBox> txtIPStatNOutDiscard;
			NotNullPtr<UI::GUILabel> lblIPStatNOutNoRoute;
			NotNullPtr<UI::GUITextBox> txtIPStatNOutNoRoute;
			NotNullPtr<UI::GUILabel> lblIPStatReasmTimeout;
			NotNullPtr<UI::GUITextBox> txtIPStatReasmTimeout;
			NotNullPtr<UI::GUILabel> lblIPStatNReasmReqds;
			NotNullPtr<UI::GUITextBox> txtIPStatNReasmReqds;
			NotNullPtr<UI::GUILabel> lblIPStatNReasmOk;
			NotNullPtr<UI::GUITextBox> txtIPStatNReasmOk;
			NotNullPtr<UI::GUILabel> lblIPStatNReasmFail;
			NotNullPtr<UI::GUITextBox> txtIPStatNReasmFail;
			NotNullPtr<UI::GUILabel> lblIPStatNFragOk;
			NotNullPtr<UI::GUITextBox> txtIPStatNFragOksl;
			NotNullPtr<UI::GUILabel> lblIPStatNFragFail;
			NotNullPtr<UI::GUITextBox> txtIPStatNFragFail;
			NotNullPtr<UI::GUILabel> lblIPStatNFragCreate;
			NotNullPtr<UI::GUITextBox> txtIPStatNFragCreate;
			NotNullPtr<UI::GUILabel> lblIPStatNIf;
			NotNullPtr<UI::GUITextBox> txtIPStatNIf;
			NotNullPtr<UI::GUILabel> lblIPStatNAddr;
			NotNullPtr<UI::GUITextBox> txtIPStatNAddr;
			NotNullPtr<UI::GUILabel> lblIPStatNRoute;
			NotNullPtr<UI::GUITextBox> txtIPStatNRoute;

			NotNullPtr<UI::GUILabel> lblTCPStatRtoAlgorithm;
			NotNullPtr<UI::GUITextBox> txtTCPStatRtoAlgorithm;
			NotNullPtr<UI::GUILabel> lblTCPStatRtoMin;
			NotNullPtr<UI::GUITextBox> txtTCPStatRtoMin;
			NotNullPtr<UI::GUILabel> lblTCPStatRtoMax;
			NotNullPtr<UI::GUITextBox> txtTCPStatRtoMax;
			NotNullPtr<UI::GUILabel> lblTCPStatMaxConn;
			NotNullPtr<UI::GUITextBox> txtTCPStatMaxConn;
			NotNullPtr<UI::GUILabel> lblTCPStatActiveOpens;
			NotNullPtr<UI::GUITextBox> txtTCPStatActiveOpens;
			NotNullPtr<UI::GUILabel> lblTCPStatPassiveOpens;
			NotNullPtr<UI::GUITextBox> txtTCPStatPassiveOpens;
			NotNullPtr<UI::GUILabel> lblTCPStatAttemptFails;
			NotNullPtr<UI::GUITextBox> txtTCPStatAttemptFails;
			NotNullPtr<UI::GUILabel> lblTCPStatEstabResets;
			NotNullPtr<UI::GUITextBox> txtTCPStatEstabResets;
			NotNullPtr<UI::GUILabel> lblTCPStatCurrEstab;
			NotNullPtr<UI::GUITextBox> txtTCPStatCurrEstab;
			NotNullPtr<UI::GUILabel> lblTCPStatInSegs;
			NotNullPtr<UI::GUITextBox> txtTCPStatInSegs;
			NotNullPtr<UI::GUILabel> lblTCPStatOutSegs;
			NotNullPtr<UI::GUITextBox> txtTCPStatOutSegs;
			NotNullPtr<UI::GUILabel> lblTCPStatRetransSegs;
			NotNullPtr<UI::GUITextBox> txtTCPStatRetransSeg;
			NotNullPtr<UI::GUILabel> lblTCPStatInErrs;
			NotNullPtr<UI::GUITextBox> txtTCPStatInErrs;
			NotNullPtr<UI::GUILabel> lblTCPStatOutRsts;
			NotNullPtr<UI::GUITextBox> txtTCPStatOutRsts;
			NotNullPtr<UI::GUILabel> lblTCPStatNumConns;
			NotNullPtr<UI::GUITextBox> txtTCPStatNumConns;

			NotNullPtr<UI::GUILabel> lblUDPStatInDatagrams;
			NotNullPtr<UI::GUITextBox> txtUDPStatInDatagrams;
			NotNullPtr<UI::GUILabel> lblUDPStatNoPorts;
			NotNullPtr<UI::GUITextBox> txtUDPStatNoPorts;
			NotNullPtr<UI::GUILabel> lblUDPStatInErrors;
			NotNullPtr<UI::GUITextBox> txtUDPStatInErrors;
			NotNullPtr<UI::GUILabel> lblUDPStatOutDatagrams;
			NotNullPtr<UI::GUITextBox> txtUDPStatOutDatagrams;
			NotNullPtr<UI::GUILabel> lblUDPStatNumAddrs;
			NotNullPtr<UI::GUITextBox> txtUDPStatNumAddrs;

			NotNullPtr<UI::GUIListView> lvARPInfo;

			NotNullPtr<UI::GUIListBox> lbWIFIIFs;
			NotNullPtr<UI::GUIHSplitter> hspWIFI;
			NotNullPtr<UI::GUITabControl> tcWIFI;
			NotNullPtr<UI::GUITabPage> tpWIFIAP;
			NotNullPtr<UI::GUIListView> lvWIFINetwork;
			NotNullPtr<UI::GUITabPage> tpWIFIBSS;
			NotNullPtr<UI::GUIListView> lvWIFIBSS;

			NotNullPtr<UI::GUIPanel> pnlPortInfo;
			NotNullPtr<UI::GUIButton> btnPortRefresh;
			NotNullPtr<UI::GUICheckBox> chkPortAuto;
			NotNullPtr<UI::GUIListView> lvPortInfo;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayList<Net::ConnectionInfo *> *conns;
			Data::ArrayList<Net::WirelessLAN::Interface *> *wlanIfs;
			Net::WirelessLAN *wlan;
			OSInt wlanScanCnt;

			static void __stdcall OnAdaptorSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPortClicked(void *userObj);
			static void __stdcall OnAdaptorEnableClicked(void *userObj);
			static void __stdcall OnAdaptorDisableClicked(void *userObj);

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
			AVIRNetInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
