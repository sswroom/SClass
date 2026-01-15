#ifndef _SM_SSWR_AVIREAD_AVIRCPUINFOFORM
#define _SM_SSWR_AVIREAD_AVIRCPUINFOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTableMsgForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListView> lvTable;
			NN<SSWR::AVIRead::AVIRCore> core;
			UIntOS colCnt;

		public:
			AVIRTableMsgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Text::CStringNN title, UIntOS colCnt, UnsafeArray<UnsafeArray<const UTF8Char>> colName);
			virtual ~AVIRTableMsgForm();

			virtual void OnMonitorChanged();

			void AddRow(UnsafeArray<UnsafeArray<const UTF8Char>> row);
		};
	}
}
#endif
