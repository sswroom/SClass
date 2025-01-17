#ifndef _SM_SSWR_AVIREAD_AVIRCERTUTILFORM
#define _SM_SSWR_AVIREAD_AVIRCERTUTILFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Data::ArrayListStringNN sanList;
			Optional<Crypto::Cert::X509Key> key;

			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKeyDetail;
			NN<UI::GUIComboBox> cboKeyType;
			NN<UI::GUIButton> btnKeyGenerate;
			NN<UI::GUIButton> btnKeyView;
			NN<UI::GUIButton> btnKeySave;
			NN<UI::GUILabel> lblCountryName;
			NN<UI::GUITextBox> txtCountryName;
			NN<UI::GUILabel> lblCountryName2;
			NN<UI::GUILabel> lblStateOrProvinceName;
			NN<UI::GUITextBox> txtStateOrProvinceName;
			NN<UI::GUILabel> lblStateOrProvinceName2;
			NN<UI::GUILabel> lblLocalityName;
			NN<UI::GUITextBox> txtLocalityName;
			NN<UI::GUILabel> lblLocalityName2;
			NN<UI::GUILabel> lblOrganizationName;
			NN<UI::GUITextBox> txtOrganizationName;
			NN<UI::GUILabel> lblOrganizationName2;
			NN<UI::GUILabel> lblOrganizationUnitName;
			NN<UI::GUITextBox> txtOrganizationUnitName;
			NN<UI::GUILabel> lblOrganizationUnitName2;
			NN<UI::GUILabel> lblCommonName;
			NN<UI::GUITextBox> txtCommonName;
			NN<UI::GUILabel> lblCommonName2;
			NN<UI::GUILabel> lblEmailAddress;
			NN<UI::GUITextBox> txtEmailAddress;
			NN<UI::GUILabel> lblValidDays;
			NN<UI::GUITextBox> txtValidDays;
			NN<UI::GUICheckBox> chkCACert;
			NN<UI::GUICheckBox> chkDigitalSign;
			NN<UI::GUILabel> lblSAN;
			NN<UI::GUITextBox> txtSAN;
			NN<UI::GUIButton> btnSANAdd;
			NN<UI::GUIListBox> lbSAN;
			NN<UI::GUIButton> btnSANClear;
			NN<UI::GUIButton> btnCSRGenerate;
			NN<UI::GUIButton> btnSelfSignedCert;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnKeyGenerateClicked(AnyType userObj);
			static void __stdcall OnKeyViewClicked(AnyType userObj);
			static void __stdcall OnKeySaveClicked(AnyType userObj);
			static void __stdcall OnSANAddClicked(AnyType userObj);
			static void __stdcall OnSANClearClicked(AnyType userObj);
			static void __stdcall OnCSRGenerateClicked(AnyType userObj);
			static void __stdcall OnSelfSignedCertClicked(AnyType userObj);

			Bool GetNames(NN<Crypto::Cert::CertNames> names);
			void DisplayKeyDetail();
			void DisplayNames(NN<Crypto::Cert::CertNames> names);
			void DisplayExtensions(NN<Crypto::Cert::CertExtensions> exts);
			void ClearExtensions();
		public:
			AVIRCertUtilForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCertUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
