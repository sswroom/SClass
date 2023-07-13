#ifndef _SM_SSWR_PROCMONFORM
#define _SM_SSWR_PROCMONFORM

#include "IO/LogTool.h"
#include "Text/String.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	class ProcMonForm : public UI::GUIForm
	{
	private:
		typedef struct
		{
			NotNullPtr<Text::String> progName;
			Text::String *progPath;
			UOSInt procId;
		} ProgInfo;
	private:
		UI::GUITabControl *tcMain;
		UI::GUITabPage *tpLog;
		UI::GUIListBox *lbLog;
		UI::GUITextBox *txtLog;

		UI::GUITabPage *tpProg;
		UI::GUIListBox *lbProg;
		UI::GUIPanel *pnlProg;
		UI::GUIGroupBox *grpProgAdd;
		UI::GUILabel *lblProgAddName;
		UI::GUITextBox *txtProgAddName;
		UI::GUILabel *lblProgAddId;
		UI::GUITextBox *txtProgAddId;
		UI::GUIButton *btnProgAdd;
		UI::GUILabel *lblProcId;
		UI::GUITextBox *txtProcId;
		UI::GUIButton *btnProcId;
		UI::GUILabel *lblProgPath;
		UI::GUITextBox *txtProgPath;

		Data::ArrayList<ProgInfo*> *progList;
		IO::LogTool *log;
		UI::ListBoxLogger *logger;
		Text::String *notifyCmd;

		void AddProg(Text::CString progName, Text::CString progPath);
		Bool SearchProcId(ProgInfo *prog);
		void SetByProcId(ProgInfo *prog, UOSInt procId);
		void LoadProgList();
		void SaveProgList();
		static void __stdcall OnProgSelChange(void *userObj);
		static void __stdcall OnProcIdClicked(void *userObj);
		static void __stdcall OnProgAddClicked(void *userObj);
		static void __stdcall OnLogSelChg(void *userObj);
		static void __stdcall OnTimerTick(void *userObj);
	public:
		ProcMonForm(UI::GUIClientControl *parent, UI::GUICore *ui);
		virtual ~ProcMonForm();
	};
}
#endif