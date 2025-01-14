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
			NN<IO::ProgramLinkManager> progMgr;
			NN<UI::GUICheckBox> chkThisUser;
			NN<UI::GUILabel> lblShortName;
			NN<UI::GUITextBox> txtShortName;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblComment;
			NN<UI::GUITextBox> txtComment;
			NN<UI::GUILabel> lblCategories;
			NN<UI::GUITextBox> txtCategories;
			NN<UI::GUILabel> lblCmdLine;
			NN<UI::GUITextBox> txtCmdLine;
			NN<UI::GUIButton> btnCreate;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnCreateClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRProgramLinksCreateForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::ProgramLinkManager> progMgr);
			virtual ~AVIRProgramLinksCreateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
