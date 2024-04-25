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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Text::String> fileName;

			NN<UI::GUILabel> lblNewName;
			NN<UI::GUITextBox> txtNewName;
			NN<UI::GUITextBox> txtNewExt;
			NN<UI::GUIButton> btnRename;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnRenameClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);

		public:
			AVIRFileRenameForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Text::String> fileName);
			virtual ~AVIRFileRenameForm();

			virtual void OnMonitorChanged();
			NN<Text::String> GetFileName() const;
		};
	}
}
#endif
