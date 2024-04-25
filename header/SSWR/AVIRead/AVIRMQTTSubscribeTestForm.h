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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::MQTTConn *client;
			IO::LogTool log;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;
			UInt64 totalCount;
			UInt64 dispCount;
			Data::Timestamp lastDispTime;

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
			NN<UI::GUILabel> lblTopic;
			NN<UI::GUITextBox> txtTopic;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblTotalCount;
			NN<UI::GUITextBox> txtTotalCount;
			NN<UI::GUILabel> lblRecvRate;
			NN<UI::GUITextBox> txtRecvRate;
			NN<UI::GUILabel> lblStatus;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnCliCertClicked(AnyType userObj);
			static void __stdcall OnCliKeyClicked(AnyType userObj);
			static void __stdcall OnPingTimerTick(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPublishMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &message);

			void ServerStop();
		public:
			AVIRMQTTSubscribeTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTSubscribeTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
