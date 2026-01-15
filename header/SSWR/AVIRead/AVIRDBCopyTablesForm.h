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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpSource;
			NN<UI::GUILabel> lblSourceConn;
			NN<UI::GUIComboBox> cboSourceConn;
			NN<UI::GUILabel> lblSourceDB;
			NN<UI::GUITextBox> txtSourceDB;
			NN<UI::GUILabel> lblSourceCollation;
			NN<UI::GUITextBox> txtSourceCollation;
			NN<UI::GUILabel> lblSourceSchema;
			NN<UI::GUIComboBox> cboSourceSchema;
			NN<UI::GUIButton> btnSourceSelect;

			NN<UI::GUITabPage> tpData;
			NN<UI::GUIPanel> pnlData;
			NN<UI::GUIButton> btnDataSelectNone;
			NN<UI::GUIButton> btnDataSelectAll;
			NN<UI::GUIGroupBox> grpDest;
			NN<UI::GUIListView> lvData;
			NN<UI::GUILabel> lblDestDB;
			NN<UI::GUIComboBox> cboDestDB;
			NN<UI::GUILabel> lblDestSchema;
			NN<UI::GUIComboBox> cboDestSchema;
			NN<UI::GUILabel> lblSQLType;
			NN<UI::GUITextBox> txtSQLType;
			NN<UI::GUILabel> lblDestOptions;
			NN<UI::GUIComboBox> cboDestTableType;
			NN<UI::GUICheckBox> chkDestCopyData;
			NN<UI::GUICheckBox> chkDestCreateDDB;
			NN<UI::GUIButton> btnCopy;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::ArrayListNN<DB::DBManagerCtrl>> dbList;

			Optional<DB::ReadingDB> dataConn;
			Optional<Text::String> dataSchema;
			Data::ArrayListStringNN dataTables;
			Data::ArrayListNative<Bool> dataTableSelected;

			static void __stdcall OnSourceDBChg(AnyType userObj);
			static void __stdcall OnSourceSelectClicked(AnyType userObj);
			static void __stdcall OnDestDBChg(AnyType userObj);
			static void __stdcall OnCopyClicked(AnyType userObj);
			static void __stdcall OnDataDblClk(AnyType userObj, UIntOS index);
			static void __stdcall OnDataSelectAllClicked(AnyType userObj);
			static void __stdcall OnDataSelectNoneClicked(AnyType userObj);
			void SetAllSelect(Bool selected);
		public:
			AVIRDBCopyTablesForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ArrayListNN<DB::DBManagerCtrl>> dbList);
			virtual ~AVIRDBCopyTablesForm();

			virtual void OnMonitorChanged();

			void SetSourceDB(UIntOS index);
		};
	}
}
#endif
