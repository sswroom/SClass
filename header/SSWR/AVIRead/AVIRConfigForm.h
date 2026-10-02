#ifndef _SM_SSWR_AVIREAD_AVIRCONFIGFORM
#define _SM_SSWR_AVIREAD_AVIRCONFIGFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "IO/ConfigFile.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRConfigForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListBox> lbCategory;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUIListView> lvKeyValues;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::ConfigFile> cfg;

			static void __stdcall OnCategorySelChg(AnyType userObj);
		public:
			AVIRConfigForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::ConfigFile> cfg);
			virtual ~AVIRConfigForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
