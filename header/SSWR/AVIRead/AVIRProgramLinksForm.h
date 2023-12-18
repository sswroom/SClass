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
			NotNullPtr<UI::GUIHSplitter> hspItems;
			NotNullPtr<UI::GUIPanel> pnlItem;
			NotNullPtr<UI::GUIButton> btnDelete;
			NotNullPtr<UI::GUIButton> btnCreate;
			NotNullPtr<UI::GUILabel> lblLinkName;
			UI::GUITextBox *txtLinkName;
			NotNullPtr<UI::GUILabel> lblType;
			UI::GUITextBox *txtType;
			NotNullPtr<UI::GUILabel> lblName;
			UI::GUITextBox *txtName;
			NotNullPtr<UI::GUILabel> lblGenericName;
			UI::GUITextBox *txtGenericName;
			NotNullPtr<UI::GUILabel> lblVersion;
			UI::GUITextBox *txtVersion;
			NotNullPtr<UI::GUILabel> lblComment;
			UI::GUITextBox *txtComment;
			NotNullPtr<UI::GUILabel> lblMimeTypes;
			UI::GUITextBox *txtMimeTypes;
			NotNullPtr<UI::GUILabel> lblCategories;
			UI::GUITextBox *txtCategories;
			NotNullPtr<UI::GUILabel> lblKeywords;
			UI::GUITextBox *txtKeywords;
			NotNullPtr<UI::GUILabel> lblCmdLine;
			UI::GUITextBox *txtCmdLine;
			NotNullPtr<UI::GUILabel> lblNoDisplay;
			UI::GUITextBox *txtNoDisplay;
			NotNullPtr<UI::GUILabel> lblStartupNotify;
			UI::GUITextBox *txtStartupNotify;
			NotNullPtr<UI::GUILabel> lblTerminal;
			UI::GUITextBox *txtTerminal;
			NotNullPtr<UI::GUILabel> lblIcon;
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
