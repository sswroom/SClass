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

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBCopyTablesForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpSource;
			UI::GUILabel *lblSourceDB;
			UI::GUIComboBox *cboSourceDB;
			UI::GUILabel *lblSourceSchema;
			UI::GUIComboBox *cboSourceSchema;
			UI::GUIButton *btnSourceSelect;

			UI::GUITabPage *tpData;
			UI::GUIGroupBox *grpDest;
			UI::GUIListView *lvData;
			UI::GUILabel *lblDestDB;
			UI::GUIComboBox *cboDestDB;
			UI::GUILabel *lblDestSchema;
			UI::GUIComboBox *cboDestSchema;
			UI::GUILabel *lblDestOptions;
			UI::GUIComboBox *cboDestTableType;
			UI::GUICheckBox *chkDestCopyData;
			UI::GUIButton *btnCopy;

			SSWR::AVIRead::AVIRCore *core;
			Data::ArrayList<DB::DBManagerCtrl*> *dbList;

			DB::DBTool *dataConn;
			Text::String *dataSchema;
			Data::ArrayList<Text::String*> dataTables;

			static void __stdcall OnSourceDBChg(void *userObj);
			static void __stdcall OnSourceSelectClicked(void *userObj);
			static void __stdcall OnDestDBChg(void *userObj);
			static void __stdcall OnCopyClicked(void *userObj);
		public:
			AVIRDBCopyTablesForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Data::ArrayList<DB::DBManagerCtrl*> *dbList);
			virtual ~AVIRDBCopyTablesForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
