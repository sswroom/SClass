#ifndef _SM_SSWR_AVIREAD_AVIRBYDC9RFORM
#define _SM_SSWR_AVIREAD_AVIRBYDC9RFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBYDC9RForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

		public:
			AVIRBYDC9RForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBYDC9RForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
