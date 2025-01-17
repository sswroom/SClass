#ifndef _SM_SSWR_AVIREAD_AVIRASN1DATAFORM
#define _SM_SSWR_AVIREAD_AVIRASN1DATAFORM
#include "Net/ASN1Data.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
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
		class AVIRASN1DataForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::ASN1Data> asn1;

			NN<UI::GUIMainMenu> mnuMain;

			NN<UI::GUIPanel> pnlStatus;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpDesc;
			NN<UI::GUITextBox> txtDesc;
			
			NN<UI::GUITabPage> tpASN1;
			NN<UI::GUITextBox> txtASN1;

			NN<UI::GUITabPage> tpVerify;
			NN<UI::GUILabel> lblVerifyHash;
			NN<UI::GUIComboBox> cboVerifyHash;
			NN<UI::GUILabel> lblVerifyPayloadFile;
			NN<UI::GUITextBox> txtVerifyPayloadFile;
			NN<UI::GUILabel> lblVerifySignature;
			NN<UI::GUITextBox> txtVerifySignature;
			NN<UI::GUIButton> btnVerify;
			NN<UI::GUIButton> btnVerifySignInfo;
			NN<UI::GUILabel> lblVerifyStatus;
			NN<UI::GUITextBox> txtVerifyStatus;

//			NN<UI::GUITabPage> tpSignature;
//			NN<UI::GUILabel> lblSignatureHash;
//			NN<UI::GUIComboBox> cboSignatureHash;
//			NN<UI::GUILabel> lblSignaturePayloadFile;
//			NN<UI::GUITextBox> txtSignaturePayloadFile;
//			NN<UI::GUIButton> btnSignature;
//			NN<UI::GUILabel> lblSiguatureValue;
//			NN<UI::GUITextBox> txtSignatureValue;

			NN<UI::GUITabPage> tpEncrypt;
			NN<UI::GUILabel> lblEncryptInputType;
			NN<UI::GUIComboBox> cboEncryptInputType;
			NN<UI::GUILabel> lblEncryptOutputType;
			NN<UI::GUIComboBox> cboEncryptOutputType;
			NN<UI::GUILabel> lblEncryptRSAPadding;
			NN<UI::GUIComboBox> cboEncryptRSAPadding;
			NN<UI::GUILabel> lblEncryptInput;
			NN<UI::GUITextBox> txtEncryptInput;
			NN<UI::GUIButton> btnEncryptEncrypt;
			NN<UI::GUIButton> btnEncryptDecrypt;
			NN<UI::GUILabel> lblEncryptOutput;
			NN<UI::GUITextBox> txtEncryptOutput;

			static UOSInt AddHash(NN<UI::GUIComboBox> cbo, Crypto::Hash::HashType hashType, Crypto::Hash::HashType targetType);
			static void AddHashTypes(NN<UI::GUIComboBox> cbo, Crypto::Hash::HashType hashType);
			static Bool FileIsSign(NN<Text::String> fileName);
			static void __stdcall OnVerifyClicked(AnyType userObj);
			static void __stdcall OnVerifySignInfoClicked(AnyType userObj);
			static void __stdcall OnEncryptEncryptClicked(AnyType userObj);
			static void __stdcall OnEncryptDecryptClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			UOSInt ParseSignature(NN<Text::PString> s, UnsafeArray<UInt8> sign);
			Optional<Crypto::Cert::X509Key> GetNewKey();
		public:
			AVIRASN1DataForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Net::ASN1Data> asn1);
			virtual ~AVIRASN1DataForm();

			virtual void OnMonitorChanged();
			virtual void EventMenuClicked(UInt16 cmdId);
		};
	}
}
#endif
