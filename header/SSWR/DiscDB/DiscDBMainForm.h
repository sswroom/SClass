#ifndef _SM_SSWR_DISCDB_DISCDBMAINFORM
#define _SM_SSWR_DISCDB_DISCDBMAINFORM
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "UI/GUIForm.h"
#include "UI/GUIMainMenu.h"

namespace SSWR
{
	namespace DiscDB
	{
		class DiscDBMainForm : public UI::GUIForm
		{
		private:
			SSWR::DiscDB::DiscDBEnv *env;
			UI::GUIMainMenu *mnuMain;

			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

		public:
			DiscDBMainForm(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, SSWR::DiscDB::DiscDBEnv *env);
			virtual ~DiscDBMainForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
