#ifndef _SM_SSWR_AVIREAD_AVIRMQTTSUBSCRIBETESTFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTSUBSCRIBETESTFORM
#include "Net/MQTTConn.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMQTTSubscribeTestForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::MQTTConn *client;
			IO::LogTool log;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;
			UInt64 totalCount;
			UInt64 dispCount;
			Data::Timestamp lastDispTime;

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
			NotNullPtr<UI::GUILabel> lblTopic;
			NotNullPtr<UI::GUITextBox> txtTopic;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblTotalCount;
			NotNullPtr<UI::GUITextBox> txtTotalCount;
			NotNullPtr<UI::GUILabel> lblRecvRate;
			NotNullPtr<UI::GUITextBox> txtRecvRate;
			NotNullPtr<UI::GUILabel> lblStatus;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnCliCertClicked(void *userObj);
			static void __stdcall OnCliKeyClicked(void *userObj);
			static void __stdcall OnPingTimerTick(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPublishMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &message);

			void ServerStop();
		public:
			AVIRMQTTSubscribeTestForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTSubscribeTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
