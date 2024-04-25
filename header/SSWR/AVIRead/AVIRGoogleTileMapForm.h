#ifndef _SM_SSWR_AVIREAD_AVIRGOOGLETILEMAPFORM
#define _SM_SSWR_AVIREAD_AVIRGOOGLETILEMAPFORM
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
		class AVIRGoogleTileMapForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUILabel> lblMapType;
			NN<UI::GUIComboBox> cboMapType;
			NN<UI::GUITextBox> txtDeclare;
			NN<UI::GUIButton> btnOK;
			Map::MapDrawLayer *layer;

			static void __stdcall OnOKClicked(AnyType userObj);
		public:
			AVIRGoogleTileMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRGoogleTileMapForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
