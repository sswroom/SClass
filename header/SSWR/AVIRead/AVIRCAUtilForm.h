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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Data::ArrayList<const UTF8Char*> *sanList;
			Crypto::Cert::X509Cert *caCert;
			Crypto::Cert::X509Key *key;
			Crypto::Cert::X509CertReq *csr;

			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUIButton> btnKeyView;
			NN<UI::GUILabel> lblCACert;
			NN<UI::GUITextBox> txtCACert;
			NN<UI::GUIButton> btnCACertView;
			NN<UI::GUILabel> lblCSR;
			NN<UI::GUITextBox> txtCSR;
			NN<UI::GUIButton> btnCSRView;
			NN<UI::GUILabel> lblCountryName;
			NN<UI::GUITextBox> txtCountryName;
			NN<UI::GUILabel> lblStateOrProvinceName;
			NN<UI::GUITextBox> txtStateOrProvinceName;
			NN<UI::GUILabel> lblLocalityName;
			NN<UI::GUITextBox> txtLocalityName;
			NN<UI::GUILabel> lblOrganizationName;
			NN<UI::GUITextBox> txtOrganizationName;
			NN<UI::GUILabel> lblOrganizationUnitName;
			NN<UI::GUITextBox> txtOrganizationUnitName;
			NN<UI::GUILabel> lblCommonName;
			NN<UI::GUITextBox> txtCommonName;
			NN<UI::GUILabel> lblEmailAddress;
			NN<UI::GUITextBox> txtEmailAddress;
			NN<UI::GUILabel> lblSAN;
			NN<UI::GUIListBox> lbSAN;
			NN<UI::GUILabel> lblValidDays;
			NN<UI::GUITextBox> txtValidDays;
			NN<UI::GUIButton> btnIssue;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnKeyViewClicked(AnyType userObj);
			static void __stdcall OnCACertViewClicked(AnyType userObj);
			static void __stdcall OnCSRViewClicked(AnyType userObj);
			static void __stdcall OnIssueClicked(AnyType userObj);

			void DisplayKeyDetail();
			void DisplayNames(NN<Crypto::Cert::CertNames> names);
		public:
			AVIRCAUtilForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCAUtilForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
