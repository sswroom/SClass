#ifndef _SM_SSWR_COLORDEMO_COLORDEMOFORM
#define _SM_SSWR_COLORDEMO_COLORDEMOFORM
#include "Media/RGBColorFilter.h"
#include "Parser/ParserList.h"
#include "UI/MSWindowForm.h"
#include "UI/MSWindowHScrollBar.h"
#include "UI/MSWindowLabel.h"
#include "UI/MSWindowPanel.h"
#include "UI/MSWindowPictureBoxDD.h"
#include "UI/MSWindowTrackBar.h"

namespace SSWR
{
	namespace ColorDemo
	{
		class ColorDemoForm : public UI::MSWindowForm
		{
		private:
			Parser::ParserList *parsers;
			Media::StaticImage *currImage;
			Media::StaticImage *currPrevImage;
			Media::StaticImage *currDispImage;
			Media::RGBColorFilter *rgbFilter;
			Double currValue;
			Media::ColorManager *colorMgr;
			Media::ColorManagerSess *colorSess;
			UI::MSWindowPictureBoxDD *pbMain;
			UI::MSWindowPanel *pnlMain;
			UI::MSWindowLabel *lblValue;
//			UI::MSWindowTrackBar *tbValue;
			UI::MSWindowHScrollBar *hsbValue;

			static void __stdcall FileHandler(void *userObj, const WChar **files, OSInt nFiles);
			static void __stdcall OnValueChanged(void *userObj, Int32 scrollPos);
			static void __stdcall OnPBResized(void *userObj);

			void CreatePrevImage();
			void UpdatePrevImage();
		public:
			ColorDemoForm(void *hInst, UI::MSWindowClientControl *parent, UI::MSWindowUI *ui, Media::ColorManager *colorMgr);
			virtual ~ColorDemoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
