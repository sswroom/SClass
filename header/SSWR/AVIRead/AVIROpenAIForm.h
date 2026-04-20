#ifndef _SM_SSWR_AVIREAD_AVIROPENAIFORM
#define _SM_SSWR_AVIREAD_AVIROPENAIFORM
#include "Net/OpenAIClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROpenAIForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblKey;
			NN<UI::GUITextBox> txtKey;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblModel;
			NN<UI::GUIComboBox> cboModel;
			NN<UI::GUIPanel> pnlQuestion;
			NN<UI::GUIPanel> pnlQuestionFile;
			NN<UI::GUITextBox> txtQuestion;
			NN<UI::GUILabel> lblFiles;
			NN<UI::GUILabel> lblFileNames;
			NN<UI::GUIVSplitter> vspQuestion;
			NN<UI::GUITextBox> txtAnswer;
			NN<SSWR::AVIRead::AVIRCore> core;

			Optional<Net::SSLEngine> ssl;
			Optional<Net::OpenAIClient> cli;
			Data::ArrayListStringNN fileList;

			static void __stdcall OnStartClicked(AnyType userObj);
			static UI::EventState __stdcall OnQuestionKeyDown(AnyType userObj, UInt32 osKey);
			static void __stdcall OnModelSelChg(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		public:
			AVIROpenAIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROpenAIForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
