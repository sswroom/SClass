#ifndef _SM_SSWR_AVIREAD_AVIRFONTRENDERERFORM
#define _SM_SSWR_AVIREAD_AVIRFONTRENDERERFORM
#include "Media/FontRenderer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFontRendererForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpChar;
			NN<UI::GUIListBox> lbChar;
			NN<UI::GUIHSplitter> hspChar;
			NN<UI::GUIPictureBoxDD> pbChar;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::FontRenderer> font;
			Optional<Media::StaticImage> currImg;

			static void __stdcall OnCharSelChg(AnyType userObj);
		public:
			AVIRFontRendererForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::FontRenderer> font);
			virtual ~AVIRFontRendererForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
