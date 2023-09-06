#ifndef _SM_SSWR_AVIREAD_AVIRIMAGERESIZEFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGERESIZEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageResizeForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<Media::StaticImage> srcImg;
			Media::StaticImage *outImg;

			UI::GUILabel *lblOriW;
			UI::GUITextBox *txtOriW;
			UI::GUILabel *lblOriH;
			UI::GUITextBox *txtOriH;
			UI::GUILabel *lblOutW;
			UI::GUITextBox *txtOutW;
			UI::GUILabel *lblOutH;
			UI::GUITextBox *txtOutH;
			UI::GUILabel *lblNTap;
			UI::GUITextBox *txtNTap;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRImageResizeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::Image *srcImg);
			virtual ~AVIRImageResizeForm();

			virtual void OnMonitorChanged();

			Media::StaticImage *GetNewImage();
		};
	};
};
#endif
