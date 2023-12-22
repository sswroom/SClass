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
			NotNullPtr<UI::GUIPanel> pnlFile;
			NotNullPtr<UI::GUILabel> lblFile;
			NotNullPtr<UI::GUITextBox> txtFile;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpProgGroup;
			NotNullPtr<UI::GUIListBox> lbProgGroup;
			NotNullPtr<UI::GUIHSplitter> hspProgGroup;
			NotNullPtr<UI::GUIListBox> lbProgGroupItems;

			NotNullPtr<UI::GUITabPage> tpProg;
			NotNullPtr<UI::GUIListBox> lbProg;
			NotNullPtr<UI::GUIHSplitter> hspProg;
			NotNullPtr<UI::GUIPanel> pnlProg;
			NotNullPtr<UI::GUITabControl> tcProg;

			NotNullPtr<UI::GUITabPage> tpProgObject;
			NotNullPtr<UI::GUIListBox> lbProgObject;

			NotNullPtr<UI::GUITabPage> tpProgHeader;
			NotNullPtr<UI::GUIListBox> lbProgHeader;

			NotNullPtr<UI::GUITabPage> tpProgSource;
			NotNullPtr<UI::GUIListBox> lbProgSource;

			NotNullPtr<UI::GUITabPage> tpConfig;
			NotNullPtr<UI::GUIListView> lvConfig;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::SMake> smake;

			static void __stdcall OnProgSelChg(void *userObj);
			static void __stdcall OnProgGroupSelChg(void *userObj);
		public:
			AVIRSMakeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::SMake> smake);
			virtual ~AVIRSMakeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
