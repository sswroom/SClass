#ifndef _SM_SSWR_AVIREAD_AVIRDNSPROXYFORM
#define _SM_SSWR_AVIREAD_AVIRDNSPROXYFORM

#include "Net/DNSProxy.h"
#include "Net/WhoisHandler.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
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
		class AVIRDNSProxyForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Int32 year;
				Int32 month;
				Int32 day;
				Int32 hour;
				Int64 reqCount;
			} HourInfo;

			struct ClientInfo
			{
				UInt32 cliId;
				Net::SocketUtil::AddressInfo addr;
				Sync::Mutex mut;
				Data::ArrayList<HourInfo*> hourInfos;
			};
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpV4Main;
			UI::GUIListBox *lbV4Request;
			NotNullPtr<UI::GUIHSplitter> hspV4Request;
			NotNullPtr<UI::GUIPanel> pnlV4Request;
			NotNullPtr<UI::GUILabel> lblV4RequestTime;
			UI::GUITextBox *txtV4RequestTime;
			NotNullPtr<UI::GUILabel> lblV4RequestTTL;
			UI::GUITextBox *txtV4RequestTTL;
			UI::GUIListBox *lbV4Answer;
			NotNullPtr<UI::GUIHSplitter> hspV4Answer;
			NotNullPtr<UI::GUIPanel> pnlV4Detail;
			NotNullPtr<UI::GUILabel> lblV4AnsName;
			UI::GUITextBox *txtV4AnsName;
			NotNullPtr<UI::GUILabel> lblV4AnsType;
			UI::GUITextBox *txtV4AnsType;
			NotNullPtr<UI::GUILabel> lblV4AnsClass;
			UI::GUITextBox *txtV4AnsClass;
			NotNullPtr<UI::GUILabel> lblV4AnsTTL;
			UI::GUITextBox *txtV4AnsTTL;
			NotNullPtr<UI::GUILabel> lblV4AnsRD;
			UI::GUITextBox *txtV4AnsRD;

			NotNullPtr<UI::GUITabPage> tpV6Main;
			UI::GUIListBox *lbV6Request;
			NotNullPtr<UI::GUIHSplitter> hspV6Request;
			NotNullPtr<UI::GUIPanel> pnlV6Request;
			NotNullPtr<UI::GUILabel> lblV6RequestTime;
			UI::GUITextBox *txtV6RequestTime;
			NotNullPtr<UI::GUILabel> lblV6RequestTTL;
			UI::GUITextBox *txtV6RequestTTL;
			UI::GUIListBox *lbV6Answer;
			NotNullPtr<UI::GUIHSplitter> hspV6Answer;
			NotNullPtr<UI::GUIPanel> pnlV6Detail;
			NotNullPtr<UI::GUILabel> lblV6AnsName;
			UI::GUITextBox *txtV6AnsName;
			NotNullPtr<UI::GUILabel> lblV6AnsType;
			UI::GUITextBox *txtV6AnsType;
			NotNullPtr<UI::GUILabel> lblV6AnsClass;
			UI::GUITextBox *txtV6AnsClass;
			NotNullPtr<UI::GUILabel> lblV6AnsTTL;
			UI::GUITextBox *txtV6AnsTTL;
			NotNullPtr<UI::GUILabel> lblV6AnsRD;
			UI::GUITextBox *txtV6AnsRD;

			NotNullPtr<UI::GUITabPage> tpOthMain;
			UI::GUIListBox *lbOthRequest;
			NotNullPtr<UI::GUIHSplitter> hspOthRequest;
			NotNullPtr<UI::GUIPanel> pnlOthRequest;
			NotNullPtr<UI::GUILabel> lblOthRequestTime;
			UI::GUITextBox *txtOthRequestTime;
			NotNullPtr<UI::GUILabel> lblOthRequestTTL;
			UI::GUITextBox *txtOthRequestTTL;
			UI::GUIListBox *lbOthAnswer;
			NotNullPtr<UI::GUIHSplitter> hspOthAnswer;
			NotNullPtr<UI::GUIPanel> pnlOthDetail;
			NotNullPtr<UI::GUILabel> lblOthAnsName;
			UI::GUITextBox *txtOthAnsName;
			NotNullPtr<UI::GUILabel> lblOthAnsType;
			UI::GUITextBox *txtOthAnsType;
			NotNullPtr<UI::GUILabel> lblOthAnsClass;
			UI::GUITextBox *txtOthAnsClass;
			NotNullPtr<UI::GUILabel> lblOthAnsTTL;
			UI::GUITextBox *txtOthAnsTTL;
			NotNullPtr<UI::GUILabel> lblOthAnsRD;
			UI::GUITextBox *txtOthAnsRD;

			NotNullPtr<UI::GUITabPage> tpTarget;
			UI::GUIListBox *lbTarget;
			NotNullPtr<UI::GUIHSplitter> hspTarget;
			UI::GUITabControl *tcTarget;
			NotNullPtr<UI::GUITabPage> tpTargetInfo;
			NotNullPtr<UI::GUILabel> lblTargetName;
			UI::GUITextBox *txtTargetName;
			NotNullPtr<UI::GUILabel> lblTargetCountry;
			UI::GUITextBox *txtTargetCountry;
			NotNullPtr<UI::GUILabel> lblTargetDomains;
			UI::GUIListBox *lbTargetDomains;
			NotNullPtr<UI::GUITabPage> tpTargetWhois;
			UI::GUITextBox *txtTargetWhois;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUILabel> lblDNSPort;
			UI::GUITextBox *txtDNSPort;
			NotNullPtr<UI::GUILabel> lblDisableV6;
			UI::GUICheckBox *chkDisableV6;
			NotNullPtr<UI::GUILabel> lblDNSServer;
			UI::GUITextBox *txtDNSServer;
			NotNullPtr<UI::GUIButton> btnDNSSwitch;
			NotNullPtr<UI::GUILabel> lblDNSList;
			UI::GUIListBox *lbDNSList;
			UI::GUITextBox *txtDNSServer2;
			NotNullPtr<UI::GUIButton> btnDNSAdd;
			NotNullPtr<UI::GUIButton> btnDNSSet;
			NotNullPtr<UI::GUILabel> lblWPAD;
			UI::GUITextBox *txtWPAD;
			NotNullPtr<UI::GUIButton> btnWPAD;

			NotNullPtr<UI::GUITabPage> tpSearch;
			NotNullPtr<UI::GUIPanel> pnlSearch;
			NotNullPtr<UI::GUILabel> lblSearchIPRange;
			UI::GUITextBox *txtSearchIPRange;
			NotNullPtr<UI::GUILabel> lblSearchMask;
			UI::GUITextBox *txtSearchMask;
			NotNullPtr<UI::GUIButton> btnSearch;
			UI::GUIListBox *lbSearch;
			NotNullPtr<UI::GUIHSplitter> hspSearch;
			NotNullPtr<UI::GUIPanel> pnlSRequest;
			NotNullPtr<UI::GUILabel> lblSRequestTime;
			UI::GUITextBox *txtSRequestTime;
			NotNullPtr<UI::GUILabel> lblSRequestTTL;
			UI::GUITextBox *txtSRequestTTL;
			UI::GUIListBox *lbSAnswer;
			NotNullPtr<UI::GUIHSplitter> hspSAnswer;
			NotNullPtr<UI::GUIPanel> pnlSDetail;
			NotNullPtr<UI::GUILabel> lblSAnsName;
			UI::GUITextBox *txtSAnsName;
			NotNullPtr<UI::GUILabel> lblSAnsType;
			UI::GUITextBox *txtSAnsType;
			NotNullPtr<UI::GUILabel> lblSAnsClass;
			UI::GUITextBox *txtSAnsClass;
			NotNullPtr<UI::GUILabel> lblSAnsTTL;
			UI::GUITextBox *txtSAnsTTL;
			NotNullPtr<UI::GUILabel> lblSAnsRD;
			UI::GUITextBox *txtSAnsRD;

			NotNullPtr<UI::GUITabPage> tpBlackList;
			NotNullPtr<UI::GUIPanel> pnlBlackList;
			UI::GUIListBox *lbBlackList;
			UI::GUITextBox *txtBlackList;
			NotNullPtr<UI::GUIButton> btnBlackList;

			NotNullPtr<UI::GUITabPage> tpClient;
			UI::GUIListBox *lbClientIP;
			NotNullPtr<UI::GUIHSplitter> hspClient;
			UI::GUIListView *lvClient;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

			Sync::Mutex cliInfoMut;
			Data::FastMap<UInt32, ClientInfo*> cliInfos;
			Bool cliChg;
			UInt32 currServer;

			Data::ArrayList<Net::DNSClient::RequestAnswer*> v4ansList;
			Data::ArrayList<Net::DNSClient::RequestAnswer*> v6ansList;
			Data::ArrayList<Net::DNSClient::RequestAnswer*> othansList;
			Data::ArrayList<Net::DNSClient::RequestAnswer*> v4sansList;
			Net::DNSProxy::TargetInfo *currTarget;
			Net::DNSProxy *proxy;
			Net::WhoisHandler whois;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnV4ReqSelChg(void *userObj);
			static void __stdcall OnV4AnsSelChg(void *userObj);
			static void __stdcall OnV6ReqSelChg(void *userObj);
			static void __stdcall OnV6AnsSelChg(void *userObj);
			static void __stdcall OnOthReqSelChg(void *userObj);
			static void __stdcall OnOthAnsSelChg(void *userObj);
			static void __stdcall OnTargetSelChg(void *userObj);
			static void __stdcall OnDNSSwitchClicked(void *userObj);
			static void __stdcall OnDNSSetClicked(void *userObj);
			static void __stdcall OnDNSAddClicked(void *userObj);
			static void __stdcall OnSearchClicked(void *userObj);
			static void __stdcall OnSReqSelChg(void *userObj);
			static void __stdcall OnSAnsSelChg(void *userObj);
			static void __stdcall OnClientSelChg(void *userObj);
			static void __stdcall OnDisableV6Chg(void *userObj, Bool isChecked);
			static void __stdcall OnBlackListClicked(void *userObj);
			static void __stdcall OnWPADClicked(void *userObj);
			static void __stdcall OnDNSRequest(void *userObj, Text::CString reqName, Int32 reqType, Int32 reqClass, NotNullPtr<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed);

			void UpdateDNSList();
			void UpdateBlackList();
		public:
			AVIRDNSProxyForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDNSProxyForm();

			virtual void OnMonitorChanged();

			Bool IsError();
			void SetDNSList(Data::ArrayList<UInt32> *dnsList);
			void SetDisableV6(Bool disableV6);
			void AddBlacklist(Text::CStringNN blackList);
		};
	}
}
#endif
