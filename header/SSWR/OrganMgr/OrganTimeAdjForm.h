#ifndef _SM_SSWR_ORGANMGR_ORGANTIMEADJFORM
#define _SM_SSWR_ORGANMGR_ORGANTIMEADJFORM
#include "Data/StringUTF8Map.h"
#include "Map/MapView.h"
#include "Map/TileMapLayer.h"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "SSWR/OrganMgr/OrganTimeAdjLayer.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUIMapControl.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganTimeAdjForm : public UI::GUIForm
		{
		private:
			OrganEnv *env;
			DataFileInfo *dataFile;
			Map::GPSTrack *gpsTrk;
			Bool mapUpdated;
			Map::MapEnv *mapEnv;
			Map::MapView *mapView;
			Map::TileMapLayer *mapTileLyr;
			OrganTimeAdjLayer *adjLyr;
			Map::TileMap *mapTile;
			Map::DrawMapRenderer *mapRenderer;
			Media::ColorManagerSess *colorSess;
			Data::ArrayList<UserFileInfo*> *userFileList;
			Data::ArrayList<UserFileInfo*> *currFileList;
			Media::ImageList *dispImg;
			Data::StringUTF8Map<Int32> *cameraMap;
			Text::String *selImgCamera;
			Int64 selImgTimeTicks;

			UI::GUIPanel *pnlLeft;
			UI::GUIPictureBoxDD *pbPreview;
			UI::GUIListBox *lbTrack;
			UI::GUIComboBox *cboSpecies;
			UI::GUIListBox *lbPictures;
			UI::GUIPanel *pnlMapCtrl;
			UI::GUIPanel *pnlControl;
			UI::GUITrackBar *tbMapScale;
			UI::GUIMapControl *mapMain;
			UI::GUIComboBox *cboCamera;
			UI::GUITextBox *txtTimeAdj;
			UI::GUIButton *btnPaste;
			UI::GUIButton *btnTimeSub;
			UI::GUIButton *btnTimeAdd;
			UI::GUIButton *btnTimeApply;

			static void __stdcall OnSpeciesChg(void *userObj);
			static void __stdcall OnTileUpdated(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnMapScaleScroll(void *userObj, UOSInt newVal);
			static void __stdcall OnTrackChg(void *userObj);
			static void __stdcall OnPictureChg(void *userObj);
			static void __stdcall OnCameraChg(void *userObj);
			static void __stdcall OnPasteClicked(void *userObj);
			static void __stdcall OnTimeAddClicked(void *userObj);
			static void __stdcall OnTimeSubClicked(void *userObj);
			static void __stdcall OnTimeApplyClicked(void *userObj);
			void UpdateSelTime(const UTF8Char *camera, Int32 timeAdj);
		public:
			OrganTimeAdjForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env, DataFileInfo *dataFile);
			virtual ~OrganTimeAdjForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif