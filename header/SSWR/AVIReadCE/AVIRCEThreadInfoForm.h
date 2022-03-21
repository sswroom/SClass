#ifndef _SM_SSWR_AVIREADCE_AVIRCETHREADINFOFORM
#define _SM_SSWR_AVIREADCE_AVIRCETHREADINFOFORM

#include "Manage/SymbolResolver.h"
#include "Text/String.h"
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
	namespace AVIReadCE
	{
		class AVIRCEThreadInfoForm : public UI::GUIForm
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

			Data::ArrayList<Text::String *> *stacks;
			Data::ArrayList<Text::String *> *stacksMem;
			Manage::Process *proc;
			Manage::SymbolResolver *symbol;

			static void __stdcall OnMyStackChg(void *userObj);
			static void __stdcall OnMyStackDblClk(void *userObj, UOSInt index);
		public:
			AVIRCEThreadInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, Manage::Process *proc, Manage::SymbolResolver *symbol, Int32 threadId);
			virtual ~AVIRCEThreadInfoForm();
		};
	};
};
#endif
