#ifndef _SM_SSWR_COLORDEMO_COLORDEMOFORM
#define _SM_SSWR_COLORDEMO_COLORDEMOFORM
#include "Media/RGBColorFilter.h"
#include "Parser/ParserList.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITrackBar.h"

namespace SSWR
{
	namespace ColorDemo
	{
		class ColorDemoForm : public UI::GUIForm
		{
		private:
			NN<Parser::ParserList> parsers;
			Optional<Media::StaticImage> currImage;
			Optional<Media::StaticImage> currPrevImage;
			Optional<Media::StaticImage> currDispImage;
			NN<Media::RGBColorFilter> rgbFilter;
			Double currValue;
			NN<Media::ColorManager> colorMgr;
			NN<Media::ColorManagerSess> colorSess;
			NN<UI::GUIPictureBoxDD> pbMain;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblValue;
//			NN<UI::GUITrackBar> tbValue;
			NN<UI::GUIHScrollBar> hsbValue;

			static void __stdcall FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnValueChanged(AnyType userObj, UIntOS scrollPos);
			static void __stdcall OnPBResized(AnyType userObj);

			void CreatePrevImage();
			void UpdatePrevImage();
		public:
			ColorDemoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::ColorManager> colorMgr);
			virtual ~ColorDemoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
