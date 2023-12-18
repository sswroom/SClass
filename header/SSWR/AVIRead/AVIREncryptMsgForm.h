#ifndef _SM_SSWR_AVIREAD_AVIRENCRYPTMSGFORM
#define _SM_SSWR_AVIREAD_AVIRENCRYPTMSGFORM
#include "Crypto/Encrypt/ICrypto.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/ITextBinEnc.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREncryptMsgForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUILabel *lblAlgorithm;
			NotNullPtr<UI::GUIComboBox> cboAlgorithm;
			UI::GUILabel *lblKeyType;
			NotNullPtr<UI::GUIComboBox> cboKeyType;
			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKey;
			UI::GUILabel *lblIV;
			NotNullPtr<UI::GUIComboBox> cboIV;
			UI::GUITextBox *txtIV;
			UI::GUILabel *lblChainMode;
			NotNullPtr<UI::GUIComboBox> cboChainMode;
			UI::GUILabel *lblInputType;
			NotNullPtr<UI::GUIComboBox> cboInputType;
			UI::GUILabel *lblInputMsg;
			UI::GUITextBox *txtInputMsg;
			UI::GUILabel *lblOutputType;
			NotNullPtr<UI::GUIComboBox> cboOutputType;
			NotNullPtr<UI::GUIButton> btnEncrypt;
			NotNullPtr<UI::GUIButton> btnDecrypt;
			UI::GUILabel *lblOutputMsg;
			UI::GUITextBox *txtOutputMsg;

			Crypto::Encrypt::ICrypto *InitCrypto();
			UInt8 *InitInput(UOSInt blockSize, UOSInt *dataSize);
			UInt8 *InitIV(Crypto::Encrypt::ICrypto *crypto, UInt8 *dataBuff, UOSInt *buffSize, UOSInt blockSize, Bool enc);
			void ShowOutput(const UInt8 *buff, UOSInt buffSize);
			static void __stdcall OnEncryptClicked(void *userObj);
			static void __stdcall OnDecryptClicked(void *userObj);
			static Text::TextBinEnc::ITextBinEnc *GetTextEncType(NotNullPtr<UI::GUIComboBox> cbo);
			static void AddTextEncType(NotNullPtr<UI::GUIComboBox> cbo);
		public:
			AVIREncryptMsgForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREncryptMsgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
