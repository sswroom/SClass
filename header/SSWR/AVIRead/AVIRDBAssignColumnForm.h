#ifndef _SM_SSWR_AVIREAD_AVIRDBASSIGNCOLUMNFORM
#define _SM_SSWR_AVIREAD_AVIRDBASSIGNCOLUMNFORM
#include "DB/TableDef.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBAssignColumnForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlColumns;
			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIComboBox> *colsCbo;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::ArrayList<UOSInt>> colInd;
			NN<DB::TableDef> dbTable;
			NN<DB::ReadingDB> dataFile;
			Text::CString schema;
			Text::CStringNN table;
			Bool dataFileNoHeader;
			Int8 dataFileTz;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRDBAssignColumnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::TableDef> dbTable, NN<DB::ReadingDB> dataFile, Text::CString schema, Text::CStringNN table, Bool noHeader, Int8 dataFileTz, NN<Data::ArrayList<UOSInt>> colInd);
			virtual ~AVIRDBAssignColumnForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
