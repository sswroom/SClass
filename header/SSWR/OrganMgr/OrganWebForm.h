#ifndef _SM_SSWR_ORGANMGR_ORGANWEBFORM
#define _SM_SSWR_ORGANMGR_ORGANWEBFORM
#include "Net/SocketFactory.h"
#include "SSWR/OrganMgr/OrganWebHandler.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganWebForm : public UI::GUIForm
		{
		private:
			UI::GUIButton *btnReload;

			Net::SocketFactory *sockf;
			IO::LogTool *log;
			OrganWebHandler *dataHdlr;
			
			static void __stdcall OnReloadClicked(void *userObj);
		public:
			OrganWebForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::DrawEngine *eng);
			virtual ~OrganWebForm();

			Bool IsError();
		};
	}
}
#endif