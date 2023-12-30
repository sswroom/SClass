#ifndef _SM_SSWR_AVIREAD_AVIRTIMEDFILECOPYFORM
#define _SM_SSWR_AVIREAD_AVIRTIMEDFILECOPYFORM
#include "IO/ZIPMTBuilder.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIDateTimePicker.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTimedFileCopyForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUILabel> lblFileDir;
			NotNullPtr<UI::GUITextBox> txtFileDir;
			NotNullPtr<UI::GUILabel> lblStartTime;
			NotNullPtr<UI::GUIDateTimePicker> dtpStartTime;
			NotNullPtr<UI::GUILabel> lblEndTime;
			NotNullPtr<UI::GUIDateTimePicker> dtpEndTime;
			NotNullPtr<UI::GUIButton> btnStart;

			static void __stdcall OnStartClicked(void *userObj);

			Bool CopyToZip(IO::ZIPMTBuilder *zip, const UTF8Char *buffStart, const UTF8Char *pathBase, UTF8Char *pathEnd, NotNullPtr<Data::DateTime> startTime, NotNullPtr<Data::DateTime> endTime, Bool monthDir);
		public:
			AVIRTimedFileCopyForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimedFileCopyForm();

			Text::CStringNN GetFormName() const;
			virtual void OnMonitorChanged();
		};
	}
}
#endif