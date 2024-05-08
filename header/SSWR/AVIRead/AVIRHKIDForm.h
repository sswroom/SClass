#ifndef _SM_SSWR_AVIREAD_AVIRHKIDFORM
#define _SM_SSWR_AVIREAD_AVIRHKIDFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHKIDForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblHKID;
			NN<UI::GUITextBox> txtHKID;
			NN<UI::GUIButton> btnCheck;
			NN<UI::GUILabel> lblHKIDResult;

			static void __stdcall OnCheckClicked(AnyType userObj);
		public:
			AVIRHKIDForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKIDForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
