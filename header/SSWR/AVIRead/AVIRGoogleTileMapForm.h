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
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;

			UI::GUILabel *lblMapType;
			UI::GUIComboBox *cboMapType;
			UI::GUITextBox *txtDeclare;
			UI::GUIButton *btnOK;
			Map::MapDrawLayer *layer;

			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRGoogleTileMapForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl);
			virtual ~AVIRGoogleTileMapForm();

			virtual void OnMonitorChanged();

			Map::MapDrawLayer *GetMapLayer();
		};
	}
}
#endif
