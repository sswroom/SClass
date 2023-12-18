#ifndef _SM_SSWR_AVIREAD_AVIRSSLCERTKEYFORM
#define _SM_SSWR_AVIREAD_AVIRSSLCERTKEYFORM
#include "Crypto/Cert/X509File.h"
#include "Data/ArrayListNN.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Crypto::Cert::X509Cert *initCert;
			Crypto::Cert::X509File *initKey;
			Crypto::Cert::X509Cert *cert;
			Crypto::Cert::X509File *key;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NotNullPtr<UI::GUIPanel> pnlCurr;
			NotNullPtr<UI::GUILabel> lblCurrCert;
			UI::GUITextBox *txtCurrCert;
			NotNullPtr<UI::GUILabel> lblCurrKey;
			UI::GUITextBox *txtCurrKey;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpFile;
			NotNullPtr<UI::GUIButton> btnFileCert;
			NotNullPtr<UI::GUILabel> lblFileCert;
			NotNullPtr<UI::GUIButton> btnFileKey;
			NotNullPtr<UI::GUILabel> lblFileKey;
			NotNullPtr<UI::GUIButton> btnFileConfirm;

			NotNullPtr<UI::GUITabPage> tpGenerate;
			NotNullPtr<UI::GUILabel> lblGenCountry;
			UI::GUITextBox *txtGenCountry;
			NotNullPtr<UI::GUILabel> lblGenCompany;
			UI::GUITextBox *txtGenCompany;
			NotNullPtr<UI::GUILabel> lblGenCommonName;
			UI::GUITextBox *txtGenCommonName;
			NotNullPtr<UI::GUIButton> btnGenerate;

			static void __stdcall OnFormClosed(void *userObj, UI::GUIForm *frm);
			static void __stdcall OnFileCertClicked(void *userObj);
			static void __stdcall OnFileKeyClicked(void *userObj);
			static void __stdcall OnFileConfirmClicked(void *userObj);
			static void __stdcall OnGenerateClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

			void LoadFile(Text::CStringNN fileName);
			void ClearCACerts();
		public:
			AVIRSSLCertKeyForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key, NotNullPtr<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts);
			virtual ~AVIRSSLCertKeyForm();

			virtual void OnMonitorChanged();

			Crypto::Cert::X509Cert *GetCert();
			Crypto::Cert::X509File *GetKey();
			UOSInt GetCACerts(NotNullPtr<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts);
		};
	}
}
#endif
