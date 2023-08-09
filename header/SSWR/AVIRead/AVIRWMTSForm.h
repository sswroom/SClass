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
			UI::GUILabel *lblWMTSURL;
			UI::GUITextBox *txtWMTSURL;
			UI::GUIButton *btnLoad;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblLayer;
			UI::GUIComboBox *cboLayer;
			UI::GUILabel *lblMatrixSet;
			UI::GUIComboBox *cboMatrixSet;
			UI::GUILabel *lblResourceTileType;
			UI::GUIComboBox *cboResourceTileType;
			UI::GUILabel *lblResourceInfoType;
			UI::GUIComboBox *cboResourceInfoType;
			UI::GUIButton *btnOK;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;
			Map::WebMapTileServiceSource *wmts;

			static void __stdcall OnLoadClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnLayerSelChg(void *userObj);
			static void __stdcall OnMatrixSetSelChg(void *userObj);
			static void __stdcall OnResourceTileTypeSelChg(void *userObj);
			static void __stdcall OnResourceInfoTypeSelChg(void *userObj);
		public:
			AVIRWMTSForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Net::SSLEngine *ssl);
			virtual ~AVIRWMTSForm();

			virtual void OnMonitorChanged();

			Map::TileMap *GetTileMap();
		};
	}
}
#endif
