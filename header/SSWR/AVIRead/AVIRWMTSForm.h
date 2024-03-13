#ifndef _SM_SSWR_AVIREAD_AVIRWMTSFORM
#define _SM_SSWR_AVIREAD_AVIRWMTSFORM
#include "Map/WebMapTileServiceSource.h"
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
		class AVIRWMTSForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblWMTSURL;
			NotNullPtr<UI::GUITextBox> txtWMTSURL;
			NotNullPtr<UI::GUIButton> btnLoad;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUILabel> lblLayer;
			NotNullPtr<UI::GUIComboBox> cboLayer;
			NotNullPtr<UI::GUILabel> lblMatrixSet;
			NotNullPtr<UI::GUIComboBox> cboMatrixSet;
			NotNullPtr<UI::GUILabel> lblResourceTileType;
			NotNullPtr<UI::GUIComboBox> cboResourceTileType;
			NotNullPtr<UI::GUILabel> lblResourceInfoType;
			NotNullPtr<UI::GUIComboBox> cboResourceInfoType;
			NotNullPtr<UI::GUIButton> btnOK;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Map::WebMapTileServiceSource *wmts;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnLayerSelChg(void *userObj);
			static void __stdcall OnMatrixSetSelChg(void *userObj);
			static void __stdcall OnResourceTileTypeSelChg(void *userObj);
			static void __stdcall OnResourceInfoTypeSelChg(void *userObj);
		public:
			AVIRWMTSForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRWMTSForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
