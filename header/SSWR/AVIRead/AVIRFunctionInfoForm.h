#ifndef _SM_SSWR_AVIREAD_AVIRFUNCTIONINFOFORM
#define _SM_SSWR_AVIREAD_AVIRFUNCTIONINFOFORM
#include "Manage/Process.h"
#include "Manage/SymbolResolver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFunctionInfoForm : public UI::GUIForm
		{
		private:
			UI::GUIListView *lvMyStack;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnMyStackDblClk(void *userObj, UOSInt index);
		public:
			AVIRFunctionInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Manage::Process *proc, Manage::SymbolResolver *symbol, Int64 funcAddr);
			virtual ~AVIRFunctionInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
