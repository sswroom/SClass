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
			UI::GUILabel *lblTMSURL;
			UI::GUITextBox *txtTMSURL;
			UI::GUIButton *btnLoad;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUIButton *btnOK;

			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Map::TileMapServiceSource *tms;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRTMSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl);
			virtual ~AVIRTMSForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
