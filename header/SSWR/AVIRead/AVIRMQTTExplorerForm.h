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
				Text::String *topic;
				UTF8Char *currValue;
				UOSInt currValueLen;
				Bool updated;
				UOSInt recvCnt;
				Int64 lastRecvTime;
				Int64 dateList[256];
				Double valueList[256];
			} TopicStatus;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::MQTTConn *client;
			IO::LogTool log;
			UI::ListBoxLogger *logger;
			Sync::Mutex topicMut;
			Data::StringMap<TopicStatus*> topicMap;
			Bool topicListChanged;
			TopicStatus *currTopic;
			Media::DrawImage *dispImg;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;

			UI::GUIPanel *pnlConnect;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblUsername;
			UI::GUITextBox *txtUsername;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUICheckBox *chkSSL;
			UI::GUIButton *btnCliCert;
			UI::GUILabel *lblCliCert;
			UI::GUIButton *btnCliKey;
			UI::GUILabel *lblCliKey;
			UI::GUILabel *lblStatus;
			UI::GUIButton *btnStart;
			UI::GUITabControl *tcDetail;

			UI::GUITabPage *tpRecv;
			UI::GUIListView *lvRecvTopic;
			UI::GUIVSplitter *vspRecvTopic;
			UI::GUIPictureBoxSimple *pbRecvTopic;

			UI::GUITabPage *tpPublish;
			UI::GUIPanel *pnlPubTopic;
			UI::GUILabel *lblPubTopic;
			UI::GUITextBox *txtPubTopic;
			UI::GUIPanel *pnlPubCtrl;
			UI::GUIButton *btnPublish;
			UI::GUITextBox *txtPubContent;

			UI::GUITabPage *tpLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnCliCertClicked(void *userObj);
			static void __stdcall OnCliKeyClicked(void *userObj);
			static void __stdcall OnPublishClicked(void *userObj);
			static void __stdcall OnTopicSelChg(void *userObj);
			static void __stdcall OnPingTimerTick(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPublishMessage(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);

			void UpdateTopicChart();
			void ServerStop();
			void ClearTopics();
		public:
			AVIRMQTTExplorerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMQTTExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
