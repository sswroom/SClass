#ifndef _SM_SSWR_AVIREAD_AVIRGCISCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRGCISCLIENTFORM
#include "Crypto/Cert/X509Cert.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGCISClientForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Data::ArrayListNN<Crypto::Cert::X509Cert> cliCACerts;
			Optional<Crypto::Cert::X509Cert> cliCert;
			Optional<Crypto::Cert::X509File> cliKey;
			Optional<Crypto::Cert::X509File> svrCert;

			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblClientCert;
			NN<UI::GUIButton> btnClientCert;
			NN<UI::GUILabel> lblClientCertDisp;
			NN<UI::GUILabel> lblTo;
			NN<UI::GUITextBox> txtTo;
			NN<UI::GUILabel> lblCC;
			NN<UI::GUITextBox> txtCC;
			NN<UI::GUILabel> lblBCC;
			NN<UI::GUITextBox> txtBCC;
			NN<UI::GUILabel> lblSubject;
			NN<UI::GUITextBox> txtSubject;
			NN<UI::GUILabel> lblContent;
			NN<UI::GUITextBox> txtContent;
			NN<UI::GUILabel> lblContentType;
			NN<UI::GUITextBox> txtContentType;
			NN<UI::GUIButton> btnSend;
			NN<UI::GUILabel> lblResponseStatus;
			NN<UI::GUITextBox> txtResponseStatus;
			NN<UI::GUILabel> lblServerCert;
			NN<UI::GUITextBox> txtServerCert;
			NN<UI::GUIButton> btnServerCert;

			static void __stdcall OnClientCertClicked(AnyType userObj);
			static void __stdcall OnSendClicked(AnyType userObj);
			static void __stdcall OnServerCertClicked(AnyType userObj);
			void ClearCliCACerts();

		public:
			AVIRGCISClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGCISClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
