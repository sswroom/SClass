#ifndef _SM_SSWR_AVIREAD_AVIRFILETEXTENCRYPTFORM
#define _SM_SSWR_AVIREAD_AVIRFILETEXTENCRYPTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/TextBinEncList.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileTextEncryptForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblSrcFile;
			NN<UI::GUITextBox> txtSrcFile;
			NN<UI::GUILabel> lblDestFile;
			NN<UI::GUITextBox> txtDestFile;
			NN<UI::GUILabel> lblDecrypt;
			NN<UI::GUICheckBox> chkDecrypt;
			NN<UI::GUILabel> lblEncrypt;
			NN<UI::GUIComboBox> cboEncrypt;
			NN<UI::GUIButton> btnConvert;

			NN<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnConvertClicked(AnyType userObj);
			static void __stdcall OnDecryptChange(AnyType userObj, Bool newState);

			void GenDestFileName(Text::CStringNN fileName, Bool decrypt);
		public:
			AVIRFileTextEncryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileTextEncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
