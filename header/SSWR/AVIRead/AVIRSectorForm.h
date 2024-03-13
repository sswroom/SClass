#ifndef _SM_SSWR_AVIREAD_AVIRSECTORFORM
#define _SM_SSWR_AVIREAD_AVIRSECTORFORM
#include "IO/ISectorData.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSectorForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnParse;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::ISectorData> data;

			UInt8 *sectorData;
			UOSInt sectorSize;

			static void __stdcall OnParseClicked(void *userObj);
		public:
			AVIRSectorForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::ISectorData> data);
			virtual ~AVIRSectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
