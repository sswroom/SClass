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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<Media::StaticImage> srcImg;
			Media::StaticImage *outImg;

			NN<UI::GUILabel> lblOriW;
			NN<UI::GUITextBox> txtOriW;
			NN<UI::GUILabel> lblOriH;
			NN<UI::GUITextBox> txtOriH;
			NN<UI::GUILabel> lblOutW;
			NN<UI::GUITextBox> txtOutW;
			NN<UI::GUILabel> lblOutH;
			NN<UI::GUITextBox> txtOutH;
			NN<UI::GUILabel> lblNTap;
			NN<UI::GUITextBox> txtNTap;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRImageResizeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::RasterImage> srcImg);
			virtual ~AVIRImageResizeForm();

			virtual void OnMonitorChanged();

			Optional<Media::StaticImage> GetNewImage();
		};
	};
};
#endif
