#ifndef _SM_SSWR_AVIREAD_AVIRPROGRAMLINKSCREATEFORM
#define _SM_SSWR_AVIREAD_AVIRPROGRAMLINKSCREATEFORM
#include "IO/ProgramLinkManager.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
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
		class AVIRProgramLinksCreateForm : public UI::GUIForm
		{
		private:
			IO::ProgramLinkManager *progMgr;
			UI::GUICheckBox *chkThisUser;
			NotNullPtr<UI::GUILabel> lblShortName;
			UI::GUITextBox *txtShortName;
			NotNullPtr<UI::GUILabel> lblName;
			UI::GUITextBox *txtName;
			NotNullPtr<UI::GUILabel> lblComment;
			UI::GUITextBox *txtComment;
			NotNullPtr<UI::GUILabel> lblCategories;
			UI::GUITextBox *txtCategories;
			NotNullPtr<UI::GUILabel> lblCmdLine;
			UI::GUITextBox *txtCmdLine;
			NotNullPtr<UI::GUIButton> btnCreate;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnCreateClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRProgramLinksCreateForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::ProgramLinkManager *progMgr);
			virtual ~AVIRProgramLinksCreateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
