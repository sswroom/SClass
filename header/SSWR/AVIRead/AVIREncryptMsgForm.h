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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblAlgorithm;
			UI::GUIComboBox *cboAlgorithm;
			UI::GUILabel *lblKeyIVType;
			UI::GUIComboBox *cboKeyIVType;
			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKey;
			UI::GUILabel *lblIV;
			UI::GUITextBox *txtIV;
			UI::GUILabel *lblChainMode;
			UI::GUIComboBox *cboChainMode;
			UI::GUILabel *lblInputType;
			UI::GUIComboBox *cboInputType;
			UI::GUILabel *lblInputMsg;
			UI::GUITextBox *txtInputMsg;
			UI::GUILabel *lblOutputType;
			UI::GUIComboBox *cboOutputType;
			UI::GUIButton *btnEncrypt;
			UI::GUIButton *btnDecrypt;
			UI::GUILabel *lblOutputMsg;
			UI::GUITextBox *txtOutputMsg;

			Crypto::Encrypt::ICrypto *InitCrypto();
			UInt8 *InitInput(UOSInt blockSize, UOSInt *dataSize);
			void ShowOutput(const UInt8 *buff, UOSInt buffSize);
			static void __stdcall OnEncryptClicked(void *userObj);
			static void __stdcall OnDecryptClicked(void *userObj);
			static Text::TextBinEnc::ITextBinEnc *GetTextEncType(UI::GUIComboBox *cbo);
			static void AddTextEncType(UI::GUIComboBox *cbo);
		public:
			AVIREncryptMsgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIREncryptMsgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif