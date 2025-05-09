#ifndef _SM_SSWR_AVIREAD_AVIRTMSFORM
#define _SM_SSWR_AVIREAD_AVIRTMSFORM
#include "Map/TileMapServiceSource.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTMSForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblTMSURL;
			NN<UI::GUITextBox> txtTMSURL;
			NN<UI::GUIButton> btnLoad;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUIButton> btnOK;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Map::TileMapServiceSource> tms;

			static void __stdcall OnLoadClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
		public:
			AVIRTMSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRTMSForm();

			virtual void OnMonitorChanged();

			Optional<Map::TileMap> GetTileMap();
		};
	}
}
#endif
