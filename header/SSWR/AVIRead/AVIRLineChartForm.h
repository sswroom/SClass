#ifndef _SM_SSWR_AVIREAD_AVIRLINECHARTFORM
#define _SM_SSWR_AVIREAD_AVIRLINECHARTFORM
#include "Data/IChart.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLineChartForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				OSInt colIndex;
				DB::ColDef *colDef;
				void *datas;
			} ColInfo;
		private:
			UI::GUIPanel *pnlStrCols;
			UI::GUILabel *lblStrCols;
			UI::GUIListBox *lbStrCols;
			UI::GUIButton *btnStrColsDbl;
			UI::GUIButton *btnStrColsInt32;
			UI::GUIPanel *pnlXAxis;
			UI::GUILabel *lblXAxis;
			UI::GUIComboBox *cboXAxis;
			UI::GUIGroupBox *grpYAxis;
			UI::GUIPanel *pnlYAxis;
			UI::GUILabel *lblYAxis;
			UI::GUIComboBox *cboYAxis;
			UI::GUIButton *btnYAxis;
			UI::GUIListBox *lbYAxis;
			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnPlot;
			UI::GUIButton *btnCancel;

			SSWR::AVIRead::AVIRCore *core;
			DB::ReadingDB *db;
			Data::IChart *chart;
			Data::ArrayList<Int32> *yCols;
			const UTF8Char *tableName;
			DB::DBUtil::ColType *strTypes;

			static void __stdcall OnPlotClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnYAxisClicked(void *userObj);
			static void __stdcall OnStrColsDblClicked(void *userObj);
			static void __stdcall OnStrColsInt32Clicked(void *userObj);
		public:
			AVIRLineChartForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, const UTF8Char *tableName);
			virtual ~AVIRLineChartForm();

			virtual void OnMonitorChanged();

			Data::IChart *GetChart();
		};
	}
}
#endif
