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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpChar;
			UI::GUIListBox *lbChar;
			NotNullPtr<UI::GUIHSplitter> hspChar;
			UI::GUIPictureBoxDD *pbChar;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::FontRenderer *font;
			Media::StaticImage *currImg;

			static void __stdcall OnCharSelChg(void *userObj);
		public:
			AVIRFontRendererForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::FontRenderer *font);
			virtual ~AVIRFontRendererForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
