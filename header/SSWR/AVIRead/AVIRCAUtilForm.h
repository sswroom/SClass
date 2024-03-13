#ifndef _SM_SSWR_AVIREAD_AVIRCAUTILFORM
#define _SM_SSWR_AVIREAD_AVIRCAUTILFORM
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509CertReq.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCAUtilForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Data::ArrayList<const UTF8Char*> *sanList;
			Crypto::Cert::X509Cert *caCert;
			Crypto::Cert::X509Key *key;
			Crypto::Cert::X509CertReq *csr;

			NotNullPtr<UI::GUILabel> lblKey;
			NotNullPtr<UI::GUITextBox> txtKey;
			NotNullPtr<UI::GUIButton> btnKeyView;
			NotNullPtr<UI::GUILabel> lblCACert;
			NotNullPtr<UI::GUITextBox> txtCACert;
			NotNullPtr<UI::GUIButton> btnCACertView;
			NotNullPtr<UI::GUILabel> lblCSR;
			NotNullPtr<UI::GUITextBox> txtCSR;
			NotNullPtr<UI::GUIButton> btnCSRView;
			NotNullPtr<UI::GUILabel> lblCountryName;
			NotNullPtr<UI::GUITextBox> txtCountryName;
			NotNullPtr<UI::GUILabel> lblStateOrProvinceName;
			NotNullPtr<UI::GUITextBox> txtStateOrProvinceName;
			NotNullPtr<UI::GUILabel> lblLocalityName;
			NotNullPtr<UI::GUITextBox> txtLocalityName;
			NotNullPtr<UI::GUILabel> lblOrganizationName;
			NotNullPtr<UI::GUITextBox> txtOrganizationName;
			NotNullPtr<UI::GUILabel> lblOrganizationUnitName;
			NotNullPtr<UI::GUITextBox> txtOrganizationUnitName;
			NotNullPtr<UI::GUILabel> lblCommonName;
			NotNullPtr<UI::GUITextBox> txtCommonName;
			NotNullPtr<UI::GUILabel> lblEmailAddress;
			NotNullPtr<UI::GUITextBox> txtEmailAddress;
			NotNullPtr<UI::GUILabel> lblSAN;
			NotNullPtr<UI::GUIListBox> lbSAN;
			NotNullPtr<UI::GUILabel> lblValidDays;
			NotNullPtr<UI::GUITextBox> txtValidDays;
			NotNullPtr<UI::GUIButton> btnIssue;

			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnKeyViewClicked(void *userObj);
			static void __stdcall OnCACertViewClicked(void *userObj);
			static void __stdcall OnCSRViewClicked(void *userObj);
			static void __stdcall OnIssueClicked(void *userObj);

			void DisplayKeyDetail();
			void DisplayNames(NotNullPtr<Crypto::Cert::CertNames> names);
		public:
			AVIRCAUtilForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCAUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
