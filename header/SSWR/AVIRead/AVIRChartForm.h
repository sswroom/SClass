#ifndef _SM_SSWR_AVIREAD_AVIRCHARTFORM
#define _SM_SSWR_AVIREAD_AVIRCHARTFORM
#include "Data/Chart.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIPictureBoxSimple.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRChartForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPictureBoxSimple> pbMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::Chart *chart;

			static void __stdcall OnSizeChanged(void *userObj);
		public:
			AVIRChartForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Data::Chart *chart);
			virtual ~AVIRChartForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
