#ifndef _SM_SSWR_AVIREAD_AVIRTHREADINFOFORM
#define _SM_SSWR_AVIREAD_AVIRTHREADINFOFORM
#include "Data/ArrayListArr.h"
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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUILabel> lblThreadId;
			NN<UI::GUITextBox> txtThreadId;
			NN<UI::GUILabel> lblStartAddr;
			NN<UI::GUITextBox> txtStartAddr;
			NN<UI::GUILabel> lblStartName;
			NN<UI::GUITextBox> txtStartName;
			NN<UI::GUILabel> lblThreadName;
			NN<UI::GUITextBox> txtThreadName;

			NN<UI::GUITabPage> tpContext;
			NN<UI::GUIListView> lvContext;

			NN<UI::GUITabPage> tpStack;
			NN<UI::GUIListView> lvStack;

			NN<UI::GUITabPage> tpMyStack;
			NN<UI::GUIListBox> lbMyStack;
			NN<UI::GUIHSplitter> hspMyStack;
			NN<UI::GUIPanel> pnlMyStack;
			NN<UI::GUITextBox> txtMyStackMem;
			NN<UI::GUIVSplitter> vspMyStack;
			NN<UI::GUIListView> lvMyStack;

			NN<SSWR::AVIRead::AVIRCore> core;

			Data::ArrayListArr<const UTF8Char> stacks;
			Data::ArrayListArr<const UTF8Char> stacksMem;
			NN<Manage::Process> proc;
			NN<Manage::SymbolResolver> symbol;
			Manage::ThreadContext::ContextType contextType;

			static void __stdcall OnMyStackChg(AnyType userObj);
			static void __stdcall OnMyStackDblClk(AnyType userObj, UOSInt index);
		public:
			AVIRThreadInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Manage::Process> proc, NN<Manage::SymbolResolver> symbol, UInt32 threadId);
			virtual ~AVIRThreadInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
