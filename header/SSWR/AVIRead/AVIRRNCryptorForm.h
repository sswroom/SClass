#ifndef _SM_SSWR_AVIREAD_AVIRRNCRYPTORFORM
#define _SM_SSWR_AVIREAD_AVIRRNCRYPTORFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRNCryptorForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblSourceFile;
			UI::GUITextBox *txtSourceFile;
			UI::GUILabel *lblDestFile;
			UI::GUITextBox *txtDestFile;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUIButton *btnProcess;

			static void __stdcall OnProcessClicked(void *userObj);
			static void __stdcall OnFiles(void *userObj, Text::String **files, UOSInt nFiles);
		public:
			AVIRRNCryptorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRRNCryptorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif