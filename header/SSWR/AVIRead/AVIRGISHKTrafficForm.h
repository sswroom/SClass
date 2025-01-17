#ifndef _SM_SSWR_AVIREAD_AVIRGISHKTRAFFICFORM
#define _SM_SSWR_AVIREAD_AVIRGISHKTRAFFICFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISHKTrafficForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUILabel> lblRoadCenterline;
			NN<UI::GUITextBox> txtRoadCenterline;
			NN<UI::GUIButton> btnRoadCenterline;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			Optional<Map::MapDrawLayer> lyr;

			static void __stdcall OnRoadCenterlineClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRGISHKTrafficForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGISHKTrafficForm();

			virtual void OnMonitorChanged();

			Optional<Map::MapDrawLayer> GetMapLayer();
		};
	}
}
#endif
