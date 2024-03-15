#ifndef _SM_SSWR_AVIREAD_AVIRBATCHRENAMEFORM
#define _SM_SSWR_AVIREAD_AVIRBATCHRENAMEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBatchRenameForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblDirectory;
			NotNullPtr<UI::GUITextBox> txtDirectory;
			NotNullPtr<UI::GUIButton> btnRenameExtUpper;
			NotNullPtr<UI::GUIButton> btnRenameExtLower;

			static void __stdcall OnRenameExtUpperClicked(AnyType userObj);
			static void __stdcall OnRenameExtLowerClicked(AnyType userObj);
		public:
			AVIRBatchRenameForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBatchRenameForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
