#ifndef _SM_SSWR_AVIREAD_AVIRGISIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRGISIMAGEFORM
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureList.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISImageForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::Resizer::LanczosResizerRGB_C8> resizer;
			NN<Map::MapEnv> env;
			NN<Parser::ParserList> parsers;
			NN<Media::ColorManagerSess> colorSess;
			UOSInt imgIndex;

			NN<UI::GUILabel> lbl;
			NN<UI::GUIPanel> pnlButtons;
			NN<UI::GUIPictureList> plIcons;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames);
			void UpdateImages();
		public:
			AVIRGISImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, UOSInt imgIndex);
			virtual ~AVIRGISImageForm();

			virtual void OnMonitorChanged();

			UOSInt GetImgIndex();
		};
	}
}
#endif
