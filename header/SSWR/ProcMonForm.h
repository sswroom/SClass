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
		NotNullPtr<UI::GUITabControl> tcMain;
		NotNullPtr<UI::GUITabPage> tpLog;
		NotNullPtr<UI::GUIListBox> lbLog;
		NotNullPtr<UI::GUITextBox> txtLog;

		NotNullPtr<UI::GUITabPage> tpProg;
		NotNullPtr<UI::GUIListBox> lbProg;
		NotNullPtr<UI::GUIPanel> pnlProg;
		NotNullPtr<UI::GUIGroupBox> grpProgAdd;
		NotNullPtr<UI::GUILabel> lblProgAddName;
		NotNullPtr<UI::GUITextBox> txtProgAddName;
		NotNullPtr<UI::GUILabel> lblProgAddId;
		NotNullPtr<UI::GUITextBox> txtProgAddId;
		NotNullPtr<UI::GUIButton> btnProgAdd;
		NotNullPtr<UI::GUILabel> lblProcId;
		NotNullPtr<UI::GUITextBox> txtProcId;
		NotNullPtr<UI::GUIButton> btnProcId;
		NotNullPtr<UI::GUILabel> lblProgPath;
		NotNullPtr<UI::GUITextBox> txtProgPath;

		Data::ArrayList<ProgInfo*> *progList;
		IO::LogTool *log;
		NotNullPtr<UI::ListBoxLogger> logger;
		Text::String *notifyCmd;

		void AddProg(Text::CString progName, Text::CString progPath);
		Bool SearchProcId(ProgInfo *prog);
		void SetByProcId(ProgInfo *prog, UOSInt procId);
		void LoadProgList();
		void SaveProgList();
		static void __stdcall OnProgSelChange(AnyType userObj);
		static void __stdcall OnProcIdClicked(AnyType userObj);
		static void __stdcall OnProgAddClicked(AnyType userObj);
		static void __stdcall OnLogSelChg(AnyType userObj);
		static void __stdcall OnTimerTick(AnyType userObj);
	public:
		ProcMonForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui);
		virtual ~ProcMonForm();
	};
}
#endif