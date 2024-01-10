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

			NotNullPtr<UI::GUILabel> lblOriW;
			NotNullPtr<UI::GUITextBox> txtOriW;
			NotNullPtr<UI::GUILabel> lblOriH;
			NotNullPtr<UI::GUITextBox> txtOriH;
			NotNullPtr<UI::GUILabel> lblOutW;
			NotNullPtr<UI::GUITextBox> txtOutW;
			NotNullPtr<UI::GUILabel> lblOutH;
			NotNullPtr<UI::GUITextBox> txtOutH;
			NotNullPtr<UI::GUILabel> lblNTap;
			NotNullPtr<UI::GUITextBox> txtNTap;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRImageResizeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::RasterImage *srcImg);
			virtual ~AVIRImageResizeForm();

			virtual void OnMonitorChanged();

			Media::StaticImage *GetNewImage();
		};
	};
};
#endif
