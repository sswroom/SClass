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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::CodeImageGen::CodeImageGen *codeImgGen;
			Media::StaticImage *simg;

			UI::GUIPictureBoxDD *pbMain;
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUILabel> lblCodeType;
			NotNullPtr<UI::GUIComboBox> cboCodeType;
			NotNullPtr<UI::GUILabel> lblCodeSize;
			UI::GUITextBox *txtCodeSize;
			NotNullPtr<UI::GUILabel> lblCode;
			UI::GUITextBox *txtCode;
			NotNullPtr<UI::GUILabel> lblCodeInfo;
			NotNullPtr<UI::GUIButton> btnCodeGen;

			static void __stdcall OnCodeTypeChanged(void *userObj);
			static void __stdcall OnCodeGenClicked(void *userObj);
		public:
			AVIRCodeImageGenForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCodeImageGenForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
