#ifndef _SM_SSWR_SHPCONV_SHPCONVDBFVIEWFORM
#define _SM_SSWR_SHPCONV_SHPCONVDBFVIEWFORM
#include "DB/DBFFile.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/IMapEng.h"
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
			UI::GUIListView *lvDBF;
			Media::MonitorMgr *monMgr;

		public:
			SHPConvDBFViewForm(UI::GUIClientControl *parent, UI::GUICore *ui, DB::DBFFile *dbf, SSWR::SHPConv::IMapEng *eng, Text::CString lbl);
			virtual ~SHPConvDBFViewForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
