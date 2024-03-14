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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUILabel> lblURL;
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUILabel> lblMinLevel;
			NotNullPtr<UI::GUITextBox> txtMinLevel;
			NotNullPtr<UI::GUILabel> lblMaxLevel;
			NotNullPtr<UI::GUITextBox> txtMaxLevel;
			NotNullPtr<UI::GUIButton> btnOK;
			Map::TileMap *tileMap;

			static void __stdcall OnOKClicked(AnyType userObj);
		public:
			AVIRCustomTileMapForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRCustomTileMapForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
