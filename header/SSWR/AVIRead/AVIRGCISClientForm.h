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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;
			Data::ArrayListNN<Crypto::Cert::X509Cert> cliCACerts;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;
			Crypto::Cert::X509File *svrCert;

			NotNullPtr<UI::GUILabel> lblURL;
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUILabel> lblClientCert;
			NotNullPtr<UI::GUIButton> btnClientCert;
			NotNullPtr<UI::GUILabel> lblClientCertDisp;
			NotNullPtr<UI::GUILabel> lblTo;
			NotNullPtr<UI::GUITextBox> txtTo;
			NotNullPtr<UI::GUILabel> lblCC;
			NotNullPtr<UI::GUITextBox> txtCC;
			NotNullPtr<UI::GUILabel> lblBCC;
			NotNullPtr<UI::GUITextBox> txtBCC;
			NotNullPtr<UI::GUILabel> lblSubject;
			NotNullPtr<UI::GUITextBox> txtSubject;
			NotNullPtr<UI::GUILabel> lblContent;
			NotNullPtr<UI::GUITextBox> txtContent;
			NotNullPtr<UI::GUILabel> lblContentType;
			NotNullPtr<UI::GUITextBox> txtContentType;
			NotNullPtr<UI::GUIButton> btnSend;
			NotNullPtr<UI::GUILabel> lblResponseStatus;
			NotNullPtr<UI::GUITextBox> txtResponseStatus;
			NotNullPtr<UI::GUILabel> lblServerCert;
			NotNullPtr<UI::GUITextBox> txtServerCert;
			NotNullPtr<UI::GUIButton> btnServerCert;

			static void __stdcall OnClientCertClicked(void *userObj);
			static void __stdcall OnSendClicked(void *userObj);
			static void __stdcall OnServerCertClicked(void *userObj);
			void ClearCliCACerts();

		public:
			AVIRGCISClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGCISClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
