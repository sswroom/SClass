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
			NN<UI::GUIListBox> lbFilters;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnDelete;
			DB::DBFFile *dbf;
			NN<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			Data::ArrayList<MapFilter*> *filters;

			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnOkClicked(AnyType userObj);
		public:
			SHPConvCurrFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, DB::DBFFile *dbf, Data::ArrayList<MapFilter*> *filters, NN<Media::DrawEngine> deng);
			virtual ~SHPConvCurrFilterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
