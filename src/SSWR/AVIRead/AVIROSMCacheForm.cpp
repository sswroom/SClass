#include "Stdafx.h"
#include "SSWR/AVIRead/AVIROSMCacheForm.h"

void __stdcall SSWR::AVIRead::AVIROSMCacheForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIROSMCacheForm *me = (SSWR::AVIRead::AVIROSMCacheForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Map::OSM::OSMCacheHandler::CacheStatus status;
	me->hdlr->GetStatus(&status);
	sptr = Text::StrInt32(sbuff, status.reqCnt);
	me->txtReqCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, status.remoteSuccCnt);
	me->txtRemoteSuccCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, status.remoteErrCnt);
	me->txtRemoteErrCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, status.localCnt);
	me->txtLocalCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrInt32(sbuff, status.cacheCnt);
	me->txtCacheCnt->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIROSMCacheForm::AVIROSMCacheForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Net::WebServer::WebListener> listener, NotNullPtr<Map::OSM::OSMCacheHandler> hdlr) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("OSM Cache"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->listener = listener;
	this->hdlr = hdlr;

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblReqCnt, UI::GUILabel(ui, *this, CSTR("Request Cnt")));
	this->lblReqCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtReqCnt, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtReqCnt->SetRect(104, 4, 150, 23, false);
	this->txtReqCnt->SetReadOnly(true);
	NEW_CLASS(this->lblRemoteSuccCnt, UI::GUILabel(ui, *this, CSTR("Remote Succ")));
	this->lblRemoteSuccCnt->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtRemoteSuccCnt, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtRemoteSuccCnt->SetRect(104, 28, 150, 23, false);
	this->txtRemoteSuccCnt->SetReadOnly(true);
	NEW_CLASS(this->lblRemoteErrCnt, UI::GUILabel(ui, *this, CSTR("Remote Err")));
	this->lblRemoteErrCnt->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtRemoteErrCnt, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtRemoteErrCnt->SetRect(104, 52, 150, 23, false);
	this->txtRemoteErrCnt->SetReadOnly(true);
	NEW_CLASS(this->lblLocalCnt, UI::GUILabel(ui, *this, CSTR("Local Cnt")));
	this->lblLocalCnt->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtLocalCnt, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtLocalCnt->SetRect(104, 76, 150, 23, false);
	this->txtLocalCnt->SetReadOnly(true);
	NEW_CLASS(this->lblCacheCnt, UI::GUILabel(ui, *this, CSTR("Cache Cnt")));
	this->lblCacheCnt->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtCacheCnt, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtCacheCnt->SetRect(104, 100, 150, 23, false);
	this->txtCacheCnt->SetReadOnly(true);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIROSMCacheForm::~AVIROSMCacheForm()
{
	this->listener.Delete();
	this->hdlr->Release();
}

void SSWR::AVIRead::AVIROSMCacheForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
