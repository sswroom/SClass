#ifndef _SM_SSWR_SHPCONV_SHPCONVCURRFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVCURRFILTERFORM
#include "DB/DBFFile.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvCurrFilterForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbFilters;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnAdd;
			UI::GUIButton *btnDelete;
			DB::DBFFile *dbf;
			Media::DrawEngine *deng;
			Media::MonitorMgr *monMgr;
			Data::ArrayList<MapFilter*> *filters;

			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnDeleteClicked(void *userObj);
			static void __stdcall OnOkClicked(void *userObj);
		public:
			SHPConvCurrFilterForm(UI::GUIClientControl *parent, UI::GUICore *ui, DB::DBFFile *dbf, Data::ArrayList<MapFilter*> *filters, Media::DrawEngine *deng);
			virtual ~SHPConvCurrFilterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
