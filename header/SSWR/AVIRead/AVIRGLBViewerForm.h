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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlBinBuff;
			NN<UI::GUIVSplitter> vspMain;
			NN<UI::GUIPanel> pnlJSON;
			NN<UI::GUILabel> lblJSON;
			NN<UI::GUITextBox> txtJSON;
			NN<UI::GUILabel> lblBinBuff;
			NN<UI::GUIHexFileView> hfvBinBuff;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> filesles);
			Bool LoadFile(NN<Text::String> fileName);
			Bool LoadData(NN<IO::StreamData> jsonFD, NN<IO::StreamData> binBuffFD);
		public:
			AVIRGLBViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGLBViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
