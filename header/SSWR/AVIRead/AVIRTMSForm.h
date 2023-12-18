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
			NotNullPtr<UI::GUILabel> lblTMSURL;
			UI::GUITextBox *txtTMSURL;
			NotNullPtr<UI::GUIButton> btnLoad;
			NotNullPtr<UI::GUILabel> lblStatus;
			UI::GUITextBox *txtStatus;
			NotNullPtr<UI::GUIButton> btnOK;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Map::TileMapServiceSource *tms;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRTMSForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRTMSForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
