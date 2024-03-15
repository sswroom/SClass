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
			NotNullPtr<UI::GUIButton> btn;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OKClicked(AnyType userObj);
		public:
			AVIRAboutForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRAboutForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
