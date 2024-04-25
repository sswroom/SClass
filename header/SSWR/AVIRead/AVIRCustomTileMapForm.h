#ifndef _SM_SSWR_AVIREAD_AVIRCUSTOMTILEMAPFORM
#define _SM_SSWR_AVIREAD_AVIRCUSTOMTILEMAPFORM
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
		class AVIRCustomTileMapForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblMinLevel;
			NN<UI::GUITextBox> txtMinLevel;
			NN<UI::GUILabel> lblMaxLevel;
			NN<UI::GUITextBox> txtMaxLevel;
			NN<UI::GUIButton> btnOK;
			Map::TileMap *tileMap;

			static void __stdcall OnOKClicked(AnyType userObj);
		public:
			AVIRCustomTileMapForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRCustomTileMapForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
