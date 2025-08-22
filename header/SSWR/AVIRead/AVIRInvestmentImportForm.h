#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTIMPORTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTIMPORTFORM
#include "DB/ReadingDB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentImportForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<DB::ReadingDB> db;
			UOSInt timeCol;
			UOSInt valueCol;

			NN<UI::GUIListView> lvMain;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblTimeCol;
			NN<UI::GUIComboBox> cboTimeCol;
			NN<UI::GUILabel> lblValueCol;
			NN<UI::GUIComboBox> cboValueCol;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnFile(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		
			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRInvestmentImportForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentImportForm();

			virtual void OnMonitorChanged();

			UOSInt GetTimeCol() const { return this->timeCol; }
			UOSInt GetValueCol() const { return this->valueCol; }
			Optional<DB::ReadingDB> GetDB() const { return this->db; }
		};
	}
}
#endif
