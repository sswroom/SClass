#ifndef _SM_SSWR_AVIREAD_AVIRFILESIZEPACKFORM
#define _SM_SSWR_AVIREAD_AVIRFILESIZEPACKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileRenameForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			NotNullPtr<Text::String> fileName;

			UI::GUILabel *lblNewName;
			UI::GUITextBox *txtNewName;
			UI::GUITextBox *txtNewExt;
			UI::GUIButton *btnRename;
			UI::GUIButton *btnCancel;

			static void __stdcall OnRenameClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

		public:
			AVIRFileRenameForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, NotNullPtr<Text::String> fileName);
			virtual ~AVIRFileRenameForm();

			virtual void OnMonitorChanged();
			NotNullPtr<Text::String> GetFileName() const;
		};
	}
}
#endif
