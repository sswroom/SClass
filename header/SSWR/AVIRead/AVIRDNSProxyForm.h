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
			SSWR::AVIRead::AVIRCore *core;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpV4Main;
			UI::GUIListBox *lbV4Request;
			UI::GUIHSplitter *hspV4Request;
			UI::GUIPanel *pnlV4Request;
			UI::GUILabel *lblV4RequestTime;
			UI::GUITextBox *txtV4RequestTime;
			UI::GUILabel *lblV4RequestTTL;
			UI::GUITextBox *txtV4RequestTTL;
			UI::GUIListBox *lbV4Answer;
			UI::GUIHSplitter *hspV4Answer;
			UI::GUIPanel *pnlV4Detail;
			UI::GUILabel *lblV4AnsName;
			UI::GUITextBox *txtV4AnsName;
			UI::GUILabel *lblV4AnsType;
			UI::GUITextBox *txtV4AnsType;
			UI::GUILabel *lblV4AnsClass;
			UI::GUITextBox *txtV4AnsClass;
			UI::GUILabel *lblV4AnsTTL;
			UI::GUITextBox *txtV4AnsTTL;
			UI::GUILabel *lblV4AnsRD;
			UI::GUITextBox *txtV4AnsRD;

			UI::GUITabPage *tpV6Main;
			UI::GUIListBox *lbV6Request;
			UI::GUIHSplitter *hspV6Request;
			UI::GUIPanel *pnlV6Request;
			UI::GUILabel *lblV6RequestTime;
			UI::GUITextBox *txtV6RequestTime;
			UI::GUILabel *lblV6RequestTTL;
			UI::GUITextBox *txtV6RequestTTL;
			UI::GUIListBox *lbV6Answer;
			UI::GUIHSplitter *hspV6Answer;
			UI::GUIPanel *pnlV6Detail;
			UI::GUILabel *lblV6AnsName;
			UI::GUITextBox *txtV6AnsName;
			UI::GUILabel *lblV6AnsType;
			UI::GUITextBox *txtV6AnsType;
			UI::GUILabel *lblV6AnsClass;
			UI::GUITextBox *txtV6AnsClass;
			UI::GUILabel *lblV6AnsTTL;
			UI::GUITextBox *txtV6AnsTTL;
			UI::GUILabel *lblV6AnsRD;
			UI::GUITextBox *txtV6AnsRD;

			UI::GUITabPage *tpOthMain;
			UI::GUIListBox *lbOthRequest;
			UI::GUIHSplitter *hspOthRequest;
			UI::GUIPanel *pnlOthRequest;
			UI::GUILabel *lblOthRequestTime;
			UI::GUITextBox *txtOthRequestTime;
			UI::GUILabel *lblOthRequestTTL;
			UI::GUITextBox *txtOthRequestTTL;
			UI::GUIListBox *lbOthAnswer;
			UI::GUIHSplitter *hspOthAnswer;
			UI::GUIPanel *pnlOthDetail;
			UI::GUILabel *lblOthAnsName;
			UI::GUITextBox *txtOthAnsName;
			UI::GUILabel *lblOthAnsType;
			UI::GUITextBox *txtOthAnsType;
			UI::GUILabel *lblOthAnsClass;
			UI::GUITextBox *txtOthAnsClass;
			UI::GUILabel *lblOthAnsTTL;
			UI::GUITextBox *txtOthAnsTTL;
			UI::GUILabel *lblOthAnsRD;
			UI::GUITextBox *txtOthAnsRD;

			UI::GUITabPage *tpTarget;
			UI::GUIListBox *lbTarget;
			UI::GUIHSplitter *hspTarget;
			UI::GUITabControl *tcTarget;
			UI::GUITabPage *tpTargetInfo;
			UI::GUILabel *lblTargetName;
			UI::GUITextBox *txtTargetName;
			UI::GUILabel *lblTargetCountry;
			UI::GUITextBox *txtTargetCountry;
			UI::GUILabel *lblTargetDomains;
			UI::GUIListBox *lbTargetDomains;
			UI::GUITabPage *tpTargetWhois;
			UI::GUITextBox *txtTargetWhois;

			UI::GUITabPage *tpStatus;
			UI::GUILabel *lblDNSPort;
			UI::GUITextBox *txtDNSPort;
			UI::GUILabel *lblDisableV6;
			UI::GUICheckBox *chkDisableV6;
			UI::GUILabel *lblDNSServer;
			UI::GUITextBox *txtDNSServer;
			UI::GUIButton *btnDNSSwitch;
			UI::GUILabel *lblDNSList;
			UI::GUIListBox *lbDNSList;
			UI::GUITextBox *txtDNSServer2;
			UI::GUIButton *btnDNSAdd;
			UI::GUIButton *btnDNSSet;
			UI::GUILabel *lblWPAD;
			UI::GUITextBox *txtWPAD;
			UI::GUIButton *btnWPAD;

			UI::GUITabPage *tpSearch;
			UI::GUIPanel *pnlSearch;
			UI::GUILabel *lblSearchIPRange;
			UI::GUITextBox *txtSearchIPRange;
			UI::GUILabel *lblSearchMask;
			UI::GUITextBox *txtSearchMask;
			UI::GUIButton *btnSearch;
			UI::GUIListBox *lbSearch;
			UI::GUIHSplitter *hspSearch;
			UI::GUIPanel *pnlSRequest;
			UI::GUILabel *lblSRequestTime;
			UI::GUITextBox *txtSRequestTime;
			UI::GUILabel *lblSRequestTTL;
			UI::GUITextBox *txtSRequestTTL;
			UI::GUIListBox *lbSAnswer;
			UI::GUIHSplitter *hspSAnswer;
			UI::GUIPanel *pnlSDetail;
			UI::GUILabel *lblSAnsName;
			UI::GUITextBox *txtSAnsName;
			UI::GUILabel *lblSAnsType;
			UI::GUITextBox *txtSAnsType;
			UI::GUILabel *lblSAnsClass;
			UI::GUITextBox *txtSAnsClass;
			UI::GUILabel *lblSAnsTTL;
			UI::GUITextBox *txtSAnsTTL;
			UI::GUILabel *lblSAnsRD;
			UI::GUITextBox *txtSAnsRD;

			UI::GUITabPage *tpBlackList;
			UI::GUIPanel *pnlBlackList;
			UI::GUIListBox *lbBlackList;
			UI::GUITextBox *txtBlackList;
			UI::GUIButton *btnBlackList;

			UI::GUITabPage *tpClient;
			UI::GUIListBox *lbClientIP;
			UI::GUIHSplitter *hspClient;
			UI::GUIListView *lvClient;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
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
			UI::ListBoxLogger *logger;

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
			static void __stdcall OnDNSRequest(void *userObj, Text::CString reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed);

			void UpdateDNSList();
			void UpdateBlackList();
		public:
			AVIRDNSProxyForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRDNSProxyForm();

			virtual void OnMonitorChanged();

			Bool IsError();
			void SetDNSList(Data::ArrayList<UInt32> *dnsList);
			void SetDisableV6(Bool disableV6);
			void AddBlacklist(Text::CString blackList);
		};
	}
}
#endif
