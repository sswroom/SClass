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
			NotNullPtr<UI::GUIListView> lvTable;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UOSInt colCnt;

		public:
			AVIRTableMsgForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Text::CStringNN title, UOSInt colCnt, const UTF8Char **colName);
			virtual ~AVIRTableMsgForm();

			virtual void OnMonitorChanged();

			void AddRow(const UTF8Char **row);
		};
	}
}
#endif
