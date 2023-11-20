#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIROSMCacheCfgForm.h"
#include "SSWR/AVIRead/AVIROSMCacheForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIROSMCacheCfgForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIROSMCacheCfgForm *me = (SSWR::AVIRead::AVIROSMCacheCfgForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHTTPPort->GetText(sb);
	if (!sb.ToUInt16(port) || port <= 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter valid HTTP port number"), CSTR("Error"), me);
		return;
	}
	NotNullPtr<Map::OSM::OSMCacheHandler> hdlr;
	UOSInt selItem = me->cboMapType->GetSelectedIndex();
	switch (selItem)
	{
	default:
	case 0://MNU_OSM_TILE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.openstreetmap.org/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
		break;
	case 1: //MNU_OSM_CYCLE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOpenCycleMap"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/cycle/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/cycle/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/cycle/"));
		break;
	case 2: //MNU_OSM_TRANSP:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransport"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/transport/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport/"));
		break;
	case 3: //MNU_OSM_MAPQUEST:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMMapQuest"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://otile1.mqcdn.com/tiles/1.0.0/osm/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"));
		hdlr->AddAlternateURL(CSTR("http://otile3.mqcdn.com/tiles/1.0.0/osm/"));
		hdlr->AddAlternateURL(CSTR("http://otile4.mqcdn.com/tiles/1.0.0/osm/"));
		break;
	case 4: //MNU_OSM_LANDSCAPE:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMLandscape"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/landscape/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/landscape/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/landscape/"));
		break;
	case 5: //MNU_OSM_OUTDOORS:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOutdoors"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/outdoors/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/outdoors/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/outdoors/"));
		break;
	case 6: //MNU_OSM_TRANSP_DARK:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransportDark"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/transport-dark/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport-dark/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport-dark/"));
		break;
	case 7: //MNU_OSM_SPINAL:
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMSpinalMap"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/spinal-map/"), CSTRP(sbuff, sptr), 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/spinal-map/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/spinal-map/"));
		break;
	}
	NotNullPtr<Net::WebServer::WebListener> listener;
	NEW_CLASSNN(listener, Net::WebServer::WebListener(me->core->GetSocketFactory(), 0, hdlr, port, 120, 8, CSTR("SSWR_OSM_Cache/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (listener->IsError())
	{
		listener.Delete();
		hdlr->Release();
		UI::MessageDialog::ShowDialog(CSTR("Error in listening to the port"), CSTR("Error"), me);
		return;
	}

	SSWR::AVIRead::AVIROSMCacheForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIROSMCacheForm(0, me->ui, me->core, listener, hdlr));
	me->core->ShowForm(frm);
	me->Close();
}

void __stdcall SSWR::AVIRead::AVIROSMCacheCfgForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIROSMCacheCfgForm *me = (SSWR::AVIRead::AVIROSMCacheCfgForm*)userObj;
	me->Close();
}

SSWR::AVIRead::AVIROSMCacheCfgForm::AVIROSMCacheCfgForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 256, 128, ui)
{
	this->SetText(CSTR("OSM Cache Setting"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->lblMapType, UI::GUILabel(ui, *this, CSTR("Map Type")));
	this->lblMapType->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboMapType, UI::GUIComboBox(ui, *this, false));
	this->cboMapType->SetRect(108, 8, 100, 23, false);
	this->cboMapType->AddItem(CSTR("OSM Tile"), (void*)0);
	this->cboMapType->AddItem(CSTR("Open Cycle Map"), (void*)1);
	this->cboMapType->AddItem(CSTR("Transport"), (void*)2);
	this->cboMapType->AddItem(CSTR("MapQuest"), (void*)3);
	this->cboMapType->AddItem(CSTR("Landscape"), (void*)4);
	this->cboMapType->AddItem(CSTR("Outdoors"), (void*)5);
	this->cboMapType->AddItem(CSTR("Transport Dark"), (void*)6);
	this->cboMapType->AddItem(CSTR("Spinal Map"), (void*)7);
	this->cboMapType->SetSelectedIndex(0);

	NEW_CLASS(this->lblHTTPPort, UI::GUILabel(ui, *this, CSTR("HTTP Port")));
	this->lblHTTPPort->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtHTTPPort, UI::GUITextBox(ui, *this, CSTR("80")));
	this->txtHTTPPort->SetRect(108, 32, 100, 23, false);

	NEW_CLASS(this->btnOK, UI::GUIButton(ui, *this, CSTR("&OK")));
	this->btnOK->SetRect(49, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, *this, CSTR("&Cancel")));
	this->btnCancel->SetRect(132, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIROSMCacheCfgForm::~AVIROSMCacheCfgForm()
{
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIROSMCacheCfgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
