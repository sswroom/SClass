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
			NN<UI::GUIListBox> lbItems;
			NN<UI::GUIHSplitter> hspItems;
			NN<UI::GUIPanel> pnlItem;
			NN<UI::GUIButton> btnDelete;
			NN<UI::GUIButton> btnCreate;
			NN<UI::GUILabel> lblLinkName;
			NN<UI::GUITextBox> txtLinkName;
			NN<UI::GUILabel> lblType;
			NN<UI::GUITextBox> txtType;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblGenericName;
			NN<UI::GUITextBox> txtGenericName;
			NN<UI::GUILabel> lblVersion;
			NN<UI::GUITextBox> txtVersion;
			NN<UI::GUILabel> lblComment;
			NN<UI::GUITextBox> txtComment;
			NN<UI::GUILabel> lblMimeTypes;
			NN<UI::GUITextBox> txtMimeTypes;
			NN<UI::GUILabel> lblCategories;
			NN<UI::GUITextBox> txtCategories;
			NN<UI::GUILabel> lblKeywords;
			NN<UI::GUITextBox> txtKeywords;
			NN<UI::GUILabel> lblCmdLine;
			NN<UI::GUITextBox> txtCmdLine;
			NN<UI::GUILabel> lblNoDisplay;
			NN<UI::GUITextBox> txtNoDisplay;
			NN<UI::GUILabel> lblStartupNotify;
			NN<UI::GUITextBox> txtStartupNotify;
			NN<UI::GUILabel> lblTerminal;
			NN<UI::GUITextBox> txtTerminal;
			NN<UI::GUILabel> lblIcon;
			NN<UI::GUITextBox> txtIcon;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::ProgramLinkManager progMgr;

			static void __stdcall OnItemsSelChg(AnyType userObj);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnCreateClicked(AnyType userObj);

			void UpdateLinkList();
		public:
			AVIRProgramLinksForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRProgramLinksForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
