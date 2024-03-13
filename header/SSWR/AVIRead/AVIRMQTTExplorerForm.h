#ifndef _SM_SSWR_AVIREAD_AVIRMQTTEXPLORERFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTEXPLORERFORM
#include "Data/StringUTF8Map.h"
#include "Net/MQTTConn.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMQTTExplorerForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NotNullPtr<Text::String> topic;
				UTF8Char *currValue;
				UOSInt currValueLen;
				Bool updated;
				UOSInt recvCnt;
				Data::TimeInstant lastRecvTime;
				Data::Timestamp dateList[256];
				Double valueList[256];
			} TopicStatus;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::MQTTConn *client;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Sync::Mutex topicMut;
			Data::StringMap<TopicStatus*> topicMap;
			Bool topicListChanged;
			TopicStatus *currTopic;
			Media::DrawImage *dispImg;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;

			NotNullPtr<UI::GUIPanel> pnlConnect;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblUsername;
			NotNullPtr<UI::GUITextBox> txtUsername;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUICheckBox> chkSSL;
			NotNullPtr<UI::GUICheckBox> chkWebSocket;
			NotNullPtr<UI::GUIButton> btnCliCert;
			NotNullPtr<UI::GUILabel> lblCliCert;
			NotNullPtr<UI::GUIButton> btnCliKey;
			NotNullPtr<UI::GUILabel> lblCliKey;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUITabControl> tcDetail;

			NotNullPtr<UI::GUITabPage> tpRecv;
			NotNullPtr<UI::GUIListView> lvRecvTopic;
			NotNullPtr<UI::GUIVSplitter> vspRecvTopic;
			NotNullPtr<UI::GUIPictureBoxSimple> pbRecvTopic;

			NotNullPtr<UI::GUITabPage> tpPublish;
			NotNullPtr<UI::GUIPanel> pnlPubTopic;
			NotNullPtr<UI::GUILabel> lblPubTopic;
			NotNullPtr<UI::GUITextBox> txtPubTopic;
			NotNullPtr<UI::GUIPanel> pnlPubCtrl;
			NotNullPtr<UI::GUIButton> btnPublish;
			NotNullPtr<UI::GUITextBox> txtPubContent;

			NotNullPtr<UI::GUITabPage> tpLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnCliCertClicked(void *userObj);
			static void __stdcall OnCliKeyClicked(void *userObj);
			static void __stdcall OnPublishClicked(void *userObj);
			static void __stdcall OnTopicSelChg(void *userObj);
			static void __stdcall OnPingTimerTick(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPublishMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &message);

			void UpdateTopicChart();
			void ServerStop();
			void ClearTopics();
		public:
			AVIRMQTTExplorerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
