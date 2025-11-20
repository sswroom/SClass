#ifndef _SM_SSWR_AVIREAD_AVIRMQTTEXPLORERFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTEXPLORERFORM
#include "Data/StringMapNN.hpp"
#include "Net/MQTTConn.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
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
				NN<Text::String> topic;
				UnsafeArray<UTF8Char> currValue;
				UOSInt currValueLen;
				Bool updated;
				UOSInt recvCnt;
				Data::TimeInstant lastRecvTime;
				Data::Timestamp dateList[256];
				Double valueList[256];
			} TopicStatus;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::MQTTConn> client;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Sync::Mutex topicMut;
			Data::StringMapNN<TopicStatus> topicMap;
			Bool topicListChanged;
			Optional<TopicStatus> currTopic;
			Optional<Media::DrawImage> dispImg;
			Optional<Crypto::Cert::X509Cert> cliCert;
			Optional<Crypto::Cert::X509File> cliKey;

			NN<UI::GUIPanel> pnlConnect;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblUsername;
			NN<UI::GUITextBox> txtUsername;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUICheckBox> chkSSL;
			NN<UI::GUICheckBox> chkWebSocket;
			NN<UI::GUIButton> btnCliCert;
			NN<UI::GUILabel> lblCliCert;
			NN<UI::GUIButton> btnCliKey;
			NN<UI::GUILabel> lblCliKey;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcDetail;

			NN<UI::GUITabPage> tpRecv;
			NN<UI::GUIListView> lvRecvTopic;
			NN<UI::GUIVSplitter> vspRecvTopic;
			NN<UI::GUIPictureBoxSimple> pbRecvTopic;

			NN<UI::GUITabPage> tpPublish;
			NN<UI::GUIPanel> pnlPubTopic;
			NN<UI::GUILabel> lblPubTopic;
			NN<UI::GUITextBox> txtPubTopic;
			NN<UI::GUICheckBox> chkPubDUP;
			NN<UI::GUICheckBox> chkPubRetain;
			NN<UI::GUILabel> lblPubQoS;
			NN<UI::GUIComboBox> cboPubQoS;
			NN<UI::GUIPanel> pnlPubCtrl;
			NN<UI::GUIButton> btnPublish;
			NN<UI::GUITextBox> txtPubContent;

			NN<UI::GUITabPage> tpLog;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnCliCertClicked(AnyType userObj);
			static void __stdcall OnCliKeyClicked(AnyType userObj);
			static void __stdcall OnPublishClicked(AnyType userObj);
			static void __stdcall OnTopicSelChg(AnyType userObj);
			static void __stdcall OnPingTimerTick(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPublishMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &message);

			void UpdateTopicChart();
			void ServerStop();
			void ClearTopics();
		public:
			AVIRMQTTExplorerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
