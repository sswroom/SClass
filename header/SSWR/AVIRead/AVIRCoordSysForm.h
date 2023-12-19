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
			NotNullPtr<UI::GUITextBox> txtDesc;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Math::CoordinateSystem *csys;

		public:
			AVIRCoordSysForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Math::CoordinateSystem *csys);
			virtual ~AVIRCoordSysForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
