#ifndef _SM_SSWR_AVIREAD_AVIRSMAKEFORM
#define _SM_SSWR_AVIREAD_AVIRSMAKEFORM
#include "IO/SMake.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSMakeForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpProgGroup;
			NN<UI::GUIListBox> lbProgGroup;
			NN<UI::GUIHSplitter> hspProgGroup;
			NN<UI::GUIListBox> lbProgGroupItems;

			NN<UI::GUITabPage> tpProg;
			NN<UI::GUIListBox> lbProg;
			NN<UI::GUIHSplitter> hspProg;
			NN<UI::GUIPanel> pnlProg;
			NN<UI::GUITabControl> tcProg;

			NN<UI::GUITabPage> tpProgObject;
			NN<UI::GUIListBox> lbProgObject;

			NN<UI::GUITabPage> tpProgHeader;
			NN<UI::GUIListBox> lbProgHeader;

			NN<UI::GUITabPage> tpProgSource;
			NN<UI::GUIListBox> lbProgSource;

			NN<UI::GUITabPage> tpConfig;
			NN<UI::GUIListView> lvConfig;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::SMake> smake;

			static void __stdcall OnProgSelChg(AnyType userObj);
			static void __stdcall OnProgGroupSelChg(AnyType userObj);
		public:
			AVIRSMakeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::SMake> smake);
			virtual ~AVIRSMakeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
