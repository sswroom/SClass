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
			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpProgGroup;
			UI::GUIListBox *lbProgGroup;
			UI::GUIHSplitter *hspProgGroup;
			UI::GUIListBox *lbProgGroupItems;

			UI::GUITabPage *tpProg;
			UI::GUIListBox *lbProg;
			UI::GUIHSplitter *hspProg;
			UI::GUIPanel *pnlProg;
			UI::GUITabControl *tcProg;

			UI::GUITabPage *tpProgObject;
			UI::GUIListBox *lbProgObject;

			UI::GUITabPage *tpProgHeader;
			UI::GUIListBox *lbProgHeader;

			UI::GUITabPage *tpProgSource;
			UI::GUIListBox *lbProgSource;

			UI::GUITabPage *tpConfig;
			UI::GUIListView *lvConfig;

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
