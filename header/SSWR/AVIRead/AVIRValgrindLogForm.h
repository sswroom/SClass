#ifndef _SM_SSWR_AVIREAD_AVIRVALGRINDLOGFORM
#define _SM_SSWR_AVIREAD_AVIRVALGRINDLOGFORM
#include "IO/ValgrindLog.h"
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
		class AVIRValgrindLogForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::ValgrindLog> log;

			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUILabel> lblPPID;
			NN<UI::GUITextBox> txtPPID;
			NN<UI::GUILabel> lblPID;
			NN<UI::GUITextBox> txtPID;
			NN<UI::GUILabel> lblVersion;
			NN<UI::GUITextBox> txtVersion;
			NN<UI::GUILabel> lblCommandLine;
			NN<UI::GUITextBox> txtCommandLine;
			NN<UI::GUILabel> lblBlocksInUse;
			NN<UI::GUITextBox> txtBlocksInUse;
			NN<UI::GUILabel> lblBytesInUse;
			NN<UI::GUITextBox> txtBytesInUse;
			NN<UI::GUILabel> lblBlocksAllocs;
			NN<UI::GUITextBox> txtBlocksAllocs;
			NN<UI::GUILabel> lblBlocksFrees;
			NN<UI::GUITextBox> txtBlocksFrees;
			NN<UI::GUILabel> lblBytesAllocs;
			NN<UI::GUITextBox> txtBytesAllocs;

			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			Bool LoadFile(NN<Text::String> file);
		public:
			AVIRValgrindLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRValgrindLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
