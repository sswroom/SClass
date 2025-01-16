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
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnParse;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::ISectorData> data;

			UnsafeArray<UInt8> sectorData;
			UOSInt sectorSize;

			static void __stdcall OnParseClicked(AnyType userObj);
		public:
			AVIRSectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::ISectorData> data);
			virtual ~AVIRSectorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
