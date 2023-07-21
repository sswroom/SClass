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
			UI::GUIRadioButton *radInRange;
			UI::GUIRadioButton *radOutRange;
			UI::GUILabel *lblX1;
			UI::GUITextBox *txtX1;
			UI::GUILabel *lblY1;
			UI::GUITextBox *txtY1;
			UI::GUIPictureBoxSimple *pbBox;
			UI::GUILabel *lblX2;
			UI::GUITextBox *txtX2;
			UI::GUILabel *lblY2;
			UI::GUITextBox *txtY2;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			NotNullPtr<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			SHPConvRangeFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<Media::DrawEngine> deng);
			virtual ~SHPConvRangeFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	};
};
#endif
