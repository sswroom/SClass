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
			NN<UI::GUILabel> lblWMTSURL;
			NN<UI::GUITextBox> txtWMTSURL;
			NN<UI::GUIButton> btnLoad;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblLayer;
			NN<UI::GUIComboBox> cboLayer;
			NN<UI::GUILabel> lblMatrixSet;
			NN<UI::GUIComboBox> cboMatrixSet;
			NN<UI::GUILabel> lblResourceTileType;
			NN<UI::GUIComboBox> cboResourceTileType;
			NN<UI::GUILabel> lblResourceInfoType;
			NN<UI::GUIComboBox> cboResourceInfoType;
			NN<UI::GUIButton> btnOK;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Map::WebMapTileServiceSource> wmts;

			static void __stdcall OnLoadClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnLayerSelChg(AnyType userObj);
			static void __stdcall OnMatrixSetSelChg(AnyType userObj);
			static void __stdcall OnResourceTileTypeSelChg(AnyType userObj);
			static void __stdcall OnResourceInfoTypeSelChg(AnyType userObj);
		public:
			AVIRWMTSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl);
			virtual ~AVIRWMTSForm();

			virtual void OnMonitorChanged();

			Optional<Map::TileMap> GetTileMap();
		};
	}
}
#endif
