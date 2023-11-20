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
			UI::GUIHSplitter *hSplitter;
			NotNullPtr<UI::GUIPanel> pnlAdaptor;

			UI::GUILabel *lblAdaptorName;
			UI::GUITextBox *txtAdaptorName;
			UI::GUILabel *lblAdaptorDesc;
			UI::GUITextBox *txtAdaptorDesc;
			UI::GUILabel *lblAdaptorDNSSuffix;
			UI::GUITextBox *txtAdaptorDNSSuffix;
			UI::GUILabel *lblAdaptorConnType;
			UI::GUITextBox *txtAdaptorConnType;
			UI::GUILabel *lblAdaptorMTU;
			UI::GUITextBox *txtAdaptorMTU;
			UI::GUILabel *lblAdaptorPhysicalAddr;
			UI::GUITextBox *txtAdaptorPhysicalAddr;
			UI::GUILabel *lblAdaptorMediaState;
			UI::GUITextBox *txtAdaptorMediaState;
			UI::GUIButton *btnAdaptorEnable;
			UI::GUIButton *btnAdaptorDisable;
			UI::GUILabel *lblAdaptorIP;
			UI::GUIListBox *lbAdaptorIP;
			UI::GUILabel *lblAdaptorGW;
			UI::GUITextBox *txtAdaptorGW;
			UI::GUILabel *lblAdaptorDNS;
			UI::GUIListBox *lbAdaptorDNS;
			UI::GUILabel *lblAdaptorDHCPEnable;
			UI::GUITextBox *txtAdaptorDHCPEnable;
			UI::GUILabel *lblAdaptorDHCPServer;
			UI::GUITextBox *txtAdaptorDHCPServer;
			UI::GUILabel *lblAdaptorDHCPLeaseTime;
			UI::GUITextBox *txtAdaptorDHCPLeaseTime;
			UI::GUILabel *lblAdaptorDHCPLeaseExpire;
			UI::GUITextBox *txtAdaptorDHCPLeaseExpire;

			UI::GUILabel *lblIPStatForwarding;
			UI::GUITextBox *txtIPStatForwarding;
			UI::GUILabel *lblIPStatDefTTL;
			UI::GUITextBox *txtIPStatDefTTL;
			UI::GUILabel *lblIPStatNRecv;
			UI::GUITextBox *txtIPStatNRecv;
			UI::GUILabel *lblIPStatNHdrError;
			UI::GUITextBox *txtIPStatNHdrError;
			UI::GUILabel *lblIPStatNAddrError;
			UI::GUITextBox *txtIPStatNAddrError;
			UI::GUILabel *lblIPStatNForwDatag;
			UI::GUITextBox *txtIPStatNForwDatag;
			UI::GUILabel *lblIPStatNUnkProtos;
			UI::GUITextBox *txtIPStatNUnkProtos;
			UI::GUILabel *lblIPStatNDiscard;
			UI::GUITextBox *txtIPStatNDiscard;
			UI::GUILabel *lblIPStatNDeliver;
			UI::GUITextBox *txtIPStatNDeliver;
			UI::GUILabel *lblIPStatNOutRequest;
			UI::GUITextBox *txtIPStatNOutRequest;
			UI::GUILabel *lblIPStatNRoutingDiscard;
			UI::GUITextBox *txtIPStatNRoutingDiscard;
			UI::GUILabel *lblIPStatNOutDiscard;
			UI::GUITextBox *txtIPStatNOutDiscard;
			UI::GUILabel *lblIPStatNOutNoRoute;
			UI::GUITextBox *txtIPStatNOutNoRoute;
			UI::GUILabel *lblIPStatReasmTimeout;
			UI::GUITextBox *txtIPStatReasmTimeout;
			UI::GUILabel *lblIPStatNReasmReqds;
			UI::GUITextBox *txtIPStatNReasmReqds;
			UI::GUILabel *lblIPStatNReasmOk;
			UI::GUITextBox *txtIPStatNReasmOk;
			UI::GUILabel *lblIPStatNReasmFail;
			UI::GUITextBox *txtIPStatNReasmFail;
			UI::GUILabel *lblIPStatNFragOk;
			UI::GUITextBox *txtIPStatNFragOksl;
			UI::GUILabel *lblIPStatNFragFail;
			UI::GUITextBox *txtIPStatNFragFail;
			UI::GUILabel *lblIPStatNFragCreate;
			UI::GUITextBox *txtIPStatNFragCreate;
			UI::GUILabel *lblIPStatNIf;
			UI::GUITextBox *txtIPStatNIf;
			UI::GUILabel *lblIPStatNAddr;
			UI::GUITextBox *txtIPStatNAddr;
			UI::GUILabel *lblIPStatNRoute;
			UI::GUITextBox *txtIPStatNRoute;

			UI::GUILabel *lblTCPStatRtoAlgorithm;
			UI::GUITextBox *txtTCPStatRtoAlgorithm;
			UI::GUILabel *lblTCPStatRtoMin;
			UI::GUITextBox *txtTCPStatRtoMin;
			UI::GUILabel *lblTCPStatRtoMax;
			UI::GUITextBox *txtTCPStatRtoMax;
			UI::GUILabel *lblTCPStatMaxConn;
			UI::GUITextBox *txtTCPStatMaxConn;
			UI::GUILabel *lblTCPStatActiveOpens;
			UI::GUITextBox *txtTCPStatActiveOpens;
			UI::GUILabel *lblTCPStatPassiveOpens;
			UI::GUITextBox *txtTCPStatPassiveOpens;
			UI::GUILabel *lblTCPStatAttemptFails;
			UI::GUITextBox *txtTCPStatAttemptFails;
			UI::GUILabel *lblTCPStatEstabResets;
			UI::GUITextBox *txtTCPStatEstabResets;
			UI::GUILabel *lblTCPStatCurrEstab;
			UI::GUITextBox *txtTCPStatCurrEstab;
			UI::GUILabel *lblTCPStatInSegs;
			UI::GUITextBox *txtTCPStatInSegs;
			UI::GUILabel *lblTCPStatOutSegs;
			UI::GUITextBox *txtTCPStatOutSegs;
			UI::GUILabel *lblTCPStatRetransSegs;
			UI::GUITextBox *txtTCPStatRetransSeg;
			UI::GUILabel *lblTCPStatInErrs;
			UI::GUITextBox *txtTCPStatInErrs;
			UI::GUILabel *lblTCPStatOutRsts;
			UI::GUITextBox *txtTCPStatOutRsts;
			UI::GUILabel *lblTCPStatNumConns;
			UI::GUITextBox *txtTCPStatNumConns;

			UI::GUILabel *lblUDPStatInDatagrams;
			UI::GUITextBox *txtUDPStatInDatagrams;
			UI::GUILabel *lblUDPStatNoPorts;
			UI::GUITextBox *txtUDPStatNoPorts;
			UI::GUILabel *lblUDPStatInErrors;
			UI::GUITextBox *txtUDPStatInErrors;
			UI::GUILabel *lblUDPStatOutDatagrams;
			UI::GUITextBox *txtUDPStatOutDatagrams;
			UI::GUILabel *lblUDPStatNumAddrs;
			UI::GUITextBox *txtUDPStatNumAddrs;

			UI::GUIListView *lvARPInfo;

			UI::GUIListBox *lbWIFIIFs;
			UI::GUIHSplitter *hspWIFI;
			UI::GUITabControl *tcWIFI;
			NotNullPtr<UI::GUITabPage> tpWIFIAP;
			UI::GUIListView *lvWIFINetwork;
			NotNullPtr<UI::GUITabPage> tpWIFIBSS;
			UI::GUIListView *lvWIFIBSS;

			NotNullPtr<UI::GUIPanel> pnlPortInfo;
			UI::GUIButton *btnPortRefresh;
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
