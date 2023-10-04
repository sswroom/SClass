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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;
			Data::ArrayListNN<Text::String> *sanList;
			Crypto::Cert::X509Key *key;

			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKeyDetail;
			UI::GUIButton *btnKeyGenerate;
			UI::GUIButton *btnKeyView;
			UI::GUIButton *btnKeySave;
			UI::GUILabel *lblCountryName;
			UI::GUITextBox *txtCountryName;
			UI::GUILabel *lblCountryName2;
			UI::GUILabel *lblStateOrProvinceName;
			UI::GUITextBox *txtStateOrProvinceName;
			UI::GUILabel *lblStateOrProvinceName2;
			UI::GUILabel *lblLocalityName;
			UI::GUITextBox *txtLocalityName;
			UI::GUILabel *lblLocalityName2;
			UI::GUILabel *lblOrganizationName;
			UI::GUITextBox *txtOrganizationName;
			UI::GUILabel *lblOrganizationName2;
			UI::GUILabel *lblOrganizationUnitName;
			UI::GUITextBox *txtOrganizationUnitName;
			UI::GUILabel *lblOrganizationUnitName2;
			UI::GUILabel *lblCommonName;
			UI::GUITextBox *txtCommonName;
			UI::GUILabel *lblCommonName2;
			UI::GUILabel *lblEmailAddress;
			UI::GUITextBox *txtEmailAddress;
			UI::GUILabel *lblValidDays;
			UI::GUITextBox *txtValidDays;
			UI::GUICheckBox *chkCACert;
			UI::GUICheckBox *chkDigitalSign;
			UI::GUILabel *lblSAN;
			UI::GUITextBox *txtSAN;
			UI::GUIButton *btnSANAdd;
			UI::GUIListBox *lbSAN;
			UI::GUIButton *btnSANClear;
			UI::GUIButton *btnCSRGenerate;
			UI::GUIButton *btnSelfSignedCert;

			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnKeyGenerateClicked(void *userObj);
			static void __stdcall OnKeyViewClicked(void *userObj);
			static void __stdcall OnKeySaveClicked(void *userObj);
			static void __stdcall OnSANAddClicked(void *userObj);
			static void __stdcall OnSANClearClicked(void *userObj);
			static void __stdcall OnCSRGenerateClicked(void *userObj);
			static void __stdcall OnSelfSignedCertClicked(void *userObj);

			Bool GetNames(Crypto::Cert::CertNames *names);
			void DisplayKeyDetail();
			void DisplayNames(NotNullPtr<Crypto::Cert::CertNames> names);
			void DisplayExtensions(NotNullPtr<Crypto::Cert::CertExtensions> exts);
			void ClearExtensions();
		public:
			AVIRCertUtilForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCertUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
