#ifndef _SM_SSWR_AVIREAD_AVIROPENAIFORM
#define _SM_SSWR_AVIREAD_AVIROPENAIFORM
#include "Net/OpenAIClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
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
			struct QAPair
			{
				NN<Text::String> question;
				NN<Net::OpenAIResult> result;	
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
			NN<UI::GUIPanel> pnlInputFile;
			NN<UI::GUITextBox> txtInput;
			NN<UI::GUILabel> lblInputFiles;
			NN<UI::GUILabel> lblInputFileNames;

			NN<UI::GUITabPage> tpResult;
			NN<UI::GUIPanel> pnlQAPair;
			NN<UI::GUIButton> btnQAPairClear;
			NN<UI::GUIListBox> lbQAPair;
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

			NN<SSWR::AVIRead::AVIRCore> core;

			Optional<Net::SSLEngine> ssl;
			Optional<Net::OpenAIClient> cli;
			Data::ArrayListStringNN fileList;
			Data::ArrayListNN<QAPair> qaList;

			static void __stdcall OnStartClicked(AnyType userObj);
			static UI::EventState __stdcall OnUserKeyDown(AnyType userObj, UInt32 osKey);
			static void __stdcall OnModelSelChg(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnQAPairSelChg(AnyType userObj);
			static void __stdcall OnQAPairClearClicked(AnyType userObj);
			static void __stdcall FreeQAPair(NN<QAPair> qa);
		public:
			AVIROpenAIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROpenAIForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
