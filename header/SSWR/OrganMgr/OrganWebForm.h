#ifndef _SM_SSWR_ORGANMGR_ORGANWEBFORM
#define _SM_SSWR_ORGANMGR_ORGANWEBFORM
#include "Net/SocketFactory.h"
#include "SSWR/OrganMgr/OrganWebEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganWebForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIButton> btnReload;

			NotNullPtr<Net::SocketFactory> sockf;
			IO::LogTool log;
			OrganWebEnv *env;
			
			static void __stdcall OnReloadClicked(void *userObj);
		public:
			OrganWebForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<Media::DrawEngine> eng);
			virtual ~OrganWebForm();

			Bool IsError();
		};
	}
}
#endif