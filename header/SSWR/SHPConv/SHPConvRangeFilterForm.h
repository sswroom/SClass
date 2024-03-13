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
			NotNullPtr<UI::GUIRadioButton> radInRange;
			NotNullPtr<UI::GUIRadioButton> radOutRange;
			NotNullPtr<UI::GUILabel> lblX1;
			NotNullPtr<UI::GUITextBox> txtX1;
			NotNullPtr<UI::GUILabel> lblY1;
			NotNullPtr<UI::GUITextBox> txtY1;
			NotNullPtr<UI::GUIPictureBoxSimple> pbBox;
			NotNullPtr<UI::GUILabel> lblX2;
			NotNullPtr<UI::GUITextBox> txtX2;
			NotNullPtr<UI::GUILabel> lblY2;
			NotNullPtr<UI::GUITextBox> txtY2;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			SHPConvRangeFilterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<Media::DrawEngine> deng);
			virtual ~SHPConvRangeFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	};
};
#endif
