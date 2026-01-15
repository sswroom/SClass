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
			NN<UI::GUIListView> lvMyStack;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnMyStackDblClk(AnyType userObj, UIntOS index);
		public:
			AVIRFunctionInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Manage::Process> proc, Optional<Manage::SymbolResolver> symbol, Int64 funcAddr);
			virtual ~AVIRFunctionInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
