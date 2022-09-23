#ifndef _SM_SSWR_AVIREAD_AVIRWMTSFORM
#define _SM_SSWR_AVIREAD_AVIRWMTSFORM
#include "Map/WebMapTileServiceSource.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWMTSForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblWMTSURL;
			UI::GUITextBox *txtWMTSURL;
			UI::GUIButton *btnLoad;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUIButton *btnOK;

			SSWR::AVIRead::AVIRCore *core;
			Map::WebMapTileServiceSource *wmts;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRWMTSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWMTSForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
