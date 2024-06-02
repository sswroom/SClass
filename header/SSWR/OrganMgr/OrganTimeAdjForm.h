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
			NN<OrganEnv> env;
			NN<DataFileInfo> dataFile;
			NN<Map::GPSTrack> gpsTrk;
			Bool mapUpdated;
			NN<Map::MapEnv> mapEnv;
			NN<Map::MapView> mapView;
			NN<Map::TileMapLayer> mapTileLyr;
			NN<OrganTimeAdjLayer> adjLyr;
			NN<Map::TileMap> mapTile;
			Map::DrawMapRenderer *mapRenderer;
			NN<Media::ColorManagerSess> colorSess;
			Data::ArrayListNN<UserFileInfo> userFileList;
			Data::ArrayListNN<UserFileInfo> currFileList;
			Optional<Media::ImageList> dispImg;
			Data::StringMap<Int32> cameraMap;
			Text::String *selImgCamera;
			Data::Timestamp selImgTime;

			NN<UI::GUIPanel> pnlLeft;
			NN<UI::GUIPictureBoxDD> pbPreview;
			NN<UI::GUIListBox> lbTrack;
			NN<UI::GUIComboBox> cboSpecies;
			NN<UI::GUIListBox> lbPictures;
			NN<UI::GUIPanel> pnlMapCtrl;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUITrackBar> tbMapScale;
			UI::GUIMapControl *mapMain;
			NN<UI::GUIComboBox> cboCamera;
			NN<UI::GUITextBox> txtTimeAdj;
			NN<UI::GUIButton> btnPaste;
			NN<UI::GUIButton> btnTimeSub;
			NN<UI::GUIButton> btnTimeAdd;
			NN<UI::GUIButton> btnTimeApply;

			static void __stdcall OnSpeciesChg(AnyType userObj);
			static void __stdcall OnTileUpdated(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnMapScaleScroll(AnyType userObj, UOSInt newVal);
			static void __stdcall OnTrackChg(AnyType userObj);
			static void __stdcall OnPictureChg(AnyType userObj);
			static void __stdcall OnCameraChg(AnyType userObj);
			static void __stdcall OnPasteClicked(AnyType userObj);
			static void __stdcall OnTimeAddClicked(AnyType userObj);
			static void __stdcall OnTimeSubClicked(AnyType userObj);
			static void __stdcall OnTimeApplyClicked(AnyType userObj);
			void UpdateSelTime(UnsafeArray<const UTF8Char> camera, UOSInt cameraLen, Int32 timeAdj);
		public:
			OrganTimeAdjForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env, NN<DataFileInfo> dataFile);
			virtual ~OrganTimeAdjForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif