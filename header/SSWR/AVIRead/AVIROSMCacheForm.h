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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::WebServer::WebListener> listener;
			NN<Map::OSM::OSMCacheHandler> hdlr;

			NN<UI::GUILabel> lblReqCnt;
			NN<UI::GUITextBox> txtReqCnt;
			NN<UI::GUILabel> lblRemoteSuccCnt;
			NN<UI::GUITextBox> txtRemoteSuccCnt;
			NN<UI::GUILabel> lblRemoteErrCnt;
			NN<UI::GUITextBox> txtRemoteErrCnt;
			NN<UI::GUILabel> lblLocalCnt;
			NN<UI::GUITextBox> txtLocalCnt;
			NN<UI::GUILabel> lblCacheCnt;
			NN<UI::GUITextBox> txtCacheCnt;

			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIROSMCacheForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Net::WebServer::WebListener> listener, NN<Map::OSM::OSMCacheHandler> hdlr);
			virtual ~AVIROSMCacheForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
