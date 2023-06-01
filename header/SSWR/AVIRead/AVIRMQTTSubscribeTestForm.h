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
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::MQTTConn *client;
			IO::LogTool log;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;
			UInt64 totalCount;
			UInt64 dispCount;
			Data::Timestamp lastDispTime;

			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblUsername;
			UI::GUITextBox *txtUsername;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUICheckBox *chkSSL;
			UI::GUICheckBox *chkWebSocket;
			UI::GUIButton *btnCliCert;
			UI::GUILabel *lblCliCert;
			UI::GUIButton *btnCliKey;
			UI::GUILabel *lblCliKey;
			UI::GUILabel *lblTopic;
			UI::GUITextBox *txtTopic;
			UI::GUIButton *btnStart;
			UI::GUILabel *lblTotalCount;
			UI::GUITextBox *txtTotalCount;
			UI::GUILabel *lblRecvRate;
			UI::GUITextBox *txtRecvRate;
			UI::GUILabel *lblStatus;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnCliCertClicked(void *userObj);
			static void __stdcall OnCliKeyClicked(void *userObj);
			static void __stdcall OnPingTimerTick(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPublishMessage(void *userObj, Text::CString topic, const UInt8 *message, UOSInt msgSize);

			void ServerStop();
		public:
			AVIRMQTTSubscribeTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMQTTSubscribeTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
