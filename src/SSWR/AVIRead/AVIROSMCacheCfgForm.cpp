#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIROSMCacheCfgForm.h"
#include "SSWR/AVIRead/AVIROSMCacheForm.h"

void __stdcall SSWR::AVIRead::AVIROSMCacheCfgForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROSMCacheCfgForm> me = userObj.GetNN<SSWR::AVIRead::AVIROSMCacheCfgForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHTTPPort->GetText(sb);
	if (!sb.ToUInt16(port) || port <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid HTTP port number"), CSTR("Error"), me);
		return;
	}
	NN<Map::OSM::OSMCacheHandler> hdlr;
	UIntOS selItem = me->cboMapType->GetSelectedIndex();
	switch (selItem)
	{
	default:
	case 0://MNU_OSM_TILE:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.openstreetmap.org/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
		break;
	case 1: //MNU_OSM_CYCLE:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOpenCycleMap"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/cycle/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/cycle/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/cycle/"));
		break;
	case 2: //MNU_OSM_TRANSP:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransport"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/transport/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport/"));
		break;
	case 3: //MNU_OSM_MAPQUEST:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMMapQuest"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://otile1.mqcdn.com/tiles/1.0.0/osm/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"));
		hdlr->AddAlternateURL(CSTR("http://otile3.mqcdn.com/tiles/1.0.0/osm/"));
		hdlr->AddAlternateURL(CSTR("http://otile4.mqcdn.com/tiles/1.0.0/osm/"));
		break;
	case 4: //MNU_OSM_LANDSCAPE:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMLandscape"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/landscape/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/landscape/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/landscape/"));
		break;
	case 5: //MNU_OSM_OUTDOORS:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOutdoors"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/outdoors/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/outdoors/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/outdoors/"));
		break;
	case 6: //MNU_OSM_TRANSP_DARK:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransportDark"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/transport-dark/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport-dark/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport-dark/"));
		break;
	case 7: //MNU_OSM_SPINAL:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMSpinalMap"));
		NEW_CLASSNN(hdlr, Map::OSM::OSMCacheHandler(CSTR("http://a.tile.thunderforest.com/spinal-map/"), CSTRP(sbuff, sptr), 18, me->core->GetTCPClientFactory(), me->ssl));
		hdlr->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/spinal-map/"));
		hdlr->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/spinal-map/"));
		break;
	}
	NN<Net::WebServer::WebListener> listener;
	NEW_CLASSNN(listener, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), nullptr, hdlr, port, 120, 1, 8, CSTR("SSWR_OSM_Cache/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (listener->IsError())
	{
		listener.Delete();
		hdlr.Delete();
		me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
		return;
	}

	NN<SSWR::AVIRead::AVIROSMCacheForm> frm;
	NEW_CLASSNN(frm, SSWR::AVIRead::AVIROSMCacheForm(nullptr, me->ui, me->core, listener, hdlr));
	me->core->ShowForm(frm);
	me->Close();
}

void __stdcall SSWR::AVIRead::AVIROSMCacheCfgForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROSMCacheCfgForm> me = userObj.GetNN<SSWR::AVIRead::AVIROSMCacheCfgForm>();
	me->Close();
}

SSWR::AVIRead::AVIROSMCacheCfgForm::AVIROSMCacheCfgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 256, 128, ui)
{
	this->SetText(CSTR("OSM Cache Setting"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->lblMapType = ui->NewLabel(*this, CSTR("Map Type"));
	this->lblMapType->SetRect(8, 8, 100, 23, false);
	this->cboMapType = ui->NewComboBox(*this, false);
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

	this->lblHTTPPort = ui->NewLabel(*this, CSTR("HTTP Port"));
	this->lblHTTPPort->SetRect(8, 32, 100, 23, false);
	this->txtHTTPPort = ui->NewTextBox(*this, CSTR("80"));
	this->txtHTTPPort->SetRect(108, 32, 100, 23, false);

	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(49, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(132, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIROSMCacheCfgForm::~AVIROSMCacheCfgForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIROSMCacheCfgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
