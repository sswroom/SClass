#ifndef _SM_SSWR_AVIREADCE_AVIRCETHREADINFOFORM
#define _SM_SSWR_AVIREADCE_AVIRCETHREADINFOFORM
#include "Data/ArrayListStringNN.h"
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
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUITabPage> tpContext;
			NotNullPtr<UI::GUITabPage> tpStack;
			NotNullPtr<UI::GUITabPage> tpMyStack;

			NotNullPtr<UI::GUILabel> lblThreadId;
			NotNullPtr<UI::GUITextBox> txtThreadId;
			NotNullPtr<UI::GUILabel> lblStartAddr;
			NotNullPtr<UI::GUITextBox> txtStartAddr;
			NotNullPtr<UI::GUILabel> lblStartName;
			NotNullPtr<UI::GUITextBox> txtStartName;

			UI::GUIListView *lvContext;

			UI::GUIListView *lvStack;

			UI::GUIListBox *lbMyStack;
			NotNullPtr<UI::GUIHSplitter> hspMyStack;
			NotNullPtr<UI::GUIPanel> pnlMyStack;
			NotNullPtr<UI::GUITextBox> txtMyStackMem;
			NotNullPtr<UI::GUIVSplitter> vspMyStack;
			UI::GUIListView *lvMyStack;

			Data::ArrayListStringNN stacks;
			Data::ArrayListStringNN stacksMem;
			Manage::Process *proc;
			Manage::SymbolResolver *symbol;

			static void __stdcall OnMyStackChg(void *userObj);
			static void __stdcall OnMyStackDblClk(void *userObj, UOSInt index);
		public:
			AVIRCEThreadInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, Manage::Process *proc, Manage::SymbolResolver *symbol, Int32 threadId);
			virtual ~AVIRCEThreadInfoForm();
		};
	}
}
#endif
