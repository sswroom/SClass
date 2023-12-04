#ifndef _SM_SSWR_AVIREAD_AVIRBASEFORM
#define _SM_SSWR_AVIREAD_AVIRBASEFORM
#include "Net/SSLEngine.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIMainMenu.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBaseForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UI::GUIMainMenu *mnuMain;
			Optional<Net::SSLEngine> ssl;

		private:
			static void __stdcall FileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
		public:
			AVIRBaseForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBaseForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
