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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Text::String> fileName;

			NotNullPtr<UI::GUILabel> lblNewName;
			UI::GUITextBox *txtNewName;
			UI::GUITextBox *txtNewExt;
			NotNullPtr<UI::GUIButton> btnRename;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnRenameClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

		public:
			AVIRFileRenameForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Text::String> fileName);
			virtual ~AVIRFileRenameForm();

			virtual void OnMonitorChanged();
			NotNullPtr<Text::String> GetFileName() const;
		};
	}
}
#endif
