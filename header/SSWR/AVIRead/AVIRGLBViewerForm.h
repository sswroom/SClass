#ifndef _SM_SSWR_AVIREAD_AVIRGLBVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRGLBVIEWERFORM
#include "IO/StreamData.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlBinBuff;
			NotNullPtr<UI::GUIVSplitter> vspMain;
			NotNullPtr<UI::GUIPanel> pnlJSON;
			NotNullPtr<UI::GUILabel> lblJSON;
			NotNullPtr<UI::GUITextBox> txtJSON;
			NotNullPtr<UI::GUILabel> lblBinBuff;
			UI::GUIHexFileView *hfvBinBuff;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> filesles);
			Bool LoadFile(NotNullPtr<Text::String> fileName);
			Bool LoadData(NotNullPtr<IO::StreamData> jsonFD, NotNullPtr<IO::StreamData> binBuffFD);
		public:
			AVIRGLBViewerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGLBViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
