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
			UI::GUILabel *lblSrcFile;
			UI::GUITextBox *txtSrcFile;
			UI::GUILabel *lblDestFile;
			UI::GUITextBox *txtDestFile;
			UI::GUILabel *lblDecrypt;
			UI::GUICheckBox *chkDecrypt;
			UI::GUILabel *lblEncrypt;
			NotNullPtr<UI::GUIComboBox> cboEncrypt;
			NotNullPtr<UI::GUIButton> btnConvert;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnConvertClicked(void *userObj);
			static void __stdcall OnDecryptChange(void *userObj, Bool newState);

			void GenDestFileName(Text::CStringNN fileName, Bool decrypt);
		public:
			AVIRFileTextEncryptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileTextEncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
