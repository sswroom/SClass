#ifndef _SM_SSWR_AVIREAD_AVIRCODEIMAGEGENFORM
#define _SM_SSWR_AVIREAD_AVIRCODEIMAGEGENFORM
#include "Media/CodeImageGen/CodeImageGen.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCodeImageGenForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			Media::CodeImageGen::CodeImageGen *codeImgGen;
			Media::StaticImage *simg;

			NN<UI::GUIPictureBoxDD> pbMain;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblCodeType;
			NN<UI::GUIComboBox> cboCodeType;
			NN<UI::GUILabel> lblCodeSize;
			NN<UI::GUITextBox> txtCodeSize;
			NN<UI::GUILabel> lblCode;
			NN<UI::GUITextBox> txtCode;
			NN<UI::GUILabel> lblCodeInfo;
			NN<UI::GUIButton> btnCodeGen;

			static void __stdcall OnCodeTypeChanged(AnyType userObj);
			static void __stdcall OnCodeGenClicked(AnyType userObj);
		public:
			AVIRCodeImageGenForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCodeImageGenForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
