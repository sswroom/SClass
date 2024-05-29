#ifndef _SM_SSWR_AVIREAD_AVIRMQTTBROKERFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTBROKERFORM
#include "Data/StringMapNN.h"
#include "Net/MQTTBroker.h"
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
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMQTTBrokerForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NN<Text::String> topic;
				UInt8 *message;
				UOSInt msgSize;
				Bool updated;
				Int64 updateTime;
			} TopicStatus;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
			Net::MQTTBroker *broker;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Data::StringMapNN<TopicStatus> topicMap;
			Sync::Mutex topicMut;
			Bool topicListUpdated;
			UInt64 totalCount;
			UInt64 dispCount;
			Data::Timestamp dispTime;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblSSL;
			NN<UI::GUICheckBox> chkSSL;
			NN<UI::GUIButton> btnSSLCert;
			NN<UI::GUILabel> lblSSLCert;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblTotalCount;
			NN<UI::GUITextBox> txtTotalCount;
			NN<UI::GUILabel> lblDataRate;
			NN<UI::GUITextBox> txtDataRate;

			NN<UI::GUITabPage> tpTopic;
			NN<UI::GUIListView> lvTopic;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnSSLCertClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnTopicUpdate(AnyType userObj, Text::CStringNN topic, const UInt8 *message, UOSInt msgSize);

			void ServerStop();
			void ClearCACerts();
		public:
			AVIRMQTTBrokerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTBrokerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
