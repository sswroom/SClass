#ifndef _SM_SSWR_ORGANMGR_ORGANMAINFORM
#define _SM_SSWR_ORGANMGR_ORGANMAINFORM
#include "Data/FastMapNN.h"
#include "Map/DrawMapRenderer.h"
#include "Map/TileMap.h"
#include "Map/TileMapLayer.h"
#include "Media/ImageList.h"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "SSWR/OrganMgr/OrganSpImgLayer.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIMapControl.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganMainForm : public UI::GUIForm, public UI::GUIDropHandler
		{
		private:
			typedef enum
			{
				IM_EMPTY,
				IM_GROUP,
				IM_SPECIES
			} InputMode;

		private:
			NN<UI::GUIPanel> pnlLeft;
			NN<UI::GUILabel> lblLeft;
			NN<UI::GUIListBox> lbDir;
			NN<UI::GUIHSplitter> hsbLeft;

			NN<UI::GUIPanel> pnlMid;
			NN<UI::GUILabel> lblMid;
			NN<UI::GUIListBox> lbObj;
			NN<UI::GUIPanel> pnlMidBottom;
			NN<UI::GUILabel> lblPickMsg;
			NN<UI::GUIButton> btnObjPick;
			NN<UI::GUIButton> btnObjPlace;
			NN<UI::GUIButton> btnObjCombine;
			NN<UI::GUIHSplitter> hsbMid;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpGroup;
			NN<UI::GUITextBox> txtGroupId;
			NN<UI::GUIComboBox> cboGroupType;
			NN<UI::GUITextBox> txtGroupEName;
			NN<UI::GUITextBox> txtGroupCName;
			NN<UI::GUICheckBox> chkGroupAdmin;
			NN<UI::GUITextBox> txtGroupDesc;
			NN<UI::GUITextBox> txtGroupKey;
			NN<UI::GUIButton> btnGroupRemove;
			NN<UI::GUIButton> btnGroupAdd;
			NN<UI::GUIButton> btnGroupEnter;

			NN<UI::GUITabPage> tpSpecies;
			NN<UI::GUIPanel> pnlSpecies;
			NN<UI::GUITextBox> txtSpeciesId;
			NN<UI::GUITextBox> txtSpeciesCName;
			NN<UI::GUITextBox> txtSpeciesSName;
			NN<UI::GUITextBox> txtSpeciesEName;
			NN<UI::GUITextBox> txtSpeciesDName;
			NN<UI::GUITextBox> txtSpeciesDesc;
			NN<UI::GUITextBox> txtSpeciesKey;
			NN<UI::GUIButton> btnSpeciesRemove;
			NN<UI::GUIButton> btnSpeciesAdd;
			NN<UI::GUIButton> btnSpeciesPasteSName;
			NN<UI::GUIButton> btnSpeciesColor;
			NN<UI::GUIGroupBox> grpSpBook;
			NN<UI::GUIPanel> pnlSpBook;
			NN<UI::GUITextBox> txtSpBook;
			NN<UI::GUITextBox> txtSpBookYear;
			NN<UI::GUIComboBox> cboSpBook;
			NN<UI::GUIButton> btnSpBookAdd;
			NN<UI::GUIPanel> pnlSpBookCtrl;
			NN<UI::GUIButton> btnSpBookDel;
			NN<UI::GUIListView> lvSpBook;

			NN<UI::GUITabPage> tpImage;
			NN<UI::GUIPictureBoxDD> pbImg;
			NN<UI::GUIVSplitter> vsbImg;
			NN<UI::GUIListBox> lbImage;
			NN<UI::GUIPanel> pnlImage;
			NN<UI::GUIButton> btnImageSaveAll;
			NN<UI::GUIButton> btnImageSave;
			NN<UI::GUIButton> btnImagePick;
			NN<UI::GUIButton> btnImagePickAdd;
			NN<UI::GUIButton> btnImagePickAll;
			NN<UI::GUIButton> btnImageDir;
			NN<UI::GUIButton> btnImageRotate;
			NN<UI::GUIButton> btnImageCrop;
			NN<UI::GUIButton> btnImageClipboard;

			NN<UI::GUITabPage> tpMap;
			NN<UI::GUIPanel> pnlMapCtrl;
			NN<UI::GUIPanel> pnlMapStatus;
			NN<UI::GUITrackBar> tbMapScale;
			NN<UI::GUITextBox> txtMapPos;
			NN<UI::GUITextBox> txtMapScale;
			UI::GUIMapControl *mcMap;

			NN<UI::GUIMainMenu> mnuMain;

			NN<Media::ColorManager> colorMgr;
			NN<Media::ColorManagerSess> colorSess;
			NN<OrganEnv> env;
			NN<OrganGroup> rootGroup;
			UOSInt lastDirIndex;
			UOSInt lastObjIndex;
			Bool restoreObj;
			OrganSpecies *lastSpeciesObj;
			OrganGroup *lastGroupObj;
			InputMode inputMode;
			Data::ArrayListNN<OrganGroupItem> groupItems;
			Data::ArrayListNN<OrganGroup> groupList;
			Data::ArrayListNN<OrganImageItem> imgItems;
			Bool indexChanged;
			Media::StaticImage *lastBmp;
			Bool newDirName;
			Data::ArrayListNN<OrganGroupItem> pickObjs;

			Optional<Media::ImageList> dispImage;
			Optional<UserFileInfo> dispImageUF;
			Optional<WebFileInfo> dispImageWF;
			Bool dispImageToCrop;
			Bool dispImageDown;
			Math::Coord2D<OSInt> dispImageDownPos;
			Math::Coord2D<OSInt> dispImageCurrPos;

			Map::DrawMapRenderer *mapRenderer;
			NN<Map::MapView> mapView;
			NN<Map::MapEnv> mapEnv;
			NN<Map::TileMapLayer> mapTileLyr;
			NN<Map::TileMap> mapTile;
			Bool mapUpdated;
			Data::FastMapNN<UInt32, OrganSpImgLayer> mapImgLyrs;
			UOSInt imgFontStyle;

			Text::String *initSelObj;
			Text::String *initSelImg;

			Data::ArrayListNN<UserFileInfo> mapUFiles;
			UserFileInfo *mapCurrFile;
			Optional<Media::DrawImage> mapCurrImage;
			Media::Resizer::LanczosResizer8_C8 *mapResizer;

			UInt32 unkCnt;
//			UInt32 dragEff;

		private:
			static void __stdcall OnGroupEnterClick(AnyType userObj);
			static void __stdcall OnGroupAddClicked(AnyType userObj);
			static void __stdcall OnGroupRemoveClicked(AnyType userObj);
			static void __stdcall OnDirChanged(AnyType userObj);
			static void __stdcall OnObjDblClicked(AnyType userObj);
			static void __stdcall OnObjSelChg(AnyType userObj);
			static void __stdcall OnImgSelChg(AnyType userObj);
			static Bool __stdcall OnImgRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool __stdcall OnImgMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool __stdcall OnImgMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool __stdcall OnImgMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnImgDraw(AnyType userObj, UInt8 *imgPtr, UOSInt w, UOSInt h, UOSInt bpl);
			static void __stdcall OnImgDblClicked(AnyType userObj);
			static void __stdcall OnImgDirClicked(AnyType userObj);
			static void __stdcall OnImageRotateClicked(AnyType userObj);
			static void __stdcall OnImageCropClicked(AnyType userObj);
			static void __stdcall OnImageSaveClicked(AnyType userObj);
			static void __stdcall OnImageSaveAllClicked(AnyType userObj);
			static void __stdcall OnImageClipboardClicked(AnyType userObj);
			static void __stdcall OnSpAddClicked(AnyType userObj);
			static void __stdcall OnSpRemoveClicked(AnyType userObj);
			static void __stdcall OnSpPasteSNameClicked(AnyType userObj);
			static void __stdcall OnSpBookYearChg(AnyType userObj);
			static void __stdcall OnSpBookAddClicked(AnyType userObj);
			static void __stdcall OnSpBookSelChg(AnyType userObj);
			static void __stdcall OnSpeciesSNameChg(AnyType userObj);
			static void __stdcall OnSpeciesColorClicked(AnyType userObj);
			static void __stdcall OnTabSelChg(AnyType userObj);
			static void __stdcall OnImagePickClicked(AnyType userObj);
			static void __stdcall OnImagePickAddClicked(AnyType userObj);
			static void __stdcall OnImagePickAllClicked(AnyType userObj);
			static void __stdcall OnObjPickClicked(AnyType userObj);
			static void __stdcall OnObjPlaceClicked(AnyType userObj);
			static void __stdcall OnObjCombineClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnMapScaleScroll(AnyType userObj, UOSInt newVal);
			static void __stdcall OnTileUpdated(AnyType userObj);
			static void __stdcall OnMapMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnMapDraw(AnyType userObj, NN<Media::DrawImage> dimg, OSInt xOfst, OSInt yOfst);

			static OSInt __stdcall GroupCompare(NN<OrganGroupItem> group1, NN<OrganGroupItem> group2);

			Bool CalcCropRect(Math::Coord2D<OSInt> *rect);

			void UpdateDir();
			void UpdateImgDir();
			void UpdateSpBook();
			void UpdateSpBookList();
			void UpdatePicks();
			void ClearPicks();

			Bool ToSaveGroup();
			Bool ToSaveSpecies();
			Bool GroupFormValid();
			Bool SpeciesFormValid();

			void ClearSpeciesForm();
			void ClearGroupForm();
			void FillGroupCboBox();
			void InitCategory();
			void SelectGroup(NN<UI::GUIComboBox> cbo, Int32 groupType);
			void GoToDir(NN<OrganGroup> grp, Int32 parentId);

			void ClearImgLayers();
			NN<OrganSpImgLayer> GetImgLayer(UInt32 mapColor);
		public:
			OrganMainForm(NN<UI::GUICore> ui, Optional<UI::GUIClientControl> parent, NN<OrganEnv> env);
			virtual ~OrganMainForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual DragEffect DragEnter(NN<UI::GUIDropData> data);
			virtual void DropData(NN<UI::GUIDropData> data, OSInt x, OSInt y);

/*			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
			virtual ULONG STDMETHODCALLTYPE AddRef();
			virtual ULONG STDMETHODCALLTYPE Release();

			virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
			virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
			virtual HRESULT STDMETHODCALLTYPE DragLeave();
			virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);*/
		};
	}
}
#endif