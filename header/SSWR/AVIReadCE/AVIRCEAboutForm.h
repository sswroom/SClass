#ifndef _SM_SSWR_AVIREADCE_AVIRCEABOUTFORM
#define _SM_SSWR_AVIREADCE_AVIRCEABOUTFORM
#include "UI/GUIForm.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIReadCE
	{
		class AVIRCEAboutForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIButton> btn;

			static void __stdcall OKClicked(void *userObj);
		public:
			AVIRCEAboutForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui);
			virtual ~AVIRCEAboutForm();
		};
	};
};
#endif
