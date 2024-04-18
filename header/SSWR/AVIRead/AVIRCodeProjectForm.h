#ifndef _SM_SSWR_AVIREAD_AVIRCODEPROJECTFORM
#define _SM_SSWR_AVIREAD_AVIRCODEPROJECTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/CodeProject.h"
#include "Text/Cpp/CppParseStatus.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUITreeView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCodeProjectForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::CodeProject *proj;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUILabel> lblConfig;
			NotNullPtr<UI::GUIComboBox> cboConfig;
			UI::GUITreeView *tvMain;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUITextBox> txtMessage;

			static void __stdcall OnItemSelected(AnyType userObj);
			void DisplayStatus(NotNullPtr<Text::StringBuilderUTF8> sb, Text::Cpp::CppParseStatus *status);
			void AddTreeObj(Optional<UI::GUITreeView::TreeItem> parent, Text::CodeContainer *container);
		public:
			AVIRCodeProjectForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Text::CodeProject *proj);
			virtual ~AVIRCodeProjectForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
