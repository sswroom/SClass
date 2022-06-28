#ifndef _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
#define _SM_SSWR_AVIREAD_AVIRGISQUERYFORM
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
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpInfo;
			UI::GUIListView *lvInfo;

			UI::GUITabPage *tpShape;
			UI::GUIPanel *pnlShape;
			UI::GUILabel *lblShapeFmt;
			UI::GUIComboBox *cboShapeFmt;
			UI::GUITextBox *txtShape;

			UI::GUITabPage *tpBounds;
			UI::GUILabel *lblMinX;
			UI::GUITextBox *txtMinX;
			UI::GUILabel *lblMinY;
			UI::GUITextBox *txtMinY;
			UI::GUILabel *lblMaxX;
			UI::GUITextBox *txtMaxX;
			UI::GUILabel *lblMaxY;
			UI::GUITextBox *txtMaxY;

			SSWR::AVIRead::AVIRCore *core;
			IMapNavigator *navi;
			Map::IMapDrawLayer *lyr;
			Math::Coord2D<OSInt> downPos;
			Math::Vector2D *currVec;
			Math::VectorTextWriterList writerList;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static void __stdcall OnShapeFmtChanged(void *userObj);
		public:
			AVIRGISQueryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::IMapDrawLayer *lyr, IMapNavigator *navi);
			virtual ~AVIRGISQueryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
