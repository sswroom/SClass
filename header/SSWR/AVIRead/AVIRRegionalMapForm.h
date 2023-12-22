#ifndef _SM_SSWR_AVIREAD_AVIRREGIONALMAPFORM
#define _SM_SSWR_AVIREAD_AVIRREGIONALMAPFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRegionalMapForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NotNullPtr<Math::CoordinateSystem> envCSys;

			NotNullPtr<UI::GUITextBox> txtDesc;
			NotNullPtr<UI::GUIListView> lvMaps;
			Map::MapDrawLayer *layer;
			
			static void __stdcall OnMapsSelChg(void *userObj);
			static void __stdcall OnMapsDblClk(void *userObj, UOSInt index);
		public:
			AVIRRegionalMapForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NotNullPtr<Math::CoordinateSystem> envCSys);
			virtual ~AVIRRegionalMapForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
