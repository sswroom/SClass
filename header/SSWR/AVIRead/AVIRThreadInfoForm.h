#ifndef _SM_SSWR_AVIREAD_AVIRTHREADINFOFORM
#define _SM_SSWR_AVIREAD_AVIRTHREADINFOFORM

#include "Manage/SymbolResolver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRThreadInfoForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUILabel> lblThreadId;
			NotNullPtr<UI::GUITextBox> txtThreadId;
			NotNullPtr<UI::GUILabel> lblStartAddr;
			NotNullPtr<UI::GUITextBox> txtStartAddr;
			NotNullPtr<UI::GUILabel> lblStartName;
			NotNullPtr<UI::GUITextBox> txtStartName;
			NotNullPtr<UI::GUILabel> lblThreadName;
			NotNullPtr<UI::GUITextBox> txtThreadName;

			NotNullPtr<UI::GUITabPage> tpContext;
			NotNullPtr<UI::GUIListView> lvContext;

			NotNullPtr<UI::GUITabPage> tpStack;
			NotNullPtr<UI::GUIListView> lvStack;

			NotNullPtr<UI::GUITabPage> tpMyStack;
			NotNullPtr<UI::GUIListBox> lbMyStack;
			NotNullPtr<UI::GUIHSplitter> hspMyStack;
			NotNullPtr<UI::GUIPanel> pnlMyStack;
			NotNullPtr<UI::GUITextBox> txtMyStackMem;
			NotNullPtr<UI::GUIVSplitter> vspMyStack;
			NotNullPtr<UI::GUIListView> lvMyStack;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListNN<const UTF8Char> stacks;
			Data::ArrayListNN<const UTF8Char> stacksMem;
			Manage::Process *proc;
			Manage::SymbolResolver *symbol;
			Manage::ThreadContext::ContextType contextType;

			static void __stdcall OnMyStackChg(AnyType userObj);
			static void __stdcall OnMyStackDblClk(AnyType userObj, UOSInt index);
		public:
			AVIRThreadInfoForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Manage::Process *proc, Manage::SymbolResolver *symbol, UInt32 threadId);
			virtual ~AVIRThreadInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
