#ifndef _SM_SSWR_AVIREAD_AVIRSNMPWALKFORM
#define _SM_SSWR_AVIREAD_AVIRSNMPWALKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNMPWalkForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblAgent;
			UI::GUITextBox *txtAgent;
			UI::GUIListView *lvResults;

		public:
			AVIRSNMPWalkForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, const Net::SocketUtil::AddressInfo *addr, NotNullPtr<Text::String> community);
			virtual ~AVIRSNMPWalkForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
