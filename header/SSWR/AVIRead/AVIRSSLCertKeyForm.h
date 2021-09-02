#ifndef _SM_SSWR_AVIREAD_AVIRSSLCERTKEYFORM
#define _SM_SSWR_AVIREAD_AVIRSSLCERTKEYFORM
#include "Crypto/Cert/X509File.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSSLCertKeyForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Crypto::Cert::X509File *initCert;
			Crypto::Cert::X509File *initKey;
			Crypto::Cert::X509File *cert;
			Crypto::Cert::X509File *key;

			UI::GUIPanel *pnlCurr;
			UI::GUILabel *lblCurrCert;
			UI::GUITextBox *txtCurrCert;
			UI::GUILabel *lblCurrKey;
			UI::GUITextBox *txtCurrKey;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpFile;
			UI::GUIButton *btnFileCert;
			UI::GUILabel *lblFileCert;
			UI::GUIButton *btnFileKey;
			UI::GUILabel *lblFileKey;
			UI::GUIButton *btnFileConfirm;

			UI::GUITabPage *tpGenerate;
			UI::GUILabel *lblGenCountry;
			UI::GUITextBox *txtGenCountry;
			UI::GUILabel *lblGenCompany;
			UI::GUITextBox *txtGenCompany;
			UI::GUILabel *lblGenCommonName;
			UI::GUITextBox *txtGenCommonName;
			UI::GUIButton *btnGenerate;

			static void __stdcall OnFormClosed(void *userObj, UI::GUIForm *frm);
			static void __stdcall OnFileCertClicked(void *userObj);
			static void __stdcall OnFileKeyClicked(void *userObj);
			static void __stdcall OnFileConfirmClicked(void *userObj);
			static void __stdcall OnGenerateClicked(void *userObj);

		public:
			AVIRSSLCertKeyForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl, Crypto::Cert::X509File *cert, Crypto::Cert::X509File *key);
			virtual ~AVIRSSLCertKeyForm();

			virtual void OnMonitorChanged();

			Crypto::Cert::X509File *GetCert();
			Crypto::Cert::X509File *GetKey();
		};
	}
}
#endif
