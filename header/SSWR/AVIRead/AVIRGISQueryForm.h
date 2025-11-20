#ifndef _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#define _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#include "Data/ArrayListNN.hpp"
#include "Map/VectorLayer.h"
#include "Math/VectorTextWriterList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMapNavigator.h"
#include "Sync/Thread.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISQueryForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlObj;
			NN<UI::GUILabel> lblObjName;
			NN<UI::GUIComboBox> cboObjName;
			NN<UI::GUIButton> btnColDownload;
			NN<UI::GUIComboBox> cboObj;
			NN<UI::GUILabel> lblObjMsg;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIListView> lvInfo;

			NN<UI::GUITabPage> tpShape;
			NN<UI::GUIPanel> pnlShape;
			NN<UI::GUILabel> lblShapeHLength;
			NN<UI::GUITextBox> txtShapeHLength;
			NN<UI::GUILabel> lblShapeLength;
			NN<UI::GUITextBox> txtShapeLength;
			NN<UI::GUILabel> lblShapeArea;
			NN<UI::GUITextBox> txtShapeArea;
			NN<UI::GUILabel> lblShapeFmt;
			NN<UI::GUIComboBox> cboShapeFmt;
			NN<UI::GUITextBox> txtShape;

			NN<UI::GUITabPage> tpBounds;
			NN<UI::GUILabel> lblMinX;
			NN<UI::GUITextBox> txtMinX;
			NN<UI::GUILabel> lblMinY;
			NN<UI::GUITextBox> txtMinY;
			NN<UI::GUILabel> lblMaxX;
			NN<UI::GUITextBox> txtMaxX;
			NN<UI::GUILabel> lblMaxY;
			NN<UI::GUITextBox> txtMaxY;

			NN<UI::GUITabPage> tpDist;
			NN<UI::GUILabel> lblDist;
			NN<UI::GUITextBox> txtDist;
			NN<UI::GUILabel> lblInside;
			NN<UI::GUITextBox> txtInside;

			NN<UI::GUITabPage> tpAutoSave;
			NN<UI::GUILabel> lblAutoSaveAction;
			NN<UI::GUIComboBox> cboAutoSaveAction;
			NN<UI::GUILabel> lblAutoSavePath;
			NN<UI::GUITextBox> txtAutoSavePath;
			NN<UI::GUIButton> btnObjDownload;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<AVIRMapNavigator> navi;
			NN<Map::MapDrawLayer> lyr;
			Math::Coord2D<OSInt> downPos;
			Bool rdown;
			Math::Coord2D<OSInt> rdownPos;
			Optional<Math::Geometry::Vector2D> currVec;
			Math::VectorTextWriterList writerList;
			Bool layerNames;
			UOSInt nameCol;
			Sync::Mutex downMut;
			Data::ArrayListStringNN downList;
			Sync::Mutex openMut;
			Data::ArrayListStringNN openList;
			Sync::Thread downThread;
			Optional<Text::String> downPath;
			UOSInt dispCnt;

			Data::ArrayListNN<Math::Geometry::Vector2D> queryVecList;
			Data::ArrayListNN<Math::Geometry::Vector2D> queryVecOriList;
			Data::ArrayList<UOSInt> queryValueOfstList;
			Data::ArrayListStringNN queryNameList;
			Data::ArrayListNN<Text::String> queryValueList;

			static Bool __stdcall OnMouseLDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseLUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseRDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseRUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnShapeFmtChanged(AnyType userObj);
			static void __stdcall OnObjSelChg(AnyType userObj);
			static void __stdcall OnObjNameSelChg(AnyType userObj);
			static void __stdcall OnInfoDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnObjDownloadClicked(AnyType userObj);
			static void __stdcall OnColDownloadClicked(AnyType userObj);
			static void __stdcall OnOpenTimer(AnyType userObj);
			static void __stdcall DownThread(NN<Sync::Thread> thread);
			static void __stdcall OnFormClosed(AnyType userObj, NN<UI::GUIForm> frm);

			void ShowLayerNames();
			void ClearQueryResults();
			void SetQueryItem(UOSInt index);
			void DownloadURL(NN<Text::String> url);
		public:
			AVIRGISQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapDrawLayer> lyr, NN<AVIRMapNavigator> navi);
			virtual ~AVIRGISQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
