#ifndef _SM_SSWR_AVIREAD_AVIRJSTEXTFORM
#define _SM_SSWR_AVIREAD_AVIRJSTEXTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRJSTextForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblJSText;
			NN<UI::GUITextBox> txtJSText;
			NN<UI::GUIButton> btnExtract;
			NN<UI::GUILabel> lblOriText;
			NN<UI::GUITextBox> txtOriText;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnExtractClicked(AnyType userObj);
		public:
			AVIRJSTextForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJSTextForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
