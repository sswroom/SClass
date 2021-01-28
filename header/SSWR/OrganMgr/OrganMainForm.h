#ifndef _SM_SSWR_ORGANMGR_ORGANMAINFORM
#define _SM_SSWR_ORGANMGR_ORGANMAINFORM

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
			UI::GUIPanel *pnlLeft;
			UI::GUILabel *lblLeft;
			UI::GUIListBox *lbDir;
			UI::GUIHSplitter *hsbLeft;

			UI::GUIPanel *pnlMid;
			UI::GUILabel *lblMid;
			UI::GUIListBox *lbObj;
			UI::GUIPanel *pnlMidBottom;
			UI::GUILabel *lblPickMsg;
			UI::GUIButton *btnObjPick;
			UI::GUIButton *btnObjPlace;
			UI::GUIButton *btnObjCombine;
			UI::GUIHSplitter *hsbMid;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpGroup;
			UI::GUITextBox *txtGroupId;
			UI::GUIComboBox *cboGroupType;
			UI::GUITextBox *txtGroupEName;
			UI::GUITextBox *txtGroupCName;
			UI::GUICheckBox *chkGroupAdmin;
			UI::GUITextBox *txtGroupDesc;
			UI::GUITextBox *txtGroupKey;
			UI::GUIButton *btnGroupRemove;
			UI::GUIButton *btnGroupAdd;
			UI::GUIButton *btnGroupEnter;

			UI::GUITabPage *tpSpecies;
			UI::GUIPanel *pnlSpecies;
			UI::GUITextBox *txtSpeciesId;
			UI::GUITextBox *txtSpeciesCName;
			UI::GUITextBox *txtSpeciesSName;
			UI::GUITextBox *txtSpeciesEName;
			UI::GUITextBox *txtSpeciesDName;
			UI::GUITextBox *txtSpeciesDesc;
			UI::GUITextBox *txtSpeciesKey;
			UI::GUIButton *btnSpeciesRemove;
			UI::GUIButton *btnSpeciesAdd;
			UI::GUIButton *btnSpeciesPasteSName;
			UI::GUIButton *btnSpeciesColor;
			UI::GUIGroupBox *grpSpBook;
			UI::GUIPanel *pnlSpBook;
			UI::GUITextBox *txtSpBook;
			UI::GUITextBox *txtSpBookYear;
			UI::GUIComboBox *cboSpBook;
			UI::GUIButton *btnSpBookAdd;
			UI::GUIPanel *pnlSpBookCtrl;
			UI::GUIButton *btnSpBookDel;
			UI::GUIListView *lvSpBook;

			UI::GUITabPage *tpImage;
			UI::GUIPictureBoxDD *pbImg;
			UI::GUIVSplitter *vsbImg;
			UI::GUIListBox *lbImage;
			UI::GUIPanel *pnlImage;
			UI::GUIButton *btnImageSaveAll;
			UI::GUIButton *btnImageSave;
			UI::GUIButton *btnImagePick;
			UI::GUIButton *btnImagePickAdd;
			UI::GUIButton *btnImagePickAll;
			UI::GUIButton *btnImageDir;
			UI::GUIButton *btnImageRotate;
			UI::GUIButton *btnImageCrop;
			UI::GUIButton *btnImageClipboard;

			UI::GUITabPage *tpMap;
			UI::GUIPanel *pnlMapCtrl;
			UI::GUIPanel *pnlMapStatus;
			UI::GUITrackBar *tbMapScale;
			UI::GUITextBox *txtMapPos;
			UI::GUITextBox *txtMapScale;
			UI::GUIMapControl *mcMap;

			UI::GUIMainMenu *mnuMain;

			Media::ColorManager *colorMgr;
			Media::ColorManagerSess *colorSess;
			OrganEnv *env;
			OrganGroup *rootGroup;
			OSInt lastDirIndex;
			OSInt lastObjIndex;
			Bool restoreObj;
			OrganSpecies *lastSpeciesObj;
			OrganGroup *lastGroupObj;
			InputMode inputMode;
			Data::ArrayList<OrganGroupItem*> *groupItems;
			Data::ArrayList<OrganGroup*> *groupList;
			Data::ArrayList<OrganImageItem*> *imgItems;
			Bool indexChanged;
			Media::StaticImage *lastBmp;
			Bool newDirName;
			Data::ArrayList<OrganGroupItem*> *pickObjs;

			Media::ImageList *dispImage;
			UserFileInfo *dispImageUF;
			WebFileInfo *dispImageWF;
			Bool dispImageToCrop;
			Bool dispImageDown;
			OSInt dispImageDownX;
			OSInt dispImageDownY;
			OSInt dispImageCurrX;
			OSInt dispImageCurrY;

			Map::DrawMapRenderer *mapRenderer;
			Map::MapView *mapView;
			Map::MapEnv *mapEnv;
			Map::TileMapLayer *mapTileLyr;
			Map::TileMap *mapTile;
			Bool mapUpdated;
			Data::Integer32Map<OrganSpImgLayer*> *mapImgLyrs;
			OSInt imgFontStyle;

			const UTF8Char *initSelObj;
			const UTF8Char *initSelImg;

			Data::ArrayList<UserFileInfo*> *mapUFiles;
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
			static Bool __stdcall OnImgRClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static Bool __stdcall OnImgMouseDown(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static Bool __stdcall OnImgMouseUp(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static Bool __stdcall OnImgMouseMove(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static void __stdcall OnImgDraw(void *userObj, UInt8 *imgPtr, OSInt w, OSInt h, OSInt bpl);
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
			static void __stdcall OnMapScaleScroll(void *userObj, Int32 newVal);
			static void __stdcall OnTileUpdated(void *userObj);
			static void __stdcall OnMapMouseMove(void *userObj, OSInt x, OSInt y);
			static void __stdcall OnMapDraw(void *userObj, Media::DrawImage *dimg, OSInt xOfst, OSInt yOfst);

			static OSInt __stdcall GroupCompare(void *obj1, void *obj2);

			Bool CalcCropRect(OSInt *rect);

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
			void SelectGroup(UI::GUIComboBox *cbo, Int32 groupType);
			void GoToDir(OrganGroup *grp, Int32 parentId);

			void ClearImgLayers();
			OrganSpImgLayer *GetImgLayer(Int32 mapColor);
		public:
			OrganMainForm(UI::GUICore *ui, UI::GUIClientControl *parent, OrganEnv *env);
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