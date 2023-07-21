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
			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpInfo;
			UI::GUITabPage *tpContext;
			UI::GUITabPage *tpStack;
			UI::GUITabPage *tpMyStack;

			UI::GUILabel *lblThreadId;
			UI::GUITextBox *txtThreadId;
			UI::GUILabel *lblStartAddr;
			UI::GUITextBox *txtStartAddr;
			UI::GUILabel *lblStartName;
			UI::GUITextBox *txtStartName;

			UI::GUIListView *lvContext;

			UI::GUIListView *lvStack;

			UI::GUIListBox *lbMyStack;
			UI::GUIHSplitter *hspMyStack;
			UI::GUIPanel *pnlMyStack;
			UI::GUITextBox *txtMyStackMem;
			UI::GUIVSplitter *vspMyStack;
			UI::GUIListView *lvMyStack;

			SSWR::AVIRead::AVIRCore *core;

			Data::ArrayListNN<const UTF8Char> stacks;
			Data::ArrayListNN<const UTF8Char> stacksMem;
			Manage::Process *proc;
			Manage::SymbolResolver *symbol;
			Manage::ThreadContext::ContextType contextType;

			static void __stdcall OnMyStackChg(void *userObj);
			static void __stdcall OnMyStackDblClk(void *userObj, UOSInt index);
		public:
			AVIRThreadInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Manage::Process *proc, Manage::SymbolResolver *symbol, UInt32 threadId);
			virtual ~AVIRThreadInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
