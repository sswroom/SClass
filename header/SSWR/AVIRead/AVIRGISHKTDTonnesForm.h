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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblRoadRoute;
			NotNullPtr<UI::GUITextBox> txtRoadRoute;
			NotNullPtr<UI::GUIButton> btnRoadRoute;
			NotNullPtr<UI::GUILabel> lblVehicleRes;
			NotNullPtr<UI::GUITextBox> txtVehicleRes;
			NotNullPtr<UI::GUIButton> btnVehicleRes;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			Map::MapDrawLayer *lyr;

			static void __stdcall OnRoadRouteClicked(AnyType userObj);
			static void __stdcall OnVehicleResClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRGISHKTDTonnesForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGISHKTDTonnesForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
