#ifndef _SM_SSWR_MAPREPLAY_MAPREPLAYFORM
#define _SM_SSWR_MAPREPLAY_MAPREPLAYFORM
#include "Map/MapRenderer.h"
#include "Map/MultiMapRenderer.h"
#include "Map/TileMap.h"
#include "Map/MapEnv.h"
#include "Media/ColorManager.h"
#include "Net/SocketFactory.h"
#include "UI/MSWindowForm.h"
#include "UI/MSWindowHSplitter.h"
#include "UI/MSWindowListBox.h"
#include "UI/MSWindowMapControl.h"
#include "Ui/MSWindowMainMenu.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"

namespace SSWR
{
	namespace MapReplay
	{
		class MapReplayForm : public UI::MSWindowForm, public SSWR::AVIRead::IMapNavigator
		{
		private:
			UI::MSWindowMapControl *map;
			UI::MSWindowMainMenu *mnu;
			UI::MSWindowListBox *lbLayers;
			UI::MSWindowHSplitter *splitter;
			SSWR::AVIRead::AVIRGISReplayForm *replayForm;

			Map::MapEnv *env;
			Media::GDIEngine *eng;
			Map::DrawMapRenderer *envRenderer;
			Map::TileMap *tileMap;
			Int32 currMapType;
			Parser::ParserList *parsers;
			Media::ColorManager *colorMgr;
			Net::SocketFactory *sockf;
			Media::ColorManagerSess *colorSess;
			Map::MapView *mapView;

		private:
			void LoadMap(Int32 mapType);
			void UpdateList();

			static void __stdcall OnFileDrop(void *userObj, const WChar **files, OSInt nFiles);
			static void __stdcall OnLayerDblClicked(void *userObj);
		public:
			MapReplayForm(UI::MSWindowUI *ui, UI::MSWindowForm *parent, Media::GDIEngine *eng, Parser::ParserList *parsers, Media::ColorManager *colorMgr, Net::SocketFactory *sockf);
			virtual ~MapReplayForm();

			virtual void EventMenuClicked(UInt16 cmdId);

			virtual Bool InMap(Double lat, Double lon);
			virtual void PanToMap(Double lat, Double lon);
			virtual void ShowMarker(Double lat, Double lon);
			virtual void HideMarker();
			virtual void AddLayer(Map::IMapDrawLayer *layer);
			virtual void SetSelectedVector(Math::Vector2D *vec);

			virtual void SetKMapEnv(const WChar *kmapIP, Int32 kmapPort, Int32 lcid);
			virtual Bool HasKMap();
			virtual WChar *ResolveAddress(WChar *sbuff, Double lat, Double lon);

		};
	};
};

#endif
