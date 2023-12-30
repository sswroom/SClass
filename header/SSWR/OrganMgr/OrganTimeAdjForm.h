#ifndef _SM_SSWR_ORGANMGR_ORGANTIMEADJFORM
#define _SM_SSWR_ORGANMGR_ORGANTIMEADJFORM
#include "Data/StringMap.h"
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
			NotNullPtr<Map::GPSTrack> gpsTrk;
			Bool mapUpdated;
			NotNullPtr<Map::MapEnv> mapEnv;
			NotNullPtr<Map::MapView> mapView;
			NotNullPtr<Map::TileMapLayer> mapTileLyr;
			NotNullPtr<OrganTimeAdjLayer> adjLyr;
			NotNullPtr<Map::TileMap> mapTile;
			Map::DrawMapRenderer *mapRenderer;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Data::ArrayList<UserFileInfo*> *userFileList;
			Data::ArrayList<UserFileInfo*> *currFileList;
			Media::ImageList *dispImg;
			Data::StringMap<Int32> *cameraMap;
			Text::String *selImgCamera;
			Data::Timestamp selImgTime;

			NotNullPtr<UI::GUIPanel> pnlLeft;
			NotNullPtr<UI::GUIPictureBoxDD> pbPreview;
			NotNullPtr<UI::GUIListBox> lbTrack;
			NotNullPtr<UI::GUIComboBox> cboSpecies;
			NotNullPtr<UI::GUIListBox> lbPictures;
			NotNullPtr<UI::GUIPanel> pnlMapCtrl;
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUITrackBar> tbMapScale;
			UI::GUIMapControl *mapMain;
			NotNullPtr<UI::GUIComboBox> cboCamera;
			NotNullPtr<UI::GUITextBox> txtTimeAdj;
			NotNullPtr<UI::GUIButton> btnPaste;
			NotNullPtr<UI::GUIButton> btnTimeSub;
			NotNullPtr<UI::GUIButton> btnTimeAdd;
			NotNullPtr<UI::GUIButton> btnTimeApply;

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
			void UpdateSelTime(const UTF8Char *camera, UOSInt cameraLen, Int32 timeAdj);
		public:
			OrganTimeAdjForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env, DataFileInfo *dataFile);
			virtual ~OrganTimeAdjForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif