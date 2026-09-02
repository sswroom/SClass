#ifndef _SM_SSWR_AVIREAD_AVIRSYSTEMLOGFORM
#define _SM_SSWR_AVIREAD_AVIRSYSTEMLOGFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSystemLogForm : public UI::GUIForm
		{
		private:
			NN<UI::ListBoxLogger> logger;
			NN<SSWR::AVIRead::AVIRCore> core;

		public:
			AVIRSystemLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSystemLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
