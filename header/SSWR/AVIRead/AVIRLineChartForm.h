#ifndef _SM_SSWR_AVIREAD_AVIRLINECHARTFORM
#define _SM_SSWR_AVIREAD_AVIRLINECHARTFORM
#include "Data/Chart.h"
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
				UOSInt colIndex;
				NotNullPtr<DB::ColDef> colDef;
				void *datas;
			} ColInfo;
		private:
			NotNullPtr<UI::GUIPanel> pnlStrCols;
			NotNullPtr<UI::GUILabel> lblStrCols;
			NotNullPtr<UI::GUIListBox> lbStrCols;
			NotNullPtr<UI::GUIButton> btnStrColsDbl;
			NotNullPtr<UI::GUIButton> btnStrColsInt32;
			NotNullPtr<UI::GUIPanel> pnlXAxis;
			NotNullPtr<UI::GUILabel> lblXAxis;
			NotNullPtr<UI::GUIComboBox> cboXAxis;
			NotNullPtr<UI::GUIGroupBox> grpYAxis;
			NotNullPtr<UI::GUIPanel> pnlYAxis;
			NotNullPtr<UI::GUILabel> lblYAxis;
			NotNullPtr<UI::GUIComboBox> cboYAxis;
			NotNullPtr<UI::GUIButton> btnYAxis;
			NotNullPtr<UI::GUIListBox> lbYAxis;
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnPlot;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			DB::ReadingDB *db;
			Data::Chart *chart;
			Data::ArrayList<UInt32> *yCols;
			NotNullPtr<Text::String> tableName;
			Optional<Text::String> schemaName;
			DB::DBUtil::ColType *strTypes;

			static void __stdcall OnPlotClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnYAxisClicked(void *userObj);
			static void __stdcall OnStrColsDblClicked(void *userObj);
			static void __stdcall OnStrColsInt32Clicked(void *userObj);
		public:
			AVIRLineChartForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schemaName, Text::CString tableName);
			virtual ~AVIRLineChartForm();

			virtual void OnMonitorChanged();

			Data::Chart *GetChart();
		};
	}
}
#endif
