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
				Data::ArrayListNN<HourInfo> hourInfos;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpV4Main;
			NN<UI::GUIListBox> lbV4Request;
			NN<UI::GUIHSplitter> hspV4Request;
			NN<UI::GUIPanel> pnlV4Request;
			NN<UI::GUILabel> lblV4RequestTime;
			NN<UI::GUITextBox> txtV4RequestTime;
			NN<UI::GUILabel> lblV4RequestTTL;
			NN<UI::GUITextBox> txtV4RequestTTL;
			NN<UI::GUIListBox> lbV4Answer;
			NN<UI::GUIHSplitter> hspV4Answer;
			NN<UI::GUIPanel> pnlV4Detail;
			NN<UI::GUILabel> lblV4AnsName;
			NN<UI::GUITextBox> txtV4AnsName;
			NN<UI::GUILabel> lblV4AnsType;
			NN<UI::GUITextBox> txtV4AnsType;
			NN<UI::GUILabel> lblV4AnsClass;
			NN<UI::GUITextBox> txtV4AnsClass;
			NN<UI::GUILabel> lblV4AnsTTL;
			NN<UI::GUITextBox> txtV4AnsTTL;
			NN<UI::GUILabel> lblV4AnsRD;
			NN<UI::GUITextBox> txtV4AnsRD;

			NN<UI::GUITabPage> tpV6Main;
			NN<UI::GUIListBox> lbV6Request;
			NN<UI::GUIHSplitter> hspV6Request;
			NN<UI::GUIPanel> pnlV6Request;
			NN<UI::GUILabel> lblV6RequestTime;
			NN<UI::GUITextBox> txtV6RequestTime;
			NN<UI::GUILabel> lblV6RequestTTL;
			NN<UI::GUITextBox> txtV6RequestTTL;
			NN<UI::GUIListBox> lbV6Answer;
			NN<UI::GUIHSplitter> hspV6Answer;
			NN<UI::GUIPanel> pnlV6Detail;
			NN<UI::GUILabel> lblV6AnsName;
			NN<UI::GUITextBox> txtV6AnsName;
			NN<UI::GUILabel> lblV6AnsType;
			NN<UI::GUITextBox> txtV6AnsType;
			NN<UI::GUILabel> lblV6AnsClass;
			NN<UI::GUITextBox> txtV6AnsClass;
			NN<UI::GUILabel> lblV6AnsTTL;
			NN<UI::GUITextBox> txtV6AnsTTL;
			NN<UI::GUILabel> lblV6AnsRD;
			NN<UI::GUITextBox> txtV6AnsRD;

			NN<UI::GUITabPage> tpOthMain;
			NN<UI::GUIListBox> lbOthRequest;
			NN<UI::GUIHSplitter> hspOthRequest;
			NN<UI::GUIPanel> pnlOthRequest;
			NN<UI::GUILabel> lblOthRequestTime;
			NN<UI::GUITextBox> txtOthRequestTime;
			NN<UI::GUILabel> lblOthRequestTTL;
			NN<UI::GUITextBox> txtOthRequestTTL;
			NN<UI::GUIListBox> lbOthAnswer;
			NN<UI::GUIHSplitter> hspOthAnswer;
			NN<UI::GUIPanel> pnlOthDetail;
			NN<UI::GUILabel> lblOthAnsName;
			NN<UI::GUITextBox> txtOthAnsName;
			NN<UI::GUILabel> lblOthAnsType;
			NN<UI::GUITextBox> txtOthAnsType;
			NN<UI::GUILabel> lblOthAnsClass;
			NN<UI::GUITextBox> txtOthAnsClass;
			NN<UI::GUILabel> lblOthAnsTTL;
			NN<UI::GUITextBox> txtOthAnsTTL;
			NN<UI::GUILabel> lblOthAnsRD;
			NN<UI::GUITextBox> txtOthAnsRD;

			NN<UI::GUITabPage> tpTarget;
			NN<UI::GUIListBox> lbTarget;
			NN<UI::GUIHSplitter> hspTarget;
			NN<UI::GUITabControl> tcTarget;
			NN<UI::GUITabPage> tpTargetInfo;
			NN<UI::GUILabel> lblTargetName;
			NN<UI::GUITextBox> txtTargetName;
			NN<UI::GUILabel> lblTargetCountry;
			NN<UI::GUITextBox> txtTargetCountry;
			NN<UI::GUILabel> lblTargetDomains;
			NN<UI::GUIListBox> lbTargetDomains;
			NN<UI::GUITabPage> tpTargetWhois;
			NN<UI::GUITextBox> txtTargetWhois;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblDNSPort;
			NN<UI::GUITextBox> txtDNSPort;
			NN<UI::GUILabel> lblDisableV6;
			NN<UI::GUICheckBox> chkDisableV6;
			NN<UI::GUILabel> lblDNSServer;
			NN<UI::GUITextBox> txtDNSServer;
			NN<UI::GUIButton> btnDNSSwitch;
			NN<UI::GUILabel> lblDNSList;
			NN<UI::GUIListBox> lbDNSList;
			NN<UI::GUITextBox> txtDNSServer2;
			NN<UI::GUIButton> btnDNSAdd;
			NN<UI::GUIButton> btnDNSSet;
			NN<UI::GUILabel> lblWPAD;
			NN<UI::GUITextBox> txtWPAD;
			NN<UI::GUIButton> btnWPAD;

			NN<UI::GUITabPage> tpSearch;
			NN<UI::GUIPanel> pnlSearch;
			NN<UI::GUILabel> lblSearchIPRange;
			NN<UI::GUITextBox> txtSearchIPRange;
			NN<UI::GUILabel> lblSearchMask;
			NN<UI::GUITextBox> txtSearchMask;
			NN<UI::GUIButton> btnSearch;
			NN<UI::GUIListBox> lbSearch;
			NN<UI::GUIHSplitter> hspSearch;
			NN<UI::GUIPanel> pnlSRequest;
			NN<UI::GUILabel> lblSRequestTime;
			NN<UI::GUITextBox> txtSRequestTime;
			NN<UI::GUILabel> lblSRequestTTL;
			NN<UI::GUITextBox> txtSRequestTTL;
			NN<UI::GUIListBox> lbSAnswer;
			NN<UI::GUIHSplitter> hspSAnswer;
			NN<UI::GUIPanel> pnlSDetail;
			NN<UI::GUILabel> lblSAnsName;
			NN<UI::GUITextBox> txtSAnsName;
			NN<UI::GUILabel> lblSAnsType;
			NN<UI::GUITextBox> txtSAnsType;
			NN<UI::GUILabel> lblSAnsClass;
			NN<UI::GUITextBox> txtSAnsClass;
			NN<UI::GUILabel> lblSAnsTTL;
			NN<UI::GUITextBox> txtSAnsTTL;
			NN<UI::GUILabel> lblSAnsRD;
			NN<UI::GUITextBox> txtSAnsRD;

			NN<UI::GUITabPage> tpBlackList;
			NN<UI::GUIPanel> pnlBlackList;
			NN<UI::GUIListBox> lbBlackList;
			NN<UI::GUITextBox> txtBlackList;
			NN<UI::GUIButton> btnBlackList;

			NN<UI::GUITabPage> tpClient;
			NN<UI::GUIListBox> lbClientIP;
			NN<UI::GUIHSplitter> hspClient;
			NN<UI::GUIListView> lvClient;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			Sync::Mutex cliInfoMut;
			Data::FastMapNN<UInt32, ClientInfo> cliInfos;
			Bool cliChg;
			UInt32 currServer;

			Data::ArrayListNN<Net::DNSClient::RequestAnswer> v4ansList;
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> v6ansList;
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> othansList;
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> v4sansList;
			Optional<Net::DNSProxy::TargetInfo> currTarget;
			NN<Net::DNSProxy> proxy;
			Net::WhoisHandler whois;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnV4ReqSelChg(AnyType userObj);
			static void __stdcall OnV4AnsSelChg(AnyType userObj);
			static void __stdcall OnV6ReqSelChg(AnyType userObj);
			static void __stdcall OnV6AnsSelChg(AnyType userObj);
			static void __stdcall OnOthReqSelChg(AnyType userObj);
			static void __stdcall OnOthAnsSelChg(AnyType userObj);
			static void __stdcall OnTargetSelChg(AnyType userObj);
			static void __stdcall OnDNSSwitchClicked(AnyType userObj);
			static void __stdcall OnDNSSetClicked(AnyType userObj);
			static void __stdcall OnDNSAddClicked(AnyType userObj);
			static void __stdcall OnSearchClicked(AnyType userObj);
			static void __stdcall OnSReqSelChg(AnyType userObj);
			static void __stdcall OnSAnsSelChg(AnyType userObj);
			static void __stdcall OnClientSelChg(AnyType userObj);
			static void __stdcall OnDisableV6Chg(AnyType userObj, Bool isChecked);
			static void __stdcall OnBlackListClicked(AnyType userObj);
			static void __stdcall OnWPADClicked(AnyType userObj);
			static void __stdcall OnDNSRequest(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed);

			void UpdateDNSList();
			void UpdateBlackList();
		public:
			AVIRDNSProxyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDNSProxyForm();

			virtual void OnMonitorChanged();

			Bool IsError();
			void SetDNSList(NN<Data::ArrayListNative<UInt32>> dnsList);
			void SetDisableV6(Bool disableV6);
			void AddBlacklist(Text::CStringNN blackList);
		};
	}
}
#endif
