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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Crypto::Cert::X509Cert *initCert;
			Crypto::Cert::X509File *initKey;
			Crypto::Cert::X509Cert *cert;
			Crypto::Cert::X509File *key;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NN<UI::GUIPanel> pnlCurr;
			NN<UI::GUILabel> lblCurrCert;
			NN<UI::GUITextBox> txtCurrCert;
			NN<UI::GUILabel> lblCurrKey;
			NN<UI::GUITextBox> txtCurrKey;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpFile;
			NN<UI::GUIButton> btnFileCert;
			NN<UI::GUILabel> lblFileCert;
			NN<UI::GUIButton> btnFileKey;
			NN<UI::GUILabel> lblFileKey;
			NN<UI::GUIButton> btnFileConfirm;

			NN<UI::GUITabPage> tpGenerate;
			NN<UI::GUILabel> lblGenCountry;
			NN<UI::GUITextBox> txtGenCountry;
			NN<UI::GUILabel> lblGenCompany;
			NN<UI::GUITextBox> txtGenCompany;
			NN<UI::GUILabel> lblGenCommonName;
			NN<UI::GUITextBox> txtGenCommonName;
			NN<UI::GUIButton> btnGenerate;

			static void __stdcall OnFormClosed(AnyType userObj, NN<UI::GUIForm> frm);
			static void __stdcall OnFileCertClicked(AnyType userObj);
			static void __stdcall OnFileKeyClicked(AnyType userObj);
			static void __stdcall OnFileConfirmClicked(AnyType userObj);
			static void __stdcall OnGenerateClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);

			void LoadFile(Text::CStringNN fileName);
			void ClearCACerts();
		public:
			AVIRSSLCertKeyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts);
			virtual ~AVIRSSLCertKeyForm();

			virtual void OnMonitorChanged();

			Crypto::Cert::X509Cert *GetCert();
			Crypto::Cert::X509File *GetKey();
			UOSInt GetCACerts(NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts);
		};
	}
}
#endif
