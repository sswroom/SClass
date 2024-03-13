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
			NotNullPtr<UI::GUILabel> lblJSText;
			NotNullPtr<UI::GUITextBox> txtJSText;
			NotNullPtr<UI::GUIButton> btnExtract;
			NotNullPtr<UI::GUILabel> lblOriText;
			NotNullPtr<UI::GUITextBox> txtOriText;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnExtractClicked(void *userObj);
		public:
			AVIRJSTextForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJSTextForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
