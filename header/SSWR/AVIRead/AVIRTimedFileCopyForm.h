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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUILabel> lblFileDir;
			NN<UI::GUITextBox> txtFileDir;
			NN<UI::GUILabel> lblStartTime;
			NN<UI::GUIDateTimePicker> dtpStartTime;
			NN<UI::GUILabel> lblEndTime;
			NN<UI::GUIDateTimePicker> dtpEndTime;
			NN<UI::GUIButton> btnStart;

			static void __stdcall OnStartClicked(AnyType userObj);

			Bool CopyToZip(IO::ZIPMTBuilder *zip, UnsafeArray<const UTF8Char> buffStart, UnsafeArray<const UTF8Char> pathBase, UnsafeArray<UTF8Char> pathEnd, NN<Data::DateTime> startTime, NN<Data::DateTime> endTime, Bool monthDir);
		public:
			AVIRTimedFileCopyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimedFileCopyForm();

			Text::CStringNN GetFormName() const;
			virtual void OnMonitorChanged();
		};
	}
}
#endif