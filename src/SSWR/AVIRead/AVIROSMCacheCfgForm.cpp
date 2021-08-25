#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIROSMCacheCfgForm.h"
#include "SSWR/AVIRead/AVIROSMCacheForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIROSMCacheCfgForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIROSMCacheCfgForm *me = (SSWR::AVIRead::AVIROSMCacheCfgForm*)userObj;
	UTF8Char sbuff[512];
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtHTTPPort->GetText(&sb);
	if (!sb.ToUInt16(&port) || port <= 0 || port >= 65536)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid HTTP port number", (const UTF8Char *)"Error", me);
		return;
	}
	Map::OSM::OSMCacheHandler *hdlr = 0;
	UOSInt selItem = me->cboMapType->GetSelectedIndex();
	switch (selItem)
	{
	case 0://MNU_OSM_TILE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMTile");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.openstreetmap.org/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.openstreetmap.org/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.openstreetmap.org/");
		break;
	case 1: //MNU_OSM_CYCLE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMOpenCycleMap");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.thunderforest.com/cycle/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/cycle/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/cycle/");
		break;
	case 2: //MNU_OSM_TRANSP:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMTransport");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.thunderforest.com/transport/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/transport/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/transport/");
		break;
	case 3: //MNU_OSM_MAPQUEST:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMMapQuest");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://otile1.mqcdn.com/tiles/1.0.0/osm/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://otile2.mqcdn.com/tiles/1.0.0/osm/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://otile3.mqcdn.com/tiles/1.0.0/osm/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://otile4.mqcdn.com/tiles/1.0.0/osm/");
		break;
	case 4: //MNU_OSM_LANDSCAPE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMLandscape");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.thunderforest.com/landscape/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/landscape/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/landscape/");
		break;
	case 5: //MNU_OSM_OUTDOORS:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMOutdoors");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.thunderforest.com/outdoors/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/outdoors/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/outdoors/");
		break;
	case 6: //MNU_OSM_TRANSP_DARK:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMTransportDark");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.thunderforest.com/transport-dark/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/transport-dark/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/transport-dark/");
		break;
	case 7: //MNU_OSM_SPINAL:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"OSMSpinalMap");
		NEW_CLASS(hdlr, Map::OSM::OSMCacheHandler((const UTF8Char*)"http://a.tile.thunderforest.com/spinal-map/", sbuff, 18, me->core->GetSocketFactory(), me->ssl));
		hdlr->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/spinal-map/");
		hdlr->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/spinal-map/");
		break;
	}
	Net::WebServer::WebListener *listener;
	NEW_CLASS(listener, Net::WebServer::WebListener(me->core->GetSocketFactory(), 0, hdlr, port, 120, 8, (const UTF8Char*)"SSWR_OSM_Cache/1.0", false, true));
	if (listener->IsError())
	{
		DEL_CLASS(listener);
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in listening to the port", (const UTF8Char *)"Error", me);
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

SSWR::AVIRead::AVIROSMCacheCfgForm::AVIROSMCacheCfgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 256, 128, ui)
{
	this->SetText((const UTF8Char*)"OSM Cache Setting");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::DefaultSSLEngine::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->lblMapType, UI::GUILabel(ui, this, (const UTF8Char*)"Map Type"));
	this->lblMapType->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboMapType, UI::GUIComboBox(ui, this, false));
	this->cboMapType->SetRect(108, 8, 100, 23, false);
	this->cboMapType->AddItem((const UTF8Char*)"OSM Tile", (void*)0);
	this->cboMapType->AddItem((const UTF8Char*)"Open Cycle Map", (void*)1);
	this->cboMapType->AddItem((const UTF8Char*)"Transport", (void*)2);
	this->cboMapType->AddItem((const UTF8Char*)"MapQuest", (void*)3);
	this->cboMapType->AddItem((const UTF8Char*)"Landscape", (void*)4);
	this->cboMapType->AddItem((const UTF8Char*)"Outdoors", (void*)5);
	this->cboMapType->AddItem((const UTF8Char*)"Transport Dark", (void*)6);
	this->cboMapType->AddItem((const UTF8Char*)"Spinal Map", (void*)7);
	this->cboMapType->SetSelectedIndex(0);

	NEW_CLASS(this->lblHTTPPort, UI::GUILabel(ui, this, (const UTF8Char*)"HTTP Port"));
	this->lblHTTPPort->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtHTTPPort, UI::GUITextBox(ui, this, (const UTF8Char*)"80"));
	this->txtHTTPPort->SetRect(108, 32, 100, 23, false);

	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(49, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"&Cancel"));
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
