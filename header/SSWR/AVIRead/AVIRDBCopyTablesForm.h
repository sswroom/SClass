#ifndef _SM_SSWR_AVIREAD_AVIRDBCOPYTABLESFORM
#define _SM_SSWR_AVIREAD_AVIRDBCOPYTABLESFORM
#include "DB/DBManagerCtrl.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBCopyTablesForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpSource;
			UI::GUILabel *lblSourceConn;
			NotNullPtr<UI::GUIComboBox> cboSourceConn;
			UI::GUILabel *lblSourceDB;
			UI::GUITextBox *txtSourceDB;
			UI::GUILabel *lblSourceCollation;
			UI::GUITextBox *txtSourceCollation;
			UI::GUILabel *lblSourceSchema;
			NotNullPtr<UI::GUIComboBox> cboSourceSchema;
			NotNullPtr<UI::GUIButton> btnSourceSelect;

			NotNullPtr<UI::GUITabPage> tpData;
			NotNullPtr<UI::GUIGroupBox> grpDest;
			UI::GUIListView *lvData;
			UI::GUILabel *lblDestDB;
			NotNullPtr<UI::GUIComboBox> cboDestDB;
			UI::GUILabel *lblDestSchema;
			NotNullPtr<UI::GUIComboBox> cboDestSchema;
			UI::GUILabel *lblDestOptions;
			NotNullPtr<UI::GUIComboBox> cboDestTableType;
			UI::GUICheckBox *chkDestCopyData;
			UI::GUICheckBox *chkDestCreateDDB;
			NotNullPtr<UI::GUIButton> btnCopy;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayList<DB::DBManagerCtrl*> *dbList;

			DB::ReadingDB *dataConn;
			Text::String *dataSchema;
			Data::ArrayListStringNN dataTables;

			static void __stdcall OnSourceDBChg(void *userObj);
			static void __stdcall OnSourceSelectClicked(void *userObj);
			static void __stdcall OnDestDBChg(void *userObj);
			static void __stdcall OnCopyClicked(void *userObj);
		public:
			AVIRDBCopyTablesForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Data::ArrayList<DB::DBManagerCtrl*> *dbList);
			virtual ~AVIRDBCopyTablesForm();

			virtual void OnMonitorChanged();

			void SetSourceDB(UOSInt index);
		};
	}
}
#endif
