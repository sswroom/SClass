#ifndef _SM_SSWR_AVIREAD_AVIRCERTUTILFORM
#define _SM_SSWR_AVIREAD_AVIRCERTUTILFORM
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
		class AVIRCertUtilForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Data::ArrayList<const UTF8Char*> *sanList;
			Crypto::Cert::X509Key *key;

			UI::GUILabel *lblKey;
			UI::GUILabel *lblKeyDetail;
			UI::GUIButton *btnKeyGenerate;
			UI::GUIButton *btnKeyView;
			UI::GUIButton *btnKeySave;
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
			UI::GUILabel *lblValidDays;
			UI::GUITextBox *txtValidDays;
			UI::GUICheckBox *chkCACert;
			UI::GUILabel *lblSAN;
			UI::GUITextBox *txtSAN;
			UI::GUIButton *btnSANAdd;
			UI::GUIListBox *lbSAN;
			UI::GUIButton *btnSANClear;
			UI::GUIButton *btnCSRGenerate;
			UI::GUIButton *btnSelfSignedCert;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnKeyGenerateClicked(void *userObj);
			static void __stdcall OnKeyViewClicked(void *userObj);
			static void __stdcall OnKeySaveClicked(void *userObj);
			static void __stdcall OnSANAddClicked(void *userObj);
			static void __stdcall OnSANClearClicked(void *userObj);
			static void __stdcall OnCSRGenerateClicked(void *userObj);
			static void __stdcall OnSelfSignedCertClicked(void *userObj);

			Bool GetNames(Crypto::Cert::CertNames *names);
			void UpdateKeyDetail();
			void UpdateNames(Crypto::Cert::CertNames *names);
			void UpdateExtensions(Crypto::Cert::CertExtensions *exts);
			void ClearExtensions();
		public:
			AVIRCertUtilForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCertUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
