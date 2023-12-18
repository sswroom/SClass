#ifndef _SM_SSWR_AVIREAD_AVIRMQTTBROKERFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTBROKERFORM
#include "Data/StringUTF8Map.h"
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
				NotNullPtr<Text::String> topic;
				UInt8 *message;
				UOSInt msgSize;
				Bool updated;
				Int64 updateTime;
			} TopicStatus;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
			Net::MQTTBroker *broker;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Data::StringMap<TopicStatus*> topicMap;
			Sync::Mutex topicMut;
			Bool topicListUpdated;
			UInt64 totalCount;
			UInt64 dispCount;
			Data::Timestamp dispTime;

			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpStatus;
			UI::GUILabel *lblSSL;
			UI::GUICheckBox *chkSSL;
			NotNullPtr<UI::GUIButton> btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			NotNullPtr<UI::GUIButton> btnStart;
			UI::GUILabel *lblTotalCount;
			UI::GUITextBox *txtTotalCount;
			UI::GUILabel *lblDataRate;
			UI::GUITextBox *txtDataRate;

			NotNullPtr<UI::GUITabPage> tpTopic;
			UI::GUIListView *lvTopic;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnTopicUpdate(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);

			void ServerStop();
			void ClearCACerts();
		public:
			AVIRMQTTBrokerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTBrokerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
