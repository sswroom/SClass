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
				Text::String *topic;
				UInt8 *message;
				UOSInt msgSize;
				Bool updated;
				Int64 updateTime;
			} TopicStatus;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;
			Net::MQTTBroker *broker;
			IO::LogTool *log;
			UI::ListBoxLogger *logger;
			Data::StringMap<TopicStatus*> topicMap;
			Sync::Mutex topicMut;
			Bool topicListUpdated;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpStatus;
			UI::GUILabel *lblSSL;
			UI::GUICheckBox *chkSSL;
			UI::GUIButton *btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStart;

			UI::GUITabPage *tpTopic;
			UI::GUIListView *lvTopic;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnTopicUpdate(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);

			void ServerStop();
		public:
			AVIRMQTTBrokerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMQTTBrokerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
