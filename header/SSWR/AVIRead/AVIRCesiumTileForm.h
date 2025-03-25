#ifndef _SM_SSWR_AVIREAD_AVIRCESIUMTILEFORM
#define _SM_SSWR_AVIREAD_AVIRCESIUMTILEFORM
#include "Map/CesiumTile.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCesiumTileForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUIButton> btnOpen;
			NN<UI::GUIListBox> lbTiles;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<Map::CesiumTile> tileList;
			NN<Net::WebServer::WebListener> listener;
			NN<Net::WebServer::WebServiceHandler> hdlr;

			static void __stdcall OnOpenClicked(AnyType userObj);
		public:
			AVIRCesiumTileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ArrayListNN<Map::CesiumTile>> tiles);
			virtual ~AVIRCesiumTileForm();

			virtual void OnMonitorChanged();
			void AddTile(NN<Map::CesiumTile> tile);
			void AddTiles(NN<Data::ArrayListNN<Map::CesiumTile>> tiles);
		};
	}
}
#endif
