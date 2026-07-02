#ifndef _SM_SSWR_MAPREPLAY_MAPREPLAYFORM
#define _SM_SSWR_MAPREPLAY_MAPREPLAYFORM
#include "Map/MapRenderer.h"
#include "Map/MultiMapRenderer.h"
#include "Map/TileMap.h"
#include "Map/MapEnv.h"
#include "Media/ColorManager.h"
#include "Net/SocketFactory.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIMapControl.h"
#include "UI/GUIMainMenu.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"

namespace SSWR
{
	namespace MapReplay
	{
		class MapReplayForm : public UI::GUIForm, public SSWR::AVIRead::AVIRMapNavigator
		{
		private:
			NN<UI::GUIMapControl> map;
			NN<UI::GUIMainMenu> mnu;
			NN<UI::GUIListBox> lbLayers;
			NN<UI::GUIHSplitter> splitter;
			Optional<SSWR::AVIRead::AVIRGISReplayForm> replayForm;

			NN<Map::MapEnv> env;
			NN<Media::DrawEngine> eng;
			NN<Map::DrawMapRenderer> envRenderer;
			Optional<Map::TileMap> tileMap;
			Int32 currMapType;
			NN<Parser::ParserList> parsers;
			NN<Media::ColorManager> colorMgr;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			NN<Media::ColorManagerSess> colorSess;
			NN<Map::MapView> mapView;

		private:
			void LoadMap(Int32 mapType);
			void UpdateList();

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnLayerDblClicked(AnyType userObj);
		public:
			MapReplayForm(NN<UI::GUICore> ui, Optional<UI::GUIForm> parent, NN<Media::DrawEngine> eng, NN<Parser::ParserList> parsers, NN<Media::ColorManager> colorMgr, NN<Net::SocketFactory> sockf);
			virtual ~MapReplayForm();

			virtual void EventMenuClicked(UInt16 cmdId);

			virtual Bool InMap(Double lat, Double lon);
			virtual void PanToMap(Double lat, Double lon);
			virtual void ShowMarker(Double lat, Double lon);
			virtual void HideMarker();
			virtual void AddLayer(NN<Map::MapDrawLayer> layer);
			virtual void SetSelectedVector(Optional<Math::Geometry::Vector2D> vec);

			virtual void SetKMapEnv(const WChar *kmapIP, Int32 kmapPort, Int32 lcid);
			virtual Bool HasKMap();
			virtual WChar *ResolveAddress(WChar *sbuff, Double lat, Double lon);

		};
	};
};

#endif
