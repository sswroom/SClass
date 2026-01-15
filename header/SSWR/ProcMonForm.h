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
			NN<Text::String> progName;
			Optional<Text::String> progPath;
			UIntOS procId;
		} ProgInfo;
	private:
		NN<UI::GUITabControl> tcMain;
		NN<UI::GUITabPage> tpLog;
		NN<UI::GUIListBox> lbLog;
		NN<UI::GUITextBox> txtLog;

		NN<UI::GUITabPage> tpProg;
		NN<UI::GUIListBox> lbProg;
		NN<UI::GUIPanel> pnlProg;
		NN<UI::GUIGroupBox> grpProgAdd;
		NN<UI::GUILabel> lblProgAddName;
		NN<UI::GUITextBox> txtProgAddName;
		NN<UI::GUILabel> lblProgAddId;
		NN<UI::GUITextBox> txtProgAddId;
		NN<UI::GUIButton> btnProgAdd;
		NN<UI::GUILabel> lblProcId;
		NN<UI::GUITextBox> txtProcId;
		NN<UI::GUIButton> btnProcId;
		NN<UI::GUILabel> lblProgPath;
		NN<UI::GUITextBox> txtProgPath;

		Data::ArrayListNN<ProgInfo> progList;
		IO::LogTool log;
		NN<UI::ListBoxLogger> logger;
		Text::String *notifyCmd;

		void AddProg(Text::CStringNN progName, Text::CString progPath);
		Bool SearchProcId(NN<ProgInfo> prog);
		void SetByProcId(ProgInfo *prog, UIntOS procId);
		void LoadProgList();
		void SaveProgList();
		static void __stdcall OnProgSelChange(AnyType userObj);
		static void __stdcall OnProcIdClicked(AnyType userObj);
		static void __stdcall OnProgAddClicked(AnyType userObj);
		static void __stdcall OnLogSelChg(AnyType userObj);
		static void __stdcall OnTimerTick(AnyType userObj);
	public:
		ProcMonForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui);
		virtual ~ProcMonForm();
	};
}
#endif