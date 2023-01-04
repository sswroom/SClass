#ifndef _SM_SSWR_AVIREAD_AVIRSAMLDECRYPTFORM
#define _SM_SSWR_AVIREAD_AVIRSAMLDECRYPTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSAMLDecryptForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKey;
			UI::GUILabel *lblRAWResponse;
			UI::GUITextBox *txtRAWResponse;
			UI::GUIButton *btnDecrypt;
			UI::GUILabel *lblResult;
			UI::GUITextBox *txtResult;

		private:
			static void __stdcall OnFormFiles(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnDecryptClicked(void *userObj);
		public:
			AVIRSAMLDecryptForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSAMLDecryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
