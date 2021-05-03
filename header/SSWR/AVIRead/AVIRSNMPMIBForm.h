#ifndef _SM_SSWR_AVIREAD_AVIRSNMPMIBFORM
#define _SM_SSWR_AVIREAD_AVIRSNMPMIBFORM
#include "Net/SNMPMIB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNMPMIBForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SNMPMIB *mib;

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnBrowse;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpObjects;
			UI::GUIListView *lvObjects;
			UI::GUIVSplitter *vspObjects;
			UI::GUIListView *lvObjectsVal;

			UI::GUITabPage *tpOID;
			UI::GUIListView *lvOID;

			UI::GUITabPage *tpOIDText;
			UI::GUITextBox *txtOIDText;

			static void __stdcall OnFileDroped(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnObjectsSelChg(void *userObj);
			void LoadFile(const UTF8Char *fileName);
		public:
			AVIRSNMPMIBForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSNMPMIBForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
