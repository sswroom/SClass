#ifndef _SM_SSWR_AVIREAD_AVIRPROGRAMLINKSFORM
#define _SM_SSWR_AVIREAD_AVIRPROGRAMLINKSFORM
#include "IO/ProgramLinkManager.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRProgramLinksForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbItems;
			UI::GUIHSplitter *hspItems;
			NotNullPtr<UI::GUIPanel> pnlItem;
			UI::GUIButton *btnDelete;
			UI::GUIButton *btnCreate;
			UI::GUILabel *lblLinkName;
			UI::GUITextBox *txtLinkName;
			UI::GUILabel *lblType;
			UI::GUITextBox *txtType;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblGenericName;
			UI::GUITextBox *txtGenericName;
			UI::GUILabel *lblVersion;
			UI::GUITextBox *txtVersion;
			UI::GUILabel *lblComment;
			UI::GUITextBox *txtComment;
			UI::GUILabel *lblMimeTypes;
			UI::GUITextBox *txtMimeTypes;
			UI::GUILabel *lblCategories;
			UI::GUITextBox *txtCategories;
			UI::GUILabel *lblKeywords;
			UI::GUITextBox *txtKeywords;
			UI::GUILabel *lblCmdLine;
			UI::GUITextBox *txtCmdLine;
			UI::GUILabel *lblNoDisplay;
			UI::GUITextBox *txtNoDisplay;
			UI::GUILabel *lblStartupNotify;
			UI::GUITextBox *txtStartupNotify;
			UI::GUILabel *lblTerminal;
			UI::GUITextBox *txtTerminal;
			UI::GUILabel *lblIcon;
			UI::GUITextBox *txtIcon;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::ProgramLinkManager progMgr;

			static void __stdcall OnItemsSelChg(void *userObj);
			static void __stdcall OnDeleteClicked(void *userObj);
			static void __stdcall OnCreateClicked(void *userObj);

			void UpdateLinkList();
		public:
			AVIRProgramLinksForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProgramLinksForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
