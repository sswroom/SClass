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
				NN<DB::ColDef> colDef;
				void *datas;
			} ColInfo;
		private:
			NN<UI::GUIPanel> pnlStrCols;
			NN<UI::GUILabel> lblStrCols;
			NN<UI::GUIListBox> lbStrCols;
			NN<UI::GUIButton> btnStrColsDbl;
			NN<UI::GUIButton> btnStrColsInt32;
			NN<UI::GUIPanel> pnlXAxis;
			NN<UI::GUILabel> lblXAxis;
			NN<UI::GUIComboBox> cboXAxis;
			NN<UI::GUIGroupBox> grpYAxis;
			NN<UI::GUIPanel> pnlYAxis;
			NN<UI::GUILabel> lblYAxis;
			NN<UI::GUIComboBox> cboYAxis;
			NN<UI::GUIButton> btnYAxis;
			NN<UI::GUIListBox> lbYAxis;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnPlot;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<DB::ReadingDB> db;
			Optional<Data::Chart> chart;
			Data::ArrayList<UInt32> yCols;
			NN<Text::String> tableName;
			Optional<Text::String> schemaName;
			UnsafeArrayOpt<DB::DBUtil::ColType> strTypes;

			static void __stdcall OnPlotClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnYAxisClicked(AnyType userObj);
			static void __stdcall OnStrColsDblClicked(AnyType userObj);
			static void __stdcall OnStrColsInt32Clicked(AnyType userObj);
		public:
			AVIRLineChartForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName);
			virtual ~AVIRLineChartForm();

			virtual void OnMonitorChanged();

			Optional<Data::Chart> GetChart();
		};
	}
}
#endif
