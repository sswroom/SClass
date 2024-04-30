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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Text::CodeProject> proj;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblConfig;
			NN<UI::GUIComboBox> cboConfig;
			UI::GUITreeView *tvMain;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITextBox> txtMessage;

			static void __stdcall OnItemSelected(AnyType userObj);
			void DisplayStatus(NN<Text::StringBuilderUTF8> sb, NN<Text::Cpp::CppParseStatus> status);
			void AddTreeObj(Optional<UI::GUITreeView::TreeItem> parent, NN<Text::CodeContainer> container);
		public:
			AVIRCodeProjectForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Text::CodeProject> proj);
			virtual ~AVIRCodeProjectForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
