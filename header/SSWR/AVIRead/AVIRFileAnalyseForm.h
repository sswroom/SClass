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
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			NotNullPtr<UI::GUIButton> btnFile;
			UI::GUITabControl *tcMain;
			NotNullPtr<UI::GUITabPage> tpContent;
			NotNullPtr<UI::GUITabPage> tpStream;
			NotNullPtr<UI::GUITabPage> tpTool;

			UI::GUIListBox *lbPackList;
			NotNullPtr<UI::GUIHSplitter> hspPack;
			UI::GUIListBox *lbPackItems;
			NotNullPtr<UI::GUIHSplitter> hspPack2;
			UI::GUITextBox *txtPack;

			NotNullPtr<UI::GUIButton> btnTrimPadding;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::FileAnalyse::IFileAnalyse *file;
			UOSInt lastPackCount;

			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnTrimPaddingClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPackListChanged(void *userObj);
			static void __stdcall OnPackItemChanged(void *userObj);
			Bool OpenFile(Text::CStringNN fileName);
		public:
			AVIRFileAnalyseForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileAnalyseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
