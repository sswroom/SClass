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
			UI::GUITextBox *txtRoadRoute;
			NotNullPtr<UI::GUIButton> btnRoadRoute;
			NotNullPtr<UI::GUILabel> lblVehicleRes;
			UI::GUITextBox *txtVehicleRes;
			NotNullPtr<UI::GUIButton> btnVehicleRes;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

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
