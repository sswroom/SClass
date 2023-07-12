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
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Data::ArrayList<const UTF8Char*> *sanList;
			Crypto::Cert::X509Cert *caCert;
			Crypto::Cert::X509Key *key;
			Crypto::Cert::X509CertReq *csr;

			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKey;
			UI::GUIButton *btnKeyView;
			UI::GUILabel *lblCACert;
			UI::GUITextBox *txtCACert;
			UI::GUIButton *btnCACertView;
			UI::GUILabel *lblCSR;
			UI::GUITextBox *txtCSR;
			UI::GUIButton *btnCSRView;
			UI::GUILabel *lblCountryName;
			UI::GUITextBox *txtCountryName;
			UI::GUILabel *lblStateOrProvinceName;
			UI::GUITextBox *txtStateOrProvinceName;
			UI::GUILabel *lblLocalityName;
			UI::GUITextBox *txtLocalityName;
			UI::GUILabel *lblOrganizationName;
			UI::GUITextBox *txtOrganizationName;
			UI::GUILabel *lblOrganizationUnitName;
			UI::GUITextBox *txtOrganizationUnitName;
			UI::GUILabel *lblCommonName;
			UI::GUITextBox *txtCommonName;
			UI::GUILabel *lblEmailAddress;
			UI::GUITextBox *txtEmailAddress;
			UI::GUILabel *lblSAN;
			UI::GUIListBox *lbSAN;
			UI::GUILabel *lblValidDays;
			UI::GUITextBox *txtValidDays;
			UI::GUIButton *btnIssue;

			static void __stdcall OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnKeyViewClicked(void *userObj);
			static void __stdcall OnCACertViewClicked(void *userObj);
			static void __stdcall OnCSRViewClicked(void *userObj);
			static void __stdcall OnIssueClicked(void *userObj);

			void UpdateKeyDetail();
			void UpdateNames(Crypto::Cert::CertNames *names);
		public:
			AVIRCAUtilForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCAUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
