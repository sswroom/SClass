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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			UI::GUILabel *lblKey;
			UI::GUITextBox *txtKey;
			UI::GUILabel *lblImagerySet;
			NotNullPtr<UI::GUIComboBox> cboImagerySet;
			NotNullPtr<UI::GUIButton> btnOK;
			Map::TileMap *tileMap;

			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRBingMapsForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRBingMapsForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
