#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Text::String> path;

			NN<UI::GUIPanel> pnlDir;
			NN<UI::GUILabel> lblDir;
			NN<UI::GUITextBox> txtDir;
			NN<UI::GUIButton> btnDir;

			static void __stdcall OnDirClicked(AnyType userObj);
		
			Bool SaveSettings();
			void FreeAll();
		public:
			AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
