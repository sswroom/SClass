#ifndef _SM_SSWR_AVIREAD_AVIRGLBVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRGLBVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHexFileView.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGLBViewerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlBinBuff;
			UI::GUIVSplitter *vspMain;
			UI::GUIPanel *pnlJSON;
			UI::GUILabel *lblJSON;
			UI::GUITextBox *txtJSON;
			UI::GUILabel *lblBinBuff;
			UI::GUIHexFileView *hfvBinBuff;

			static void __stdcall OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles);
			Bool LoadFile(Text::String *fileName);
		public:
			AVIRGLBViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRGLBViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
