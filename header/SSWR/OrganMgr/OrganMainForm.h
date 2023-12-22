#ifndef _SM_SSWR_ORGANMGR_ORGANMAINFORM
#define _SM_SSWR_ORGANMGR_ORGANMAINFORM
#include "Data/FastMap.h"
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
			NotNullPtr<UI::GUIPanel> pnlLeft;
			NotNullPtr<UI::GUILabel> lblLeft;
			UI::GUIListBox *lbDir;
			NotNullPtr<UI::GUIHSplitter> hsbLeft;

			NotNullPtr<UI::GUIPanel> pnlMid;
			NotNullPtr<UI::GUILabel> lblMid;
			UI::GUIListBox *lbObj;
			NotNullPtr<UI::GUIPanel> pnlMidBottom;
			NotNullPtr<UI::GUILabel> lblPickMsg;
			NotNullPtr<UI::GUIButton> btnObjPick;
			NotNullPtr<UI::GUIButton> btnObjPlace;
			NotNullPtr<UI::GUIButton> btnObjCombine;
			NotNullPtr<UI::GUIHSplitter> hsbMid;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpGroup;
			NotNullPtr<UI::GUITextBox> txtGroupId;
			NotNullPtr<UI::GUIComboBox> cboGroupType;
			NotNullPtr<UI::GUITextBox> txtGroupEName;
			NotNullPtr<UI::GUITextBox> txtGroupCName;
			NotNullPtr<UI::GUICheckBox> chkGroupAdmin;
			NotNullPtr<UI::GUITextBox> txtGroupDesc;
			NotNullPtr<UI::GUITextBox> txtGroupKey;
			NotNullPtr<UI::GUIButton> btnGroupRemove;
			NotNullPtr<UI::GUIButton> btnGroupAdd;
			NotNullPtr<UI::GUIButton> btnGroupEnter;

			NotNullPtr<UI::GUITabPage> tpSpecies;
			NotNullPtr<UI::GUIPanel> pnlSpecies;
			NotNullPtr<UI::GUITextBox> txtSpeciesId;
			NotNullPtr<UI::GUITextBox> txtSpeciesCName;
			NotNullPtr<UI::GUITextBox> txtSpeciesSName;
			NotNullPtr<UI::GUITextBox> txtSpeciesEName;
			NotNullPtr<UI::GUITextBox> txtSpeciesDName;
			NotNullPtr<UI::GUITextBox> txtSpeciesDesc;
			NotNullPtr<UI::GUITextBox> txtSpeciesKey;
			NotNullPtr<UI::GUIButton> btnSpeciesRemove;
			NotNullPtr<UI::GUIButton> btnSpeciesAdd;
			NotNullPtr<UI::GUIButton> btnSpeciesPasteSName;
			NotNullPtr<UI::GUIButton> btnSpeciesColor;
			NotNullPtr<UI::GUIGroupBox> grpSpBook;
			NotNullPtr<UI::GUIPanel> pnlSpBook;
			NotNullPtr<UI::GUITextBox> txtSpBook;
			NotNullPtr<UI::GUITextBox> txtSpBookYear;
			NotNullPtr<UI::GUIComboBox> cboSpBook;
			NotNullPtr<UI::GUIButton> btnSpBookAdd;
			NotNullPtr<UI::GUIPanel> pnlSpBookCtrl;
			NotNullPtr<UI::GUIButton> btnSpBookDel;
			NotNullPtr<UI::GUIListView> lvSpBook;

			NotNullPtr<UI::GUITabPage> tpImage;
			UI::GUIPictureBoxDD *pbImg;
			NotNullPtr<UI::GUIVSplitter> vsbImg;
			UI::GUIListBox *lbImage;
			NotNullPtr<UI::GUIPanel> pnlImage;
			NotNullPtr<UI::GUIButton> btnImageSaveAll;
			NotNullPtr<UI::GUIButton> btnImageSave;
			NotNullPtr<UI::GUIButton> btnImagePick;
			NotNullPtr<UI::GUIButton> btnImagePickAdd;
			NotNullPtr<UI::GUIButton> btnImagePickAll;
			NotNullPtr<UI::GUIButton> btnImageDir;
			NotNullPtr<UI::GUIButton> btnImageRotate;
			NotNullPtr<UI::GUIButton> btnImageCrop;
			NotNullPtr<UI::GUIButton> btnImageClipboard;

			NotNullPtr<UI::GUITabPage> tpMap;
			NotNullPtr<UI::GUIPanel> pnlMapCtrl;
			NotNullPtr<UI::GUIPanel> pnlMapStatus;
			UI::GUITrackBar *tbMapScale;
			NotNullPtr<UI::GUITextBox> txtMapPos;
			NotNullPtr<UI::GUITextBox> txtMapScale;
			UI::GUIMapControl *mcMap;

			UI::GUIMainMenu *mnuMain;

			Media::ColorManager *colorMgr;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			OrganEnv *env;
			OrganGroup *rootGroup;
			UOSInt lastDirIndex;
			UOSInt lastObjIndex;
			Bool restoreObj;
			OrganSpecies *lastSpeciesObj;
			OrganGroup *lastGroupObj;
			InputMode inputMode;
			Data::ArrayList<OrganGroupItem*> groupItems;
			Data::ArrayList<OrganGroup*> groupList;
			Data::ArrayList<OrganImageItem*> imgItems;
			Bool indexChanged;
			Media::StaticImage *lastBmp;
			Bool newDirName;
			Data::ArrayList<OrganGroupItem*> pickObjs;

			Media::ImageList *dispImage;
			UserFileInfo *dispImageUF;
			WebFileInfo *dispImageWF;
			Bool dispImageToCrop;
			Bool dispImageDown;
			Math::Coord2D<OSInt> dispImageDownPos;
			Math::Coord2D<OSInt> dispImageCurrPos;

			Map::DrawMapRenderer *mapRenderer;
			NotNullPtr<Map::MapView> mapView;
			NotNullPtr<Map::MapEnv> mapEnv;
			NotNullPtr<Map::TileMapLayer> mapTileLyr;
			NotNullPtr<Map::TileMap> mapTile;
			Bool mapUpdated;
			Data::FastMap<UInt32, OrganSpImgLayer*> mapImgLyrs;
			UOSInt imgFontStyle;

			Text::String *initSelObj;
			Text::String *initSelImg;

			Data::ArrayList<UserFileInfo*> mapUFiles;
			UserFileInfo *mapCurrFile;
			Media::DrawImage *mapCurrImage;
			Media::Resizer::LanczosResizer8_C8 *mapResizer;

			UInt32 unkCnt;
//			UInt32 dragEff;

		private:
			static void __stdcall OnGroupEnterClick(void *userObj);
			static void __stdcall OnGroupAddClicked(void *userObj);
			static void __stdcall OnGroupRemoveClicked(void *userObj);
			static void __stdcall OnDirChanged(void *userObj);
			static void __stdcall OnObjDblClicked(void *userObj);
			static void __stdcall OnObjSelChg(void *userObj);
			static void __stdcall OnImgSelChg(void *userObj);
			static Bool __stdcall OnImgRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool __stdcall OnImgMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool __stdcall OnImgMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool __stdcall OnImgMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnImgDraw(void *userObj, UInt8 *imgPtr, UOSInt w, UOSInt h, UOSInt bpl);
			static void __stdcall OnImgDblClicked(void *userObj);
			static void __stdcall OnImgDirClicked(void *userObj);
			static void __stdcall OnImageRotateClicked(void *userObj);
			static void __stdcall OnImageCropClicked(void *userObj);
			static void __stdcall OnImageSaveClicked(void *userObj);
			static void __stdcall OnImageSaveAllClicked(void *userObj);
			static void __stdcall OnImageClipboardClicked(void *userObj);
			static void __stdcall OnSpAddClicked(void *userObj);
			static void __stdcall OnSpRemoveClicked(void *userObj);
			static void __stdcall OnSpPasteSNameClicked(void *userObj);
			static void __stdcall OnSpBookYearChg(void *userObj);
			static void __stdcall OnSpBookAddClicked(void *userObj);
			static void __stdcall OnSpBookSelChg(void *userObj);
			static void __stdcall OnSpeciesSNameChg(void *userObj);
			static void __stdcall OnSpeciesColorClicked(void *userObj);
			static void __stdcall OnTabSelChg(void *userObj);
			static void __stdcall OnImagePickClicked(void *userObj);
			static void __stdcall OnImagePickAddClicked(void *userObj);
			static void __stdcall OnImagePickAllClicked(void *userObj);
			static void __stdcall OnObjPickClicked(void *userObj);
			static void __stdcall OnObjPlaceClicked(void *userObj);
			static void __stdcall OnObjCombineClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnMapScaleScroll(void *userObj, UOSInt newVal);
			static void __stdcall OnTileUpdated(void *userObj);
			static void __stdcall OnMapMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnMapDraw(void *userObj, NotNullPtr<Media::DrawImage> dimg, OSInt xOfst, OSInt yOfst);

			static OSInt __stdcall GroupCompare(OrganGroupItem *group1, OrganGroupItem *group2);

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
			void SelectGroup(NotNullPtr<UI::GUIComboBox> cbo, Int32 groupType);
			void GoToDir(OrganGroup *grp, Int32 parentId);

			void ClearImgLayers();
			NotNullPtr<OrganSpImgLayer> GetImgLayer(UInt32 mapColor);
		public:
			OrganMainForm(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, OrganEnv *env);
			virtual ~OrganMainForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual DragEffect DragEnter(UI::GUIDropData *data);
			virtual void DropData(UI::GUIDropData *data, OSInt x, OSInt y);

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