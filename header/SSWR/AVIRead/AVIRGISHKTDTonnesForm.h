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

			UI::GUILabel *lblRoadRoute;
			UI::GUITextBox *txtRoadRoute;
			UI::GUIButton *btnRoadRoute;
			UI::GUILabel *lblVehicleRes;
			UI::GUITextBox *txtVehicleRes;
			UI::GUIButton *btnVehicleRes;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			Map::MapDrawLayer *lyr;

			static void __stdcall OnRoadRouteClicked(void *userObj);
			static void __stdcall OnVehicleResClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRGISHKTDTonnesForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGISHKTDTonnesForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	};
};
#endif
