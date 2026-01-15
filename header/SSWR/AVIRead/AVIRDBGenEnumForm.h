#ifndef _SM_SSWR_AVIREAD_AVIRDBGENENUMFORM
#define _SM_SSWR_AVIREAD_AVIRDBGENENUMFORM
#include "DB/ReadingDB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBGenEnumForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUITextBox> txtEnum;
			NN<UI::GUIListBox> lbValues;
			NN<UI::GUIHSplitter> hspCtrl;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblValueCase;
			NN<UI::GUIComboBox> cboValueCase;
			NN<UI::GUILabel> lblOutputCase;
			NN<UI::GUIComboBox> cboOutputCase;
			NN<UI::GUILabel> lblOutputFormat;
			NN<UI::GUIComboBox> cboOutputFormat; 

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<DB::ReadingDB> db;
			NN<Text::String> colName;
			Text::CString schema;
			Text::CStringNN table;
			UIntOS colIndex;
			Data::ArrayListStringNN nameList;

			void Generate();
			static void __stdcall OnUpdate(AnyType userObj);
		public:
			AVIRDBGenEnumForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN table, UIntOS colIndex);
			virtual ~AVIRDBGenEnumForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
