#ifndef _SM_SSWR_AVIREAD_AVIRBINGMAPSFORM
#define _SM_SSWR_AVIREAD_AVIRBINGMAPSFORM
#include "Map/TileMap.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBingMapsForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUILabel> lblImagerySet;
			NN<UI::GUIComboBox> cboImagerySet;
			NN<UI::GUIButton> btnOK;
			Optional<Map::TileMap> tileMap;

			static void __stdcall OnOKClicked(AnyType userObj);
		public:
			AVIRBingMapsForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRBingMapsForm();

			virtual void OnMonitorChanged();

			Optional<Map::TileMap> GetTileMap();
		};
	}
}
#endif
