#ifndef _SM_SSWR_SHPCONV_SHPCONVDBFVIEWFORM
#define _SM_SSWR_SHPCONV_SHPCONVDBFVIEWFORM
#include "DB/DBFFile.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/SHPConvEng.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvDBFViewForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIListView> lvDBF;
			Media::MonitorMgr *monMgr;

		public:
			SHPConvDBFViewForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<DB::DBFFile> dbf, NN<SSWR::SHPConv::SHPConvEng> eng, Text::CStringNN lbl);
			virtual ~SHPConvDBFViewForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
