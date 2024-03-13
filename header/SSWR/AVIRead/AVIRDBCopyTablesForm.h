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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpSource;
			NotNullPtr<UI::GUILabel> lblSourceConn;
			NotNullPtr<UI::GUIComboBox> cboSourceConn;
			NotNullPtr<UI::GUILabel> lblSourceDB;
			NotNullPtr<UI::GUITextBox> txtSourceDB;
			NotNullPtr<UI::GUILabel> lblSourceCollation;
			NotNullPtr<UI::GUITextBox> txtSourceCollation;
			NotNullPtr<UI::GUILabel> lblSourceSchema;
			NotNullPtr<UI::GUIComboBox> cboSourceSchema;
			NotNullPtr<UI::GUIButton> btnSourceSelect;

			NotNullPtr<UI::GUITabPage> tpData;
			NotNullPtr<UI::GUIGroupBox> grpDest;
			NotNullPtr<UI::GUIListView> lvData;
			NotNullPtr<UI::GUILabel> lblDestDB;
			NotNullPtr<UI::GUIComboBox> cboDestDB;
			NotNullPtr<UI::GUILabel> lblDestSchema;
			NotNullPtr<UI::GUIComboBox> cboDestSchema;
			NotNullPtr<UI::GUILabel> lblDestOptions;
			NotNullPtr<UI::GUIComboBox> cboDestTableType;
			NotNullPtr<UI::GUICheckBox> chkDestCopyData;
			NotNullPtr<UI::GUICheckBox> chkDestCreateDDB;
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
			AVIRDBCopyTablesForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Data::ArrayList<DB::DBManagerCtrl*> *dbList);
			virtual ~AVIRDBCopyTablesForm();

			virtual void OnMonitorChanged();

			void SetSourceDB(UOSInt index);
		};
	}
}
#endif
