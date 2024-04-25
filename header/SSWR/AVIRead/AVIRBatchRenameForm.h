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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblDirectory;
			NN<UI::GUITextBox> txtDirectory;
			NN<UI::GUIButton> btnRenameExtUpper;
			NN<UI::GUIButton> btnRenameExtLower;

			static void __stdcall OnRenameExtUpperClicked(AnyType userObj);
			static void __stdcall OnRenameExtLowerClicked(AnyType userObj);
		public:
			AVIRBatchRenameForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBatchRenameForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
