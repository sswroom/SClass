#ifndef _SM_SSWR_SHPCONV_SHPCONVADDFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVADDFILTERFORM
#include "DB/DBFFile.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvAddFilterForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblFilter;
			NotNullPtr<UI::GUIComboBox> cboFilter;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			DB::DBFFile *dbf;
			NotNullPtr<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			SHPConvAddFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, DB::DBFFile *dbf, NotNullPtr<Media::DrawEngine> deng);
			virtual ~SHPConvAddFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	};
};
#endif
