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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NN<Math::CoordinateSystem> envCSys;

			NN<UI::GUITextBox> txtDesc;
			NN<UI::GUIListView> lvMaps;
			Map::MapDrawLayer *layer;
			
			static void __stdcall OnMapsSelChg(AnyType userObj);
			static void __stdcall OnMapsDblClk(AnyType userObj, UOSInt index);
		public:
			AVIRRegionalMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NN<Math::CoordinateSystem> envCSys);
			virtual ~AVIRRegionalMapForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
