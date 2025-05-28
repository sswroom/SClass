#ifndef _SM_SSWR_AVIREAD_AVIRDBASSIGNCOLUMNFORM
#define _SM_SSWR_AVIREAD_AVIRDBASSIGNCOLUMNFORM
#include "DB/TableDef.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBAssignColumnForm : public UI::GUIForm
		{
		private:
			struct ColumnItem
			{
				NN<UI::GUIComboBox> cbo;
				NN<UI::GUITextBox> txt;
				NN<AVIRDBAssignColumnForm> me;
			};
		private:
			NN<UI::GUIPanel> pnlColumns;
			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			UnsafeArrayOpt<ColumnItem> colsItem;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::ArrayList<UOSInt>> colInd;
			NN<Data::ArrayListStringNN> colStr;
			NN<DB::TableDef> dbTable;
			NN<DB::ReadingDB> dataFile;
			Text::CString schema;
			Text::CStringNN table;
			Bool dataFileNoHeader;
			Int8 dataFileTz;

			static void __stdcall OnCboSelChg(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRDBAssignColumnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::TableDef> dbTable, NN<DB::ReadingDB> dataFile, Text::CString schema, Text::CStringNN table, Bool noHeader, Int8 dataFileTz, NN<Data::ArrayList<UOSInt>> colInd, NN<Data::ArrayListStringNN> colStr);
			virtual ~AVIRDBAssignColumnForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
