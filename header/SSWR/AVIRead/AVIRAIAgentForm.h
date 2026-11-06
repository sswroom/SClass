#ifndef _SM_SSWR_AVIREAD_AVIRAIAGENTFORM
#define _SM_SSWR_AVIREAD_AVIRAIAGENTFORM
#include "AI/AIReadDBTool.h"
#include "AI/AIReadMDTool.h"
#include "AI/SimpleAIAgent.h"
#include "DB/DBTool.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
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
	namespace AVIRead
	{
		class AVIRAIAgentForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Optional<Net::SSLEngine> ssl;
			Optional<DB::DBTool> db;
			Optional<Net::OpenAIClient> openAI;
			Optional<AI::SimpleAIAgent> agent;
			Data::ArrayListNN<AI::AITool> tools;
			Optional<Text::String> systemPrompt;
			Sync::Mutex resultMut;
			Sync::Thread worker;
			Optional<Text::String> pendingQuestion;
			Optional<Text::String> answer;
			Optional<AI::AISession> pendingSession;
			Optional<AI::AISession> displayedSession;
			Bool completed;

			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpControl;
			NN<UI::GUIPanel> pnlSettings;
			NN<UI::GUITextBox> txtJDBC;
			NN<UI::GUITextBox> txtDBUser;
			NN<UI::GUITextBox> txtDBPassword;
			NN<UI::GUITextBox> txtSchema;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUITextBox> txtMaxToolCalls;
			NN<UI::GUITextBox> txtMDPath;
			NN<UI::GUITextBox> txtPrompt;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStop;

			NN<UI::GUITabPage> tpQuestion;
			NN<UI::GUIGroupBox> grpQuestion;
			NN<UI::GUITextBox> txtQuestion;
			NN<UI::GUIPanel> pnlQuestion;
			NN<UI::GUIButton> btnAsk;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUIGroupBox> grpAnswer;
			NN<UI::GUITextBox> txtAnswer;
			NN<UI::GUIGroupBox> grpSession;
			NN<UI::GUIListBox> lbSession;
			NN<UI::GUITextBox> txtTrace;
			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnStopClicked(AnyType userObj);
			static void __stdcall OnAskClicked(AnyType userObj);
			static void __stdcall OnSessionSelChg(AnyType userObj);
			static UI::EventState __stdcall OnQuestionKeyDown(AnyType userObj, UInt32 osKey);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall WorkerThread(NN<Sync::Thread> thread);
			void Stop(Bool updateUI);

		public:
			AVIRAIAgentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRAIAgentForm();
			virtual void OnMonitorChanged();
		};
	}
}
#endif
