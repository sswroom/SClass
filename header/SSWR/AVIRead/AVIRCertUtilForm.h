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
			Optional<Net::SSLEngine> ssl;
			Data::ArrayListStringNN *sanList;
			Crypto::Cert::X509Key *key;

			NotNullPtr<UI::GUILabel> lblKey;
			NotNullPtr<UI::GUITextBox> txtKeyDetail;
			NotNullPtr<UI::GUIButton> btnKeyGenerate;
			NotNullPtr<UI::GUIButton> btnKeyView;
			NotNullPtr<UI::GUIButton> btnKeySave;
			NotNullPtr<UI::GUILabel> lblCountryName;
			NotNullPtr<UI::GUITextBox> txtCountryName;
			NotNullPtr<UI::GUILabel> lblCountryName2;
			NotNullPtr<UI::GUILabel> lblStateOrProvinceName;
			NotNullPtr<UI::GUITextBox> txtStateOrProvinceName;
			NotNullPtr<UI::GUILabel> lblStateOrProvinceName2;
			NotNullPtr<UI::GUILabel> lblLocalityName;
			NotNullPtr<UI::GUITextBox> txtLocalityName;
			NotNullPtr<UI::GUILabel> lblLocalityName2;
			NotNullPtr<UI::GUILabel> lblOrganizationName;
			NotNullPtr<UI::GUITextBox> txtOrganizationName;
			NotNullPtr<UI::GUILabel> lblOrganizationName2;
			NotNullPtr<UI::GUILabel> lblOrganizationUnitName;
			NotNullPtr<UI::GUITextBox> txtOrganizationUnitName;
			NotNullPtr<UI::GUILabel> lblOrganizationUnitName2;
			NotNullPtr<UI::GUILabel> lblCommonName;
			NotNullPtr<UI::GUITextBox> txtCommonName;
			NotNullPtr<UI::GUILabel> lblCommonName2;
			NotNullPtr<UI::GUILabel> lblEmailAddress;
			NotNullPtr<UI::GUITextBox> txtEmailAddress;
			NotNullPtr<UI::GUILabel> lblValidDays;
			NotNullPtr<UI::GUITextBox> txtValidDays;
			NotNullPtr<UI::GUICheckBox> chkCACert;
			NotNullPtr<UI::GUICheckBox> chkDigitalSign;
			NotNullPtr<UI::GUILabel> lblSAN;
			NotNullPtr<UI::GUITextBox> txtSAN;
			NotNullPtr<UI::GUIButton> btnSANAdd;
			NotNullPtr<UI::GUIListBox> lbSAN;
			NotNullPtr<UI::GUIButton> btnSANClear;
			NotNullPtr<UI::GUIButton> btnCSRGenerate;
			NotNullPtr<UI::GUIButton> btnSelfSignedCert;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnKeyGenerateClicked(AnyType userObj);
			static void __stdcall OnKeyViewClicked(AnyType userObj);
			static void __stdcall OnKeySaveClicked(AnyType userObj);
			static void __stdcall OnSANAddClicked(AnyType userObj);
			static void __stdcall OnSANClearClicked(AnyType userObj);
			static void __stdcall OnCSRGenerateClicked(AnyType userObj);
			static void __stdcall OnSelfSignedCertClicked(AnyType userObj);

			Bool GetNames(Crypto::Cert::CertNames *names);
			void DisplayKeyDetail();
			void DisplayNames(NotNullPtr<Crypto::Cert::CertNames> names);
			void DisplayExtensions(NotNullPtr<Crypto::Cert::CertExtensions> exts);
			void ClearExtensions();
		public:
			AVIRCertUtilForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCertUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
