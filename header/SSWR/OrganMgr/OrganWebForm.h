#ifndef _SM_SSWR_ORGANMGR_ORGANWEBFORM
#define _SM_SSWR_ORGANMGR_ORGANWEBFORM
#include "Net/SocketFactory.h"
#include "Net/TCPClientFactory.h"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganWebForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIButton> btnReload;

			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			IO::LogTool log;
			OrganEnv *env;
			
			static void __stdcall OnReloadClicked(AnyType userObj);
		public:
			OrganWebForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::DrawEngine> eng);
			virtual ~OrganWebForm();

			Bool IsError();
		};
	}
}
#endif