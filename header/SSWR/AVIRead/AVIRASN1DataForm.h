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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::ASN1Data> asn1;

			UI::GUIMainMenu *mnuMain;

			NotNullPtr<UI::GUIPanel> pnlStatus;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpDesc;
			UI::GUITextBox *txtDesc;
			
			NotNullPtr<UI::GUITabPage> tpASN1;
			UI::GUITextBox *txtASN1;

			NotNullPtr<UI::GUITabPage> tpVerify;
			UI::GUILabel *lblVerifyHash;
			NotNullPtr<UI::GUIComboBox> cboVerifyHash;
			UI::GUILabel *lblVerifyPayloadFile;
			UI::GUITextBox *txtVerifyPayloadFile;
			UI::GUILabel *lblVerifySignature;
			UI::GUITextBox *txtVerifySignature;
			NotNullPtr<UI::GUIButton> btnVerify;
			NotNullPtr<UI::GUIButton> btnVerifySignInfo;
			UI::GUILabel *lblVerifyStatus;
			UI::GUITextBox *txtVerifyStatus;

//			NotNullPtr<UI::GUITabPage> tpSignature;
/*			UI::GUILabel *lblSignatureHash;
			NotNullPtr<UI::GUIComboBox> cboSignatureHash;
			UI::GUILabel *lblSignaturePayloadFile;
			UI::GUITextBox *txtSignaturePayloadFile;
			NotNullPtr<UI::GUIButton> btnSignature;
			UI::GUILabel *lblSiguatureValue;
			UI::GUITextBox *txtSignatureValue;*/

			NotNullPtr<UI::GUITabPage> tpEncrypt;
			UI::GUILabel *lblEncryptInputType;
			NotNullPtr<UI::GUIComboBox> cboEncryptInputType;
			UI::GUILabel *lblEncryptOutputType;
			NotNullPtr<UI::GUIComboBox> cboEncryptOutputType;
			UI::GUILabel *lblEncryptRSAPadding;
			NotNullPtr<UI::GUIComboBox> cboEncryptRSAPadding;
			UI::GUILabel *lblEncryptInput;
			UI::GUITextBox *txtEncryptInput;
			NotNullPtr<UI::GUIButton> btnEncryptEncrypt;
			NotNullPtr<UI::GUIButton> btnEncryptDecrypt;
			UI::GUILabel *lblEncryptOutput;
			UI::GUITextBox *txtEncryptOutput;

			static UOSInt AddHash(NotNullPtr<UI::GUIComboBox> cbo, Crypto::Hash::HashType hashType, Crypto::Hash::HashType targetType);
			static void AddHashTypes(NotNullPtr<UI::GUIComboBox> cbo, Crypto::Hash::HashType hashType);
			static Bool FileIsSign(NotNullPtr<Text::String> fileName);
			static void __stdcall OnVerifyClicked(void *userObj);
			static void __stdcall OnVerifySignInfoClicked(void *userObj);
			static void __stdcall OnEncryptEncryptClicked(void *userObj);
			static void __stdcall OnEncryptDecryptClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			UOSInt ParseSignature(Text::PString *s, UInt8 *sign);
			Crypto::Cert::X509Key *GetNewKey();
		public:
			AVIRASN1DataForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Net::ASN1Data> asn1);
			virtual ~AVIRASN1DataForm();

			virtual void OnMonitorChanged();
			virtual void EventMenuClicked(UInt16 cmdId);
		};
	}
}
#endif
