#ifndef _SM_SSWR_AVIREAD_AVIROPENAIFORM
#define _SM_SSWR_AVIREAD_AVIROPENAIFORM
#include "Net/OpenAIClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
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
		class AVIROpenAIForm : public UI::GUIForm
		{
		private:
			struct QuestionInfo
			{
				NN<Text::String> question;
				Optional<Text::String> systemPrompt;
				Optional<Text::String> prevRespId;
				Net::OpenAIReasoningEffort reasoning;
				Data::ArrayListStringNN fileList;
				UIntOS maxTokens;
				Double temperature;
				Double topP;
				Bool background;
				Bool stream;
			};

			struct QAPair
			{
				Double duration;
				NN<Text::String> question;
				NN<Net::OpenAIResult> result;
				NN<Text::String> reqJSON;
				Bool finish;
				Bool reasoningUpdated;
				Bool messageUpdated;
				Bool allUpdated;
			};
		private:
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblModel;
			NN<UI::GUIComboBox> cboModel;
			NN<UI::GUITabControl> tcQuestion;

			NN<UI::GUITabPage> tpSystem;
			NN<UI::GUITextBox> txtSystem;

			NN<UI::GUITabPage> tpInput;
			NN<UI::GUIPanel> pnlInputParam;
			NN<UI::GUILabel> lblInputPrevRespId;
			NN<UI::GUITextBox> txtInputPrevRespId;
			NN<UI::GUILabel> lblInputReasoning;
			NN<UI::GUIComboBox> cboInputReasoning;
			NN<UI::GUILabel> lblInputMaxTokens;
			NN<UI::GUITextBox> txtInputMaxTokens;
			NN<UI::GUILabel> lblInputTemperature;
			NN<UI::GUITextBox> txtInputTemperature;
			NN<UI::GUILabel> lblInputTopP;
			NN<UI::GUITextBox> txtInputTopP;
			NN<UI::GUILabel> lblInputBackground;
			NN<UI::GUICheckBox> chkInputBackground;
			NN<UI::GUILabel> lblInputStream;
			NN<UI::GUICheckBox> chkInputStream;
			NN<UI::GUIPanel> pnlInputFile;
			NN<UI::GUITextBox> txtInput;
			NN<UI::GUILabel> lblInputFiles;
			NN<UI::GUILabel> lblInputFileNames;

			NN<UI::GUITabPage> tpResult;
			NN<UI::GUIPanel> pnlQAPair;
			NN<UI::GUIButton> btnQAPairClear;
			NN<UI::GUIListBox> lbQAPair;
			NN<UI::GUIPanel> pnlResultCtrl;
			NN<UI::GUIButton> btnResultCopy;
			NN<UI::GUILabel> lblResultCopyMsg;
			NN<UI::GUIListView> lvResultValues;
			NN<UI::GUITabControl> tcResult;
			NN<UI::GUITabPage> tpOutputMessage;
			NN<UI::GUITextBox> txtOutputMessage;
			NN<UI::GUITabPage> tpReasoning;
			NN<UI::GUITextBox> txtReasoning;
			NN<UI::GUITabPage> tpResultInstructions;
			NN<UI::GUITextBox> txtResultInstructions;
			NN<UI::GUITabPage> tpResultError;
			NN<UI::GUITextBox> txtResultError;
			NN<UI::GUITabPage> tpResultReqJSON;
			NN<UI::GUIPanel> pnlResultReqJSON;
			NN<UI::GUICheckBox> chkResultReqJSONWF;
			NN<UI::GUITextBox> txtResultReqJSON;

			NN<SSWR::AVIRead::AVIRCore> core;

			Optional<Net::SSLEngine> ssl;
			Optional<Net::OpenAIClient> cli;
			Data::ArrayListStringNN fileList;
			Sync::Mutex qaListMut;
			Data::ArrayListNN<QAPair> qaList;
			Bool qaListUpdated;
			Data::ArrayListNN<QuestionInfo> questionList;
			Sync::Mutex questionListMut;
			Sync::Thread workerThread;
			Optional<QAPair> currQAPair;

			static void __stdcall OnStartClicked(AnyType userObj);
			static UI::EventState __stdcall OnUserKeyDown(AnyType userObj, UInt32 osKey);
			static void __stdcall OnModelSelChg(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnQAPairSelChg(AnyType userObj);
			static void __stdcall OnQAPairClearClicked(AnyType userObj);
			static void __stdcall OnResultReqJSONWFChg(AnyType userObj, Bool newState);
			static void __stdcall OnResultCopyClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall WorkerThread(NN<Sync::Thread> userObj);
			static void __stdcall FreeQuestionInfo(NN<QuestionInfo> qa);
			static void __stdcall FreeQAPair(NN<QAPair> qa);
			void DisplayQAPair(NN<QAPair> qa);
			void DisplayQAPairReqJSON(NN<QAPair> qa, Bool wellFormat);
		public:
			AVIROpenAIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROpenAIForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
