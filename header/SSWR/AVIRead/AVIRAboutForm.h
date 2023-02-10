#ifndef _SM_SSWR_AVIREAD_AVIRABOUTFORM
#define _SM_SSWR_AVIREAD_AVIRABOUTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRAboutForm : public UI::GUIForm
		{
		private:
			UI::GUIButton *btn;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OKClicked(void *userObj);
		public:
			AVIRAboutForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRAboutForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
