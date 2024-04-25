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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUIMainMenu> mnuMain;
			Optional<Net::SSLEngine> ssl;

		private:
			static void __stdcall FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		public:
			AVIRBaseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBaseForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
