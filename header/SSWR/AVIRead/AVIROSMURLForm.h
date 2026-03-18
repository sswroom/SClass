#ifndef _SM_SSWR_AVIREAD_AVIROSMURLFORM
#define _SM_SSWR_AVIREAD_AVIROSMURLFORM
#include "Map/TileMap.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROSMURLForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Map::TileMap> tileMap;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblMinZoom;
			NN<UI::GUITextBox> txtMinZoom;
			NN<UI::GUILabel> lblMaxZoom;
			NN<UI::GUITextBox> txtMaxZoom;
			NN<UI::GUILabel> lblMinLon;
			NN<UI::GUITextBox> txtMinLon;
			NN<UI::GUILabel> lblMinLat;
			NN<UI::GUITextBox> txtMinLat;
			NN<UI::GUILabel> lblMaxLon;
			NN<UI::GUITextBox> txtMaxLon;
			NN<UI::GUILabel> lblMaxLat;
			NN<UI::GUITextBox> txtMaxLat;
			NN<UI::GUIButton> btnOpen;

			static void __stdcall OnOpenClicked(AnyType userObj);
		public:
			AVIROSMURLForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
			virtual ~AVIROSMURLForm();

			virtual void OnMonitorChanged();

			Optional<Map::TileMap> GetTileMap();
		};
	}
}
#endif
