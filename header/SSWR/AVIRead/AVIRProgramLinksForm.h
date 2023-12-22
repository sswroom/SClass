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
			NotNullPtr<UI::GUIListBox> lbItems;
			NotNullPtr<UI::GUIHSplitter> hspItems;
			NotNullPtr<UI::GUIPanel> pnlItem;
			NotNullPtr<UI::GUIButton> btnDelete;
			NotNullPtr<UI::GUIButton> btnCreate;
			NotNullPtr<UI::GUILabel> lblLinkName;
			NotNullPtr<UI::GUITextBox> txtLinkName;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUITextBox> txtType;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblGenericName;
			NotNullPtr<UI::GUITextBox> txtGenericName;
			NotNullPtr<UI::GUILabel> lblVersion;
			NotNullPtr<UI::GUITextBox> txtVersion;
			NotNullPtr<UI::GUILabel> lblComment;
			NotNullPtr<UI::GUITextBox> txtComment;
			NotNullPtr<UI::GUILabel> lblMimeTypes;
			NotNullPtr<UI::GUITextBox> txtMimeTypes;
			NotNullPtr<UI::GUILabel> lblCategories;
			NotNullPtr<UI::GUITextBox> txtCategories;
			NotNullPtr<UI::GUILabel> lblKeywords;
			NotNullPtr<UI::GUITextBox> txtKeywords;
			NotNullPtr<UI::GUILabel> lblCmdLine;
			NotNullPtr<UI::GUITextBox> txtCmdLine;
			NotNullPtr<UI::GUILabel> lblNoDisplay;
			NotNullPtr<UI::GUITextBox> txtNoDisplay;
			NotNullPtr<UI::GUILabel> lblStartupNotify;
			NotNullPtr<UI::GUITextBox> txtStartupNotify;
			NotNullPtr<UI::GUILabel> lblTerminal;
			NotNullPtr<UI::GUITextBox> txtTerminal;
			NotNullPtr<UI::GUILabel> lblIcon;
			NotNullPtr<UI::GUITextBox> txtIcon;

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
