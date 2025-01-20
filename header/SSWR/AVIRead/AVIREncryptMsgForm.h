#ifndef _SM_SSWR_AVIREAD_AVIRENCRYPTMSGFORM
#define _SM_SSWR_AVIREAD_AVIRENCRYPTMSGFORM
#include "Crypto/Encrypt/Encryption.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/TextBinEnc.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblAlgorithm;
			NN<UI::GUIComboBox> cboAlgorithm;
			NN<UI::GUILabel> lblKeyType;
			NN<UI::GUIComboBox> cboKeyType;
			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUILabel> lblIV;
			NN<UI::GUIComboBox> cboIV;
			NN<UI::GUITextBox> txtIV;
			NN<UI::GUILabel> lblChainMode;
			NN<UI::GUIComboBox> cboChainMode;
			NN<UI::GUILabel> lblInputType;
			NN<UI::GUIComboBox> cboInputType;
			NN<UI::GUILabel> lblInputMsg;
			NN<UI::GUITextBox> txtInputMsg;
			NN<UI::GUILabel> lblOutputType;
			NN<UI::GUIComboBox> cboOutputType;
			NN<UI::GUIButton> btnEncrypt;
			NN<UI::GUIButton> btnDecrypt;
			NN<UI::GUILabel> lblOutputMsg;
			NN<UI::GUITextBox> txtOutputMsg;

			Optional<Crypto::Encrypt::Encryption> InitCrypto();
			UnsafeArrayOpt<UInt8> InitInput(UOSInt blockSize, OutParam<UOSInt> dataSize);
			UnsafeArrayOpt<UInt8> InitIV(NN<Crypto::Encrypt::Encryption> crypto, UnsafeArray<UInt8> dataBuff, InOutParam<UOSInt> buffSize, UOSInt blockSize, Bool enc);
			void ShowOutput(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			static void __stdcall OnEncryptClicked(AnyType userObj);
			static void __stdcall OnDecryptClicked(AnyType userObj);
			static NN<Text::TextBinEnc::TextBinEnc> GetTextEncType(NN<UI::GUIComboBox> cbo);
			static void AddTextEncType(NN<UI::GUIComboBox> cbo);
		public:
			AVIREncryptMsgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREncryptMsgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
