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
			NotNullPtr<UI::GUIPanel> pnlColumns;
			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<UI::GUIComboBox> *colsCbo;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Data::ArrayList<UOSInt>> colInd;
			NotNullPtr<DB::TableDef> dbTable;
			NotNullPtr<DB::ReadingDB> dataFile;
			Text::CString schema;
			Text::CStringNN table;
			Bool dataFileNoHeader;
			Int8 dataFileTz;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRDBAssignColumnForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<DB::TableDef> dbTable, NotNullPtr<DB::ReadingDB> dataFile, Text::CString schema, Text::CStringNN table, Bool noHeader, Int8 dataFileTz, NotNullPtr<Data::ArrayList<UOSInt>> colInd);
			virtual ~AVIRDBAssignColumnForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
