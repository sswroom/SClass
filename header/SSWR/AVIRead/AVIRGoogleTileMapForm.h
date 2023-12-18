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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUILabel> lblMapType;
			NotNullPtr<UI::GUIComboBox> cboMapType;
			UI::GUITextBox *txtDeclare;
			NotNullPtr<UI::GUIButton> btnOK;
			Map::MapDrawLayer *layer;

			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRGoogleTileMapForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRGoogleTileMapForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
