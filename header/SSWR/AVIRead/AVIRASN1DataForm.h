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
			Net::ASN1Data *asn1;

			UI::GUIMainMenu *mnuMain;

			UI::GUIPanel *pnlStatus;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpDesc;
			UI::GUITextBox *txtDesc;
			
			UI::GUITabPage *tpASN1;
			UI::GUITextBox *txtASN1;

			UI::GUITabPage *tpVerify;
			UI::GUILabel *lblVerifyHash;
			UI::GUIComboBox *cboVerifyHash;
			UI::GUILabel *lblVerifyPayloadFile;
			UI::GUITextBox *txtVerifyPayloadFile;
			UI::GUILabel *lblVerifySignature;
			UI::GUITextBox *txtVerifySignature;
			UI::GUIButton *btnVerify;
			UI::GUIButton *btnVerifySignInfo;
			UI::GUILabel *lblVerifyStatus;
			UI::GUITextBox *txtVerifyStatus;

			UI::GUITabPage *tpSignature;
/*			UI::GUILabel *lblSignatureHash;
			UI::GUIComboBox *cboSignatureHash;
			UI::GUILabel *lblSignaturePayloadFile;
			UI::GUITextBox *txtSignaturePayloadFile;
			UI::GUIButton *btnSignature;
			UI::GUILabel *lblSiguatureValue;
			UI::GUITextBox *txtSignatureValue;*/

			UI::GUITabPage *tpEncrypt;
			UI::GUILabel *lblEncryptInputType;
			UI::GUIComboBox *cboEncryptInputType;
			UI::GUILabel *lblEncryptOutputType;
			UI::GUIComboBox *cboEncryptOutputType;
			UI::GUILabel *lblEncryptRSAPadding;
			UI::GUIComboBox *cboEncryptRSAPadding;
			UI::GUILabel *lblEncryptInput;
			UI::GUITextBox *txtEncryptInput;
			UI::GUIButton *btnEncryptEncrypt;
			UI::GUIButton *btnEncryptDecrypt;
			UI::GUILabel *lblEncryptOutput;
			UI::GUITextBox *txtEncryptOutput;

			static UOSInt AddHash(UI::GUIComboBox *cbo, Crypto::Hash::HashType hashType, Crypto::Hash::HashType targetType);
			static void AddHashTypes(UI::GUIComboBox *cbo, Crypto::Hash::HashType hashType);
			static Bool FileIsSign(NotNullPtr<Text::String> fileName);
			static void __stdcall OnVerifyClicked(void *userObj);
			static void __stdcall OnVerifySignInfoClicked(void *userObj);
			static void __stdcall OnEncryptEncryptClicked(void *userObj);
			static void __stdcall OnEncryptDecryptClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			UOSInt ParseSignature(Text::PString *s, UInt8 *sign);
			Crypto::Cert::X509Key *GetNewKey();
		public:
			AVIRASN1DataForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Net::ASN1Data *asn1);
			virtual ~AVIRASN1DataForm();

			virtual void OnMonitorChanged();
			virtual void EventMenuClicked(UInt16 cmdId);
		};
	}
}
#endif
