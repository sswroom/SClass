#ifndef _SM_SSWR_AVIREAD_AVIRGISFORM
#define _SM_SSWR_AVIREAD_AVIRGISFORM
#include "Map/MapLayerCollection.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIMainMenu.h"
#include "UI/GUIMapControl.h"
#include "UI/GUIMapTreeView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITextBox.h"
#include "UI/GUITrackBar.h"


namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISForm : public UI::GUIForm, public SSWR::AVIRead::IMapNavigator, public Media::IPrintHandler
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			UI::GUIForm *ctrlForm;
			UI::GUITreeView::TreeItem *ctrlItem;
			Media::ColorManagerSess *colorSess;
			Data::ArrayList<UI::GUIForm *> *subForms;
			UI::GUIMapControl *mapCtrl;
			UI::GUIMapTreeView *mapTree;
			UI::GUIHSplitter *splitter;

			UI::GUIPanel *pnlControl;
			UI::GUIPanel *pnlStatus;
			UI::GUITextBox *txtScale;
			UI::GUITextBox *txtLatLon;
			UI::GUITextBox *txtUTMGrid;
			UI::GUITextBox *txtTimeUsed;
			UI::GUITrackBar *tbScale;
			UI::GUIMainMenu *mnuMain;
			UI::GUIPopupMenu *mnuLayer;
			UI::GUIPopupMenu *mnuGroup;
			UI::GUITrackBar *tbTimeRange;
			UI::GUICheckBox *chkTime;
			Map::MapEnv *env;
			Map::DrawMapRenderer *envRenderer;
			Bool scaleChanging;
			UI::GUITreeView::TreeItem *popNode;
			Text::TextAnalyzer *ta;
			Math::GeographicCoordinateSystem *wgs84CSys;

			Bool useTime;
			Int64 currTime;
			Int64 timeRangeStart;
			Int64 timeRangeEnd;
			Bool mapUpdTChanged;
			Bool mapLyrUpdated;
			Double mapUpdT;
			Bool pauseUpdate;

			Data::ArrayList<MouseEvent> *mouseDownHdlrs;
			Data::ArrayList<void*> *mouseDownObjs;
			Data::ArrayList<MouseEvent> *mouseUpHdlrs;
			Data::ArrayList<void *> *mouseUpObjs;
			Data::ArrayList<MouseEvent> *mouseMoveHdlrs;
			Data::ArrayList<void *> *mouseMoveObjs;

			Media::Printer *printer;

			Int32 lcid;
			CursorType currCursor;

		private:
			static void __stdcall FileHandler(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnMapMouseMove(void *userObj, OSInt x, OSInt y);
			static Bool __stdcall OnMapMouseDown(void *userObj, OSInt x, OSInt y, MouseButton button);
			static Bool __stdcall OnMapMouseUp(void *userObj, OSInt x, OSInt y, MouseButton button);
			static void __stdcall OnMapScaleChanged(void *userObj, Double newScale);
			static void __stdcall OnMapUpdated(void *userObj, Double centerX, Double centerY, Double timeUsed);
			static void __stdcall OnScaleScrolled(void *userObj, UOSInt newVal);
			static void __stdcall OnTreeRightClick(void *userObj);
			static void __stdcall OnCtrlFormClosed(void *userObj, UI::GUIForm *frm);
			static void __stdcall OnSubFormClosed(void *userObj, UI::GUIForm *frm);
			static void __stdcall OnMapLayerUpdated(void *userObj);
			static void __stdcall OnTimeScrolled(void *userObj, UOSInt newVal);
			static void __stdcall OnTimeChecked(void *userObj, Bool newState);
			static void __stdcall OnTreeDrag(void *userObj, UI::GUIMapTreeView::ItemIndex *dragItem, UI::GUIMapTreeView::ItemIndex *dropItem);
			static void __stdcall OnTimerTick(void *userObj);
			void UpdateTitle();
			void CloseCtrlForm(Bool closing);
			void SetCtrlForm(UI::GUIForm *frm, UI::GUITreeView::TreeItem *item);
			Bool ParseObject(IO::ParsedObject *pobj);
			void OpenURL(const UTF8Char *url, UOSInt urlLen, const UTF8Char *customName);
			void HKOPortal(const UTF8Char *listFile, const UTF8Char *filePath);
		public:
			AVIRGISForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Map::MapView *view);
			virtual ~AVIRGISForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
			virtual void OnFocus();

			void AddLayer(Map::IMapDrawLayer *layer);
			void AddLayers(::Data::ArrayList<Map::IMapDrawLayer*> *layers);
//			void AddLayerColl(Map::MapLayerCollection *lyrColl);
			void AddSubForm(UI::GUIForm *frm);

			virtual UInt32 GetSRID();
			virtual Bool InMap(Double lat, Double lon);
			virtual void PanToMap(Double lat, Double lon);
			virtual void ShowMarker(Double lat, Double lon);
			virtual void ShowMarkerDir(Double lat, Double lon, Double dir, Math::Unit::Angle::AngleUnit unit);
			virtual void HideMarker();
			virtual void SetSelectedVector(Math::Vector2D *vec);
			virtual void RedrawMap();
			virtual Math::CoordinateSystem *GetCoordinateSystem();

			virtual void ScnXY2MapXY(OSInt scnX, OSInt scnY, Double *mapX, Double *mapY);
			virtual void MapXY2ScnXY(Double mapX, Double mapY, OSInt *scnX, OSInt *scnY);
			virtual void SetMapCursor(UI::GUIControl::CursorType curType);
			virtual void HandleMapMouseDown(MouseEvent evt, void *userObj);
			virtual void HandleMapMouseUp(MouseEvent evt, void *userObj);
			virtual void HandleMapMouseMove(MouseEvent evt, void *userObj);
			virtual void UnhandleMapMouse(void *userObj);

			virtual void SetKMapEnv(const UTF8Char *kmapIP, Int32 kmapPort, Int32 lcid);
			virtual Bool HasKMap();
			virtual UTF8Char *ResolveAddress(UTF8Char *sbuff, Double lat, Double lon);

			virtual void PauseUpdate();
			virtual void ResumeUpdate();

			void UpdateTimeRange();

			virtual Bool BeginPrint(Media::IPrintDocument *doc);
			virtual Bool PrintPage(Media::DrawImage *printPage);
			virtual Bool EndPrint(Media::IPrintDocument *doc);
		};
	};
};
#endif
