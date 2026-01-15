#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTIMPORTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTIMPORTFORM
#include "Data/Invest/InvestmentManager.h"
#include "DB/ReadingDB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
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
			UIntOS timeCol;
			UIntOS valueCol;
			Data::Invest::DateFormat fmt;
			Bool invert;

			NN<UI::GUIListView> lvMain;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblTimeCol;
			NN<UI::GUIComboBox> cboTimeCol;
			NN<UI::GUILabel> lblValueCol;
			NN<UI::GUIComboBox> cboValueCol;
			NN<UI::GUILabel> lblDateFormat;
			NN<UI::GUIComboBox> cboDateFormat;
			NN<UI::GUICheckBox> chkInvert;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnFile(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		
			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRInvestmentImportForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Bool invert);
			virtual ~AVIRInvestmentImportForm();

			virtual void OnMonitorChanged();

			UIntOS GetTimeCol() const { return this->timeCol; }
			UIntOS GetValueCol() const { return this->valueCol; }
			Data::Invest::DateFormat GetDateFormat() const { return this->fmt; }
			Optional<DB::ReadingDB> GetDB() const { return this->db; }
			Bool IsInvert() const { return this->invert; }
		};
	}
}
#endif
