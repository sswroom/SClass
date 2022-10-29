#ifndef _SM_SSWR_AVIREAD_AVIRFILETEXTENCRYPTFORM
#define _SM_SSWR_AVIREAD_AVIRFILETEXTENCRYPTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/TextBinEncList.h"
#include "UI/GUIButton.h"
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
			UI::GUILabel *lblEncrypt;
			UI::GUIComboBox *cboEncrypt;
			UI::GUIButton *btnConvert;

			SSWR::AVIRead::AVIRCore *core;
			Text::TextBinEnc::TextBinEncList encList;
			static void __stdcall OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIRFileTextEncryptForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRFileTextEncryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
