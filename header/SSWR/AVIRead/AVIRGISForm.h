#ifndef _SM_SSWR_AVIREAD_AVIRGISFORM
#define _SM_SSWR_AVIREAD_AVIRGISFORM
#include "Map/MapLayerCollection.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
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
		class AVIRGISForm : public UI::GUIForm, public SSWR::AVIRead::AVIRMapNavigator, public Media::PrintHandler
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<UI::GUIForm> ctrlForm;
			Optional<UI::GUITreeView::TreeItem> ctrlItem;
			NN<Media::ColorManagerSess> colorSess;
			Data::ArrayListNN<UI::GUIForm> subForms;
			NN<UI::GUIMapControl> mapCtrl;
			NN<UI::GUIMapTreeView> mapTree;
			NN<UI::GUIHSplitter> splitter;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIPanel> pnlStatus;
			NN<UI::GUITextBox> txtScale;
			NN<UI::GUITextBox> txtLatLon;
			NN<UI::GUITextBox> txtUTMGrid;
			NN<UI::GUITextBox> txtTimeUsed;
			NN<UI::GUITrackBar> tbScale;
			NN<UI::GUIMainMenu> mnuMain;
			NN<UI::GUIPopupMenu> mnuLayer;
			NN<UI::GUIPopupMenu> mnuGroup;
			NN<UI::GUITrackBar> tbTimeRange;
			NN<UI::GUICheckBox> chkTime;
			NN<UI::GUILabel> lblVAngle;
			NN<UI::GUITrackBar> tbVAngle;
			NN<UI::GUILabel> lblHAngle;
			NN<UI::GUITrackBar> tbHAngle;
			NN<Map::MapEnv> env;
			NN<Map::DrawMapRenderer> envRenderer;
			Bool scaleChanging;
			Optional<UI::GUITreeView::TreeItem> popNode;
			Text::TextAnalyzer ta;
			NN<Math::GeographicCoordinateSystem> wgs84CSys;

			Bool useTime;
			Int64 currTime;
			Int64 timeRangeStart;
			Int64 timeRangeEnd;
			Bool mapUpdTChanged;
			Bool mapLyrUpdated;
			Double mapUpdT;
			Bool pauseUpdate;

			Data::ArrayList<Data::CallbackStorage<MouseEvent>> mouseLDownHdlrs;
			Data::ArrayList<Data::CallbackStorage<MouseEvent>> mouseLUpHdlrs;
			Data::ArrayList<Data::CallbackStorage<MouseEvent>> mouseRDownHdlrs;
			Data::ArrayList<Data::CallbackStorage<MouseEvent>> mouseRUpHdlrs;
			Data::ArrayList<Data::CallbackStorage<MouseEvent>> mouseMoveHdlrs;

			Optional<Media::Printer> printer;

			CursorType currCursor;

		private:
			static void __stdcall FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnMapMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMapMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button);
			static Bool __stdcall OnMapMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button);
			static void __stdcall OnMapScaleChanged(AnyType userObj, Double newScale);
			static void __stdcall OnMapUpdated(AnyType userObj, Math::Coord2DDbl center, Double timeUsed);
			static void __stdcall OnScaleScrolled(AnyType userObj, UOSInt newVal);
			static void __stdcall OnTreeRightClick(AnyType userObj);
			static void __stdcall OnCtrlFormClosed(AnyType userObj, NN<UI::GUIForm> frm);
			static void __stdcall OnSubFormClosed(AnyType userObj, NN<UI::GUIForm> frm);
			static void __stdcall OnMapLayerUpdated(AnyType userObj);
			static void __stdcall OnTimeScrolled(AnyType userObj, UOSInt newVal);
			static void __stdcall OnTimeChecked(AnyType userObj, Bool newState);
			static void __stdcall OnTreeDrag(AnyType userObj, NN<UI::GUIMapTreeView::ItemIndex> dragItem, NN<UI::GUIMapTreeView::ItemIndex> dropItem);
			static void __stdcall OnVAngleScrolled(AnyType userObj, UOSInt newVal);
			static void __stdcall OnHAngleScrolled(AnyType userObj, UOSInt newVal);
			static void __stdcall OnTimerTick(AnyType userObj);
			void UpdateTitle();
			void CloseCtrlForm(Bool closing);
			void SetCtrlForm(NN<UI::GUIForm> frm, Optional<UI::GUITreeView::TreeItem> item);
			Bool ParseObject(NN<IO::ParsedObject> pobj);
			void OpenURL(Text::CStringNN url, Text::CString customName);
			void HKOPortal(Text::CStringNN listFile, Text::CStringNN filePath);
			void OpenCSV(Text::CStringNN url, UInt32 codePage, Text::CStringNN name, Text::CStringNN nameCol, Text::CStringNN latCol, Text::CStringNN lonCol);
		public:
			AVIRGISForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Map::MapView> view);
			virtual ~AVIRGISForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
			virtual void OnFocus();

			virtual void AddLayerFromFile(Text::CStringNN fileName);
			virtual void AddLayer(NN<Map::MapDrawLayer> layer);
			void AddLayers(NN<::Data::ArrayListNN<Map::MapDrawLayer>> layers);
//			void AddLayerColl(NN<Map::MapLayerCollection> lyrColl);
			void AddSubForm(NN<UI::GUIForm> frm);

			virtual UInt32 GetSRID();
			virtual Bool InMap(Math::Coord2DDbl pos);
			virtual void PanToMap(Math::Coord2DDbl pos);
			virtual void ShowMarker(Math::Coord2DDbl pos);
			virtual void ShowMarkerDir(Math::Coord2DDbl pos, Double dir, Math::Unit::Angle::AngleUnit unit);
			virtual void HideMarker();
			virtual void SetSelectedVector(Optional<Math::Geometry::Vector2D> vec);
			virtual void SetSelectedVectors(NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList);
			virtual void RedrawMap();
			virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;

			virtual Math::Coord2DDbl ScnXY2MapXY(Math::Coord2D<OSInt> scnPos);
			virtual Math::Coord2D<OSInt> MapXY2ScnXY(Math::Coord2DDbl mapPos);
			virtual void SetMapCursor(UI::GUIControl::CursorType curType);
			virtual void HandleMapMouseLDown(MouseEvent evt, AnyType userObj);
			virtual void HandleMapMouseLUp(MouseEvent evt, AnyType userObj);
			virtual void HandleMapMouseRDown(MouseEvent evt, AnyType userObj);
			virtual void HandleMapMouseRUp(MouseEvent evt, AnyType userObj);
			virtual void HandleMapMouseMove(MouseEvent evt, AnyType userObj);
			virtual void UnhandleMapMouse(AnyType userObj);

			virtual void SetKMapEnv(UnsafeArray<const UTF8Char> kmapIP, Int32 kmapPort, Int32 lcid);
			virtual Bool HasKMap();
			virtual UnsafeArrayOpt<UTF8Char> ResolveAddress(UnsafeArray<UTF8Char> sbuff, Math::Coord2DDbl pos);

			virtual void PauseUpdate();
			virtual void ResumeUpdate();

			void UpdateTimeRange();

			virtual Bool BeginPrint(NN<Media::PrintDocument> doc);
			virtual Bool PrintPage(NN<Media::DrawImage> printPage);
			virtual Bool EndPrint(NN<Media::PrintDocument> doc);
		};
	}
}
#endif
