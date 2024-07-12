#ifndef _SM_SSWR_AVIREAD_AVIRENCRYPTFILEFORM
#define _SM_SSWR_AVIREAD_AVIRENCRYPTFILEFORM
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
		class AVIREncryptFileForm : public UI::GUIForm
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
			NN<UI::GUILabel> lblPaddingMode;
			NN<UI::GUIComboBox> cboPaddingMode;
			NN<UI::GUILabel> lblInputFile;
			NN<UI::GUITextBox> txtInputFile;
			NN<UI::GUILabel> lblOutputFile;
			NN<UI::GUITextBox> txtOutputFile;
			NN<UI::GUIButton> btnEncrypt;
			NN<UI::GUIButton> btnDecrypt;

			Optional<Crypto::Encrypt::ICrypto> InitCrypto();
			UnsafeArrayOpt<UInt8> InitInput(UOSInt blockSize, OutParam<UOSInt> dataSize);
			UnsafeArrayOpt<UInt8> InitIV(NN<Crypto::Encrypt::ICrypto> crypto, UnsafeArray<UInt8> dataBuff, InOutParam<UOSInt> buffSize, UOSInt blockSize, Bool enc);
			void SaveOutput(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			void SetInputFile(Text::CStringNN file);
			static void __stdcall OnEncryptClicked(AnyType userObj);
			static void __stdcall OnDecryptClicked(AnyType userObj);
			static void __stdcall OnFilesDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static NN<Text::TextBinEnc::ITextBinEnc> GetTextEncType(NN<UI::GUIComboBox> cbo);
			static void AddTextEncType(NN<UI::GUIComboBox> cbo);
		public:
			AVIREncryptFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREncryptFileForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
