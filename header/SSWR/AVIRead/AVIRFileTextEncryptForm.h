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
			NotNullPtr<UI::GUILabel> lblSrcFile;
			NotNullPtr<UI::GUITextBox> txtSrcFile;
			NotNullPtr<UI::GUILabel> lblDestFile;
			NotNullPtr<UI::GUITextBox> txtDestFile;
			NotNullPtr<UI::GUILabel> lblDecrypt;
			NotNullPtr<UI::GUICheckBox> chkDecrypt;
			NotNullPtr<UI::GUILabel> lblEncrypt;
			NotNullPtr<UI::GUIComboBox> cboEncrypt;
			NotNullPtr<UI::GUIButton> btnConvert;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnConvertClicked(void *userObj);
			static void __stdcall OnDecryptChange(void *userObj, Bool newState);

			void GenDestFileName(Text::CStringNN fileName, Bool decrypt);
		public:
			AVIRFileTextEncryptForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileTextEncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
