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
			UI::GUITabControl *tcMain;
			NotNullPtr<UI::GUITabPage> tpAdaptor;
			NotNullPtr<UI::GUITabPage> tpIPInfo;
			NotNullPtr<UI::GUITabPage> tpTCPInfo;
			NotNullPtr<UI::GUITabPage> tpUDPInfo;
			NotNullPtr<UI::GUITabPage> tpARPInfo;
			NotNullPtr<UI::GUITabPage> tpWIFIInfo;
			NotNullPtr<UI::GUITabPage> tpPortInfo;

			UI::GUIListBox *lbAdaptors;
			NotNullPtr<UI::GUIHSplitter> hSplitter;
			NotNullPtr<UI::GUIPanel> pnlAdaptor;

			NotNullPtr<UI::GUILabel> lblAdaptorName;
			UI::GUITextBox *txtAdaptorName;
			NotNullPtr<UI::GUILabel> lblAdaptorDesc;
			UI::GUITextBox *txtAdaptorDesc;
			NotNullPtr<UI::GUILabel> lblAdaptorDNSSuffix;
			UI::GUITextBox *txtAdaptorDNSSuffix;
			NotNullPtr<UI::GUILabel> lblAdaptorConnType;
			UI::GUITextBox *txtAdaptorConnType;
			NotNullPtr<UI::GUILabel> lblAdaptorMTU;
			UI::GUITextBox *txtAdaptorMTU;
			NotNullPtr<UI::GUILabel> lblAdaptorPhysicalAddr;
			UI::GUITextBox *txtAdaptorPhysicalAddr;
			NotNullPtr<UI::GUILabel> lblAdaptorMediaState;
			UI::GUITextBox *txtAdaptorMediaState;
			NotNullPtr<UI::GUIButton> btnAdaptorEnable;
			NotNullPtr<UI::GUIButton> btnAdaptorDisable;
			NotNullPtr<UI::GUILabel> lblAdaptorIP;
			UI::GUIListBox *lbAdaptorIP;
			NotNullPtr<UI::GUILabel> lblAdaptorGW;
			UI::GUITextBox *txtAdaptorGW;
			NotNullPtr<UI::GUILabel> lblAdaptorDNS;
			UI::GUIListBox *lbAdaptorDNS;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPEnable;
			UI::GUITextBox *txtAdaptorDHCPEnable;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPServer;
			UI::GUITextBox *txtAdaptorDHCPServer;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPLeaseTime;
			UI::GUITextBox *txtAdaptorDHCPLeaseTime;
			NotNullPtr<UI::GUILabel> lblAdaptorDHCPLeaseExpire;
			UI::GUITextBox *txtAdaptorDHCPLeaseExpire;

			NotNullPtr<UI::GUILabel> lblIPStatForwarding;
			UI::GUITextBox *txtIPStatForwarding;
			NotNullPtr<UI::GUILabel> lblIPStatDefTTL;
			UI::GUITextBox *txtIPStatDefTTL;
			NotNullPtr<UI::GUILabel> lblIPStatNRecv;
			UI::GUITextBox *txtIPStatNRecv;
			NotNullPtr<UI::GUILabel> lblIPStatNHdrError;
			UI::GUITextBox *txtIPStatNHdrError;
			NotNullPtr<UI::GUILabel> lblIPStatNAddrError;
			UI::GUITextBox *txtIPStatNAddrError;
			NotNullPtr<UI::GUILabel> lblIPStatNForwDatag;
			UI::GUITextBox *txtIPStatNForwDatag;
			NotNullPtr<UI::GUILabel> lblIPStatNUnkProtos;
			UI::GUITextBox *txtIPStatNUnkProtos;
			NotNullPtr<UI::GUILabel> lblIPStatNDiscard;
			UI::GUITextBox *txtIPStatNDiscard;
			NotNullPtr<UI::GUILabel> lblIPStatNDeliver;
			UI::GUITextBox *txtIPStatNDeliver;
			NotNullPtr<UI::GUILabel> lblIPStatNOutRequest;
			UI::GUITextBox *txtIPStatNOutRequest;
			NotNullPtr<UI::GUILabel> lblIPStatNRoutingDiscard;
			UI::GUITextBox *txtIPStatNRoutingDiscard;
			NotNullPtr<UI::GUILabel> lblIPStatNOutDiscard;
			UI::GUITextBox *txtIPStatNOutDiscard;
			NotNullPtr<UI::GUILabel> lblIPStatNOutNoRoute;
			UI::GUITextBox *txtIPStatNOutNoRoute;
			NotNullPtr<UI::GUILabel> lblIPStatReasmTimeout;
			UI::GUITextBox *txtIPStatReasmTimeout;
			NotNullPtr<UI::GUILabel> lblIPStatNReasmReqds;
			UI::GUITextBox *txtIPStatNReasmReqds;
			NotNullPtr<UI::GUILabel> lblIPStatNReasmOk;
			UI::GUITextBox *txtIPStatNReasmOk;
			NotNullPtr<UI::GUILabel> lblIPStatNReasmFail;
			UI::GUITextBox *txtIPStatNReasmFail;
			NotNullPtr<UI::GUILabel> lblIPStatNFragOk;
			UI::GUITextBox *txtIPStatNFragOksl;
			NotNullPtr<UI::GUILabel> lblIPStatNFragFail;
			UI::GUITextBox *txtIPStatNFragFail;
			NotNullPtr<UI::GUILabel> lblIPStatNFragCreate;
			UI::GUITextBox *txtIPStatNFragCreate;
			NotNullPtr<UI::GUILabel> lblIPStatNIf;
			UI::GUITextBox *txtIPStatNIf;
			NotNullPtr<UI::GUILabel> lblIPStatNAddr;
			UI::GUITextBox *txtIPStatNAddr;
			NotNullPtr<UI::GUILabel> lblIPStatNRoute;
			UI::GUITextBox *txtIPStatNRoute;

			NotNullPtr<UI::GUILabel> lblTCPStatRtoAlgorithm;
			UI::GUITextBox *txtTCPStatRtoAlgorithm;
			NotNullPtr<UI::GUILabel> lblTCPStatRtoMin;
			UI::GUITextBox *txtTCPStatRtoMin;
			NotNullPtr<UI::GUILabel> lblTCPStatRtoMax;
			UI::GUITextBox *txtTCPStatRtoMax;
			NotNullPtr<UI::GUILabel> lblTCPStatMaxConn;
			UI::GUITextBox *txtTCPStatMaxConn;
			NotNullPtr<UI::GUILabel> lblTCPStatActiveOpens;
			UI::GUITextBox *txtTCPStatActiveOpens;
			NotNullPtr<UI::GUILabel> lblTCPStatPassiveOpens;
			UI::GUITextBox *txtTCPStatPassiveOpens;
			NotNullPtr<UI::GUILabel> lblTCPStatAttemptFails;
			UI::GUITextBox *txtTCPStatAttemptFails;
			NotNullPtr<UI::GUILabel> lblTCPStatEstabResets;
			UI::GUITextBox *txtTCPStatEstabResets;
			NotNullPtr<UI::GUILabel> lblTCPStatCurrEstab;
			UI::GUITextBox *txtTCPStatCurrEstab;
			NotNullPtr<UI::GUILabel> lblTCPStatInSegs;
			UI::GUITextBox *txtTCPStatInSegs;
			NotNullPtr<UI::GUILabel> lblTCPStatOutSegs;
			UI::GUITextBox *txtTCPStatOutSegs;
			NotNullPtr<UI::GUILabel> lblTCPStatRetransSegs;
			UI::GUITextBox *txtTCPStatRetransSeg;
			NotNullPtr<UI::GUILabel> lblTCPStatInErrs;
			UI::GUITextBox *txtTCPStatInErrs;
			NotNullPtr<UI::GUILabel> lblTCPStatOutRsts;
			UI::GUITextBox *txtTCPStatOutRsts;
			NotNullPtr<UI::GUILabel> lblTCPStatNumConns;
			UI::GUITextBox *txtTCPStatNumConns;

			NotNullPtr<UI::GUILabel> lblUDPStatInDatagrams;
			UI::GUITextBox *txtUDPStatInDatagrams;
			NotNullPtr<UI::GUILabel> lblUDPStatNoPorts;
			UI::GUITextBox *txtUDPStatNoPorts;
			NotNullPtr<UI::GUILabel> lblUDPStatInErrors;
			UI::GUITextBox *txtUDPStatInErrors;
			NotNullPtr<UI::GUILabel> lblUDPStatOutDatagrams;
			UI::GUITextBox *txtUDPStatOutDatagrams;
			NotNullPtr<UI::GUILabel> lblUDPStatNumAddrs;
			UI::GUITextBox *txtUDPStatNumAddrs;

			UI::GUIListView *lvARPInfo;

			UI::GUIListBox *lbWIFIIFs;
			NotNullPtr<UI::GUIHSplitter> hspWIFI;
			UI::GUITabControl *tcWIFI;
			NotNullPtr<UI::GUITabPage> tpWIFIAP;
			UI::GUIListView *lvWIFINetwork;
			NotNullPtr<UI::GUITabPage> tpWIFIBSS;
			UI::GUIListView *lvWIFIBSS;

			NotNullPtr<UI::GUIPanel> pnlPortInfo;
			NotNullPtr<UI::GUIButton> btnPortRefresh;
			UI::GUICheckBox *chkPortAuto;
			UI::GUIListView *lvPortInfo;

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
