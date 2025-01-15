#ifndef _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#define _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#include "Data/ArrayListNN.h"
#include "Map/VectorLayer.h"
#include "Math/VectorTextWriterList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "UI/GUIButton.h"
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
			NN<UI::GUIComboBox> cboObj;
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

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IMapNavigator> navi;
			NN<Map::MapDrawLayer> lyr;
			Math::Coord2D<OSInt> downPos;
			Optional<Math::Geometry::Vector2D> currVec;
			Math::VectorTextWriterList writerList;
			Bool layerNames;

			Data::ArrayListNN<Math::Geometry::Vector2D> queryVecList;
			Data::ArrayListNN<Math::Geometry::Vector2D> queryVecOriList;
			Data::ArrayList<UOSInt> queryValueOfstList;
			Data::ArrayListStringNN queryNameList;
			Data::ArrayListNN<Text::String> queryValueList;

			static Bool __stdcall OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnShapeFmtChanged(AnyType userObj);
			static void __stdcall OnObjSelChg(AnyType userObj);

			void ShowLayerNames();
			void ClearQueryResults();
			void SetQueryItem(UOSInt index);
		public:
			AVIRGISQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapDrawLayer> lyr, NN<IMapNavigator> navi);
			virtual ~AVIRGISQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
