#ifndef _SM_SSWR_AVIREAD_AVIRCOORDSYSFORM
#define _SM_SSWR_AVIREAD_AVIRCOORDSYSFORM
#include "Math/CoordinateSystem.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCoordSysForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITextBox> txtDesc;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Math::CoordinateSystem> csys;

		public:
			AVIRCoordSysForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Math::CoordinateSystem> csys);
			virtual ~AVIRCoordSysForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
