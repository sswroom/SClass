#ifndef _SM_SSWR_AVIREAD_AVIRFILEANALYSEFORM
#define _SM_SSWR_AVIREAD_AVIRFILEANALYSEFORM
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileAnalyseForm : public UI::GUIForm
		{
		private:
			UI::GUIPanel *pnlCtrl;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnFile;
			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpContent;
			UI::GUITabPage *tpStream;
			UI::GUITabPage *tpTool;

			UI::GUIListBox *lbPackList;
			UI::GUIHSplitter *hspPack;
			UI::GUIListBox *lbPackItems;
			UI::GUIHSplitter *hspPack2;
			UI::GUITextBox *txtPack;

			UI::GUIButton *btnTrimPadding;

			SSWR::AVIRead::AVIRCore *core;
			IO::FileAnalyse::IFileAnalyse *file;
			OSInt lastPackCount;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, OSInt nFiles);
			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnTrimPaddingClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPackListChanged(void *userObj);
			static void __stdcall OnPackItemChanged(void *userObj);
			Bool OpenFile(const UTF8Char *fileName);
		public:
			AVIRFileAnalyseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRFileAnalyseForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
