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

			NotNullPtr<UI::GUILabel> lblAlgorithm;
			NotNullPtr<UI::GUIComboBox> cboAlgorithm;
			NotNullPtr<UI::GUILabel> lblKeyType;
			NotNullPtr<UI::GUIComboBox> cboKeyType;
			NotNullPtr<UI::GUILabel> lblKey;
			NotNullPtr<UI::GUITextBox> txtKey;
			NotNullPtr<UI::GUILabel> lblIV;
			NotNullPtr<UI::GUIComboBox> cboIV;
			NotNullPtr<UI::GUITextBox> txtIV;
			NotNullPtr<UI::GUILabel> lblChainMode;
			NotNullPtr<UI::GUIComboBox> cboChainMode;
			NotNullPtr<UI::GUILabel> lblInputType;
			NotNullPtr<UI::GUIComboBox> cboInputType;
			NotNullPtr<UI::GUILabel> lblInputMsg;
			NotNullPtr<UI::GUITextBox> txtInputMsg;
			NotNullPtr<UI::GUILabel> lblOutputType;
			NotNullPtr<UI::GUIComboBox> cboOutputType;
			NotNullPtr<UI::GUIButton> btnEncrypt;
			NotNullPtr<UI::GUIButton> btnDecrypt;
			NotNullPtr<UI::GUILabel> lblOutputMsg;
			NotNullPtr<UI::GUITextBox> txtOutputMsg;

			Crypto::Encrypt::ICrypto *InitCrypto();
			UInt8 *InitInput(UOSInt blockSize, UOSInt *dataSize);
			UInt8 *InitIV(Crypto::Encrypt::ICrypto *crypto, UInt8 *dataBuff, UOSInt *buffSize, UOSInt blockSize, Bool enc);
			void ShowOutput(const UInt8 *buff, UOSInt buffSize);
			static void __stdcall OnEncryptClicked(AnyType userObj);
			static void __stdcall OnDecryptClicked(AnyType userObj);
			static Text::TextBinEnc::ITextBinEnc *GetTextEncType(NotNullPtr<UI::GUIComboBox> cbo);
			static void AddTextEncType(NotNullPtr<UI::GUIComboBox> cbo);
		public:
			AVIREncryptMsgForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREncryptMsgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
