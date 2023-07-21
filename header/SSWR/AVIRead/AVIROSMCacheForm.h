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
			SSWR::AVIRead::AVIRCore *core;
			Net::WebServer::WebListener *listener;
			Map::OSM::OSMCacheHandler *hdlr;

			UI::GUILabel *lblReqCnt;
			UI::GUITextBox *txtReqCnt;
			UI::GUILabel *lblRemoteSuccCnt;
			UI::GUITextBox *txtRemoteSuccCnt;
			UI::GUILabel *lblRemoteErrCnt;
			UI::GUITextBox *txtRemoteErrCnt;
			UI::GUILabel *lblLocalCnt;
			UI::GUITextBox *txtLocalCnt;
			UI::GUILabel *lblCacheCnt;
			UI::GUITextBox *txtCacheCnt;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIROSMCacheForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, Net::WebServer::WebListener *listener, Map::OSM::OSMCacheHandler *hdlr);
			virtual ~AVIROSMCacheForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
