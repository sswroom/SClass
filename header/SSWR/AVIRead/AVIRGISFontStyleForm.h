#ifndef _SM_SSWR_AVIREAD_AVIRGISFONTSTYLEFORM
#define _SM_SSWR_AVIREAD_AVIRGISFONTSTYLEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRFontSelector.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIHScrollBar.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISFontStyleForm : public UI::GUIForm, public Media::IColorHandler
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Map::MapEnv> env;
			UOSInt fontStyle;
			Bool changed;
			NN<Media::DrawEngine> eng;

			NN<UI::GUIPanel> pnlBtn;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<UI::GUIGroupBox> grpStyle;
			NN<UI::GUIPanel> pnlStyle;
			NN<UI::GUIButton> btnAddStyle;
			NN<UI::GUIButton> btnRemoveStyle;
			NN<UI::GUIButton> btnEditStyle;
			SSWR::AVIRead::AVIRFontSelector *fsFonts;

			static void __stdcall AddStyleClicked(AnyType userObj);
			static void __stdcall RemoveStyleClicked(AnyType userObj);
			static void __stdcall EditStyleClicked(AnyType userObj);
			static void __stdcall FontsSelectedChg(AnyType userObj);
			static void __stdcall FontsDoubleClicked(AnyType userObj);
			static void __stdcall OKClicked(AnyType userObj);
			static void __stdcall CancelClicked(AnyType userObj);

			void UpdatePreview();
		public:
			AVIRGISFontStyleForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UOSInt fontStyle);
			virtual ~AVIRGISFontStyleForm();

			virtual void OnMonitorChanged();

			virtual void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);

			UOSInt GetFontStyle();
			Bool IsChanged();
		};
	}
}
#endif
