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
			NotNullPtr<UI::GUIPanel> pnlObj;
			NotNullPtr<UI::GUIComboBox> cboObj;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			UI::GUIListView *lvInfo;

			NotNullPtr<UI::GUITabPage> tpShape;
			NotNullPtr<UI::GUIPanel> pnlShape;
			NotNullPtr<UI::GUILabel> lblShapeFmt;
			NotNullPtr<UI::GUIComboBox> cboShapeFmt;
			UI::GUITextBox *txtShape;

			NotNullPtr<UI::GUITabPage> tpBounds;
			NotNullPtr<UI::GUILabel> lblMinX;
			UI::GUITextBox *txtMinX;
			NotNullPtr<UI::GUILabel> lblMinY;
			UI::GUITextBox *txtMinY;
			NotNullPtr<UI::GUILabel> lblMaxX;
			UI::GUITextBox *txtMaxX;
			NotNullPtr<UI::GUILabel> lblMaxY;
			UI::GUITextBox *txtMaxY;

			NotNullPtr<UI::GUITabPage> tpDist;
			NotNullPtr<UI::GUILabel> lblDist;
			UI::GUITextBox *txtDist;
			NotNullPtr<UI::GUILabel> lblInside;
			UI::GUITextBox *txtInside;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IMapNavigator *navi;
			NotNullPtr<Map::MapDrawLayer> lyr;
			Math::Coord2D<OSInt> downPos;
			Math::Geometry::Vector2D *currVec;
			Math::VectorTextWriterList writerList;
			Bool layerNames;

			Data::ArrayList<Math::Geometry::Vector2D*> queryVecList;
			Data::ArrayList<UOSInt> queryValueOfstList;
			Data::ArrayListStringNN queryNameList;
			Data::ArrayList<Text::String*> queryValueList;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnShapeFmtChanged(void *userObj);
			static void __stdcall OnObjSelChg(void *userObj);

			void ShowLayerNames();
			void ClearQueryResults();
			void SetQueryItem(UOSInt index);
		public:
			AVIRGISQueryForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapDrawLayer> lyr, IMapNavigator *navi);
			virtual ~AVIRGISQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
