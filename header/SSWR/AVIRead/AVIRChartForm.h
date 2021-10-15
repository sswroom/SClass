#ifndef _SM_SSWR_AVIREAD_AVIRCHARTFORM
#define _SM_SSWR_AVIREAD_AVIRCHARTFORM
#include "Data/IChart.h"
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
			UI::GUIPictureBoxSimple *pbMain;

			SSWR::AVIRead::AVIRCore *core;
			Data::IChart *chart;

			static void __stdcall OnSizeChanged(void *userObj);
		public:
			AVIRChartForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Data::IChart *chart);
			virtual ~AVIRChartForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
