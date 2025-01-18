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
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUITabPage> tpContext;
			NN<UI::GUITabPage> tpStack;
			NN<UI::GUITabPage> tpMyStack;

			NN<UI::GUILabel> lblThreadId;
			NN<UI::GUITextBox> txtThreadId;
			NN<UI::GUILabel> lblStartAddr;
			NN<UI::GUITextBox> txtStartAddr;
			NN<UI::GUILabel> lblStartName;
			NN<UI::GUITextBox> txtStartName;

			NN<UI::GUIListView> lvContext;

			NN<UI::GUIListView> lvStack;

			NN<UI::GUIListBox> lbMyStack;
			NN<UI::GUIHSplitter> hspMyStack;
			NN<UI::GUIPanel> pnlMyStack;
			NN<UI::GUITextBox> txtMyStackMem;
			NN<UI::GUIVSplitter> vspMyStack;
			NN<UI::GUIListView> lvMyStack;

			Data::ArrayListStringNN stacks;
			Data::ArrayListStringNN stacksMem;
			NN<Manage::Process> proc;
			Optional<Manage::SymbolResolver> symbol;

			static void __stdcall OnMyStackChg(AnyType userObj);
			static void __stdcall OnMyStackDblClk(AnyType userObj, UOSInt index);
		public:
			AVIRCEThreadInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Manage::Process> proc, Optional<Manage::SymbolResolver> symbol, Int32 threadId);
			virtual ~AVIRCEThreadInfoForm();
		};
	}
}
#endif
