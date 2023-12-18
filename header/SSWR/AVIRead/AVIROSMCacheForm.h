#ifndef _SM_SSWR_AVIREAD_AVIROSMCACHEFORM
#define _SM_SSWR_AVIREAD_AVIROSMCACHEFORM
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROSMCacheForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::WebServer::WebListener> listener;
			NotNullPtr<Map::OSM::OSMCacheHandler> hdlr;

			NotNullPtr<UI::GUILabel> lblReqCnt;
			UI::GUITextBox *txtReqCnt;
			NotNullPtr<UI::GUILabel> lblRemoteSuccCnt;
			UI::GUITextBox *txtRemoteSuccCnt;
			NotNullPtr<UI::GUILabel> lblRemoteErrCnt;
			UI::GUITextBox *txtRemoteErrCnt;
			NotNullPtr<UI::GUILabel> lblLocalCnt;
			UI::GUITextBox *txtLocalCnt;
			NotNullPtr<UI::GUILabel> lblCacheCnt;
			UI::GUITextBox *txtCacheCnt;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIROSMCacheForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Net::WebServer::WebListener> listener, NotNullPtr<Map::OSM::OSMCacheHandler> hdlr);
			virtual ~AVIROSMCacheForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
