#ifndef _SM_SSWR_AVIREAD_AVIRGISHKTDTONNESFORM
#define _SM_SSWR_AVIREAD_AVIRGISHKTDTONNESFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISHKTDTonnesForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblRoadRoute;
			NN<UI::GUITextBox> txtRoadRoute;
			NN<UI::GUIButton> btnRoadRoute;
			NN<UI::GUILabel> lblVehicleRes;
			NN<UI::GUITextBox> txtVehicleRes;
			NN<UI::GUIButton> btnVehicleRes;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			Map::MapDrawLayer *lyr;

			static void __stdcall OnRoadRouteClicked(AnyType userObj);
			static void __stdcall OnVehicleResClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRGISHKTDTonnesForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGISHKTDTonnesForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
