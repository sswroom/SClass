#ifndef _SM_SSWR_SHPCONV_SHPCONVRANGEFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVRANGEFILTERFORM
#include "DB/DBFFile.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvRangeFilterForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIRadioButton> radInRange;
			NN<UI::GUIRadioButton> radOutRange;
			NN<UI::GUILabel> lblX1;
			NN<UI::GUITextBox> txtX1;
			NN<UI::GUILabel> lblY1;
			NN<UI::GUITextBox> txtY1;
			NN<UI::GUIPictureBoxSimple> pbBox;
			NN<UI::GUILabel> lblX2;
			NN<UI::GUITextBox> txtX2;
			NN<UI::GUILabel> lblY2;
			NN<UI::GUITextBox> txtY2;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			SHPConvRangeFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::DrawEngine> deng);
			virtual ~SHPConvRangeFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	}
}
#endif
