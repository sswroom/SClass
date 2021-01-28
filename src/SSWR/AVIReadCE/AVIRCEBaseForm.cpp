#include "stdafx.h"
#include "Text/StringBuilder.h"
#include "Crypto/Hash/CRC32R.h"
#include "IO/DirectoryPackage.h"
#include "IO/MTKGPSNMEA.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Map/ESRI/ESRITileMap.h"
#include "Media/MediaFile.h"
#include "Media/Playlist.h"
#include "Net/URL.h"
/*
#include "SSWR/AVIRead/AVIRAboutForm.h"
#include "SSWR/AVIRead/AVIRAudioFilterForm.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRCodePageForm.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRDNSClientForm.h"
#include "SSWR/AVIRead/AVIRDNSProxyForm.h"
#include "SSWR/AVIRead/AVIREncryptForm.h"
#include "SSWR/AVIRead/AVIRESRIMapForm.h"
#include "SSWR/AVIRead/AVIRFileExForm.h"
#include "SSWR/AVIRead/AVIRFileHashForm.h"
#include "SSWR/AVIRead/AVIRGenImageForm.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "SSWR/AVIRead/AVIRHTTPClientForm.h"
#include "SSWR/AVIRead/AVIRHTTPProxyClientForm.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "SSWR/AVIRead/AVIRHTTPTestForm.h"
#include "SSWR/AVIRead/AVIRImageBatchForm.h"
#include "SSWR/AVIRead/AVIRLogBackupForm.h"
#include "SSWR/AVIRead/AVIRMPEGToolForm.h"
#include "SSWR/AVIRead/AVIRNetInfoForm.h"
#include "SSWR/AVIRead/AVIRNetPingForm.h"
#include "SSWR/AVIRead/AVIRNTPClientForm.h"
#include "SSWR/AVIRead/AVIRODBCForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIRProfiledResizerForm.h"
#include "SSWR/AVIRead/AVIRProtoDecForm.h"
#include "SSWR/AVIRead/AVIRProxyServerForm.h"
#include "SSWR/AVIRead/AVIRSelSerialPortForm.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "SSWR/AVIRead/AVIRSetDPIForm.h"
#include "SSWR/AVIRead/AVIRSMTPServerForm.h"
#include "SSWR/AVIRead/AVIRStreamConvForm.h"
#include "SSWR/AVIRead/AVIRStreamTermForm.h"
#include "SSWR/AVIRead/AVIRSudokuForm.h"
#include "SSWR/AVIRead/AVIRTCPTestForm.h"
#include "SSWR/AVIRead/AVIRTVControlForm.h"
#include "SSWR/AVIRead/AVIRUDPCaptureForm.h"
#include "SSWR/AVIRead/AVIRWHOISClientForm.h"
#include "SSWR/AVIRead/AVIRWIADevForm.h"*/
#include "SSWR/AVIReadCE/AVIRCEAboutForm.h"
#include "SSWR/AVIReadCE/AVIRCEBaseForm.h"
#include "SSWR/AVIReadCE/AVIRCEProcInfoForm.h"
#include "UI/MessageDialog.h"
#include "UI/GUITextBox.h"
#include "UtilUI/TextViewerForm.h"
#include <windows.h>

typedef enum
{
	MNU_MISC,
	MNU_HELP,
	MNU_ABOUT = 100,
	MNU_PROC_INFO,
	MNU_EXIT
} MenuItems;

void __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::FileHandler(void *userObj, const UTF8Char **files, OSInt nFiles)
{
	SSWR::AVIReadCE::AVIRCEBaseForm *me = (AVIReadCE::AVIRCEBaseForm*)userObj;
	IO::Path::PathType pt;
	IO::StmData::FileData *fd;
	IO::DirectoryPackage *pkg;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Cannot parse:");
	Bool found = false;
	me->core->BeginLoad();
	OSInt i = 0;
	while (i < nFiles)
	{
		pt = IO::Path::GetPathType(files[i]);
		if (pt == IO::Path::PT_DIRECTORY)
		{
			NEW_CLASS(pkg, IO::DirectoryPackage(files[i]));
			me->core->OpenObject(pkg);
		}
		else if (pt == IO::Path::PT_FILE)
		{
			NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
			if (!me->core->LoadData(fd, 0))
			{
				sb.Append((const UTF8Char*)"\n");
				sb.Append(files[i]);
				found = true;
			}
			DEL_CLASS(fd);
		}
		i++;
	}
	me->core->EndLoad();
	if (found)
	{
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Error", me);
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::OnCategoryChg(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEBaseForm *me = (SSWR::AVIReadCE::AVIRCEBaseForm*)userObj;
	Int32 item = (Int32)(OSInt)me->lbCategory->GetSelectedItem();
	Data::ArrayList<MenuInfo*> *menu = me->menuItems->Get(item);
	MenuInfo *info;
	OSInt i;
	OSInt j;
	me->lbContent->ClearItems();
	if (menu)
	{
		i = 0;
		j = menu->GetCount();
		while (i < j)
		{
			info = menu->GetItem(i);
			me->lbContent->AddItem(info->name, (void*)info->item);
			i++;
		}
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::OnContentClick(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEBaseForm *me = (SSWR::AVIReadCE::AVIRCEBaseForm*)userObj;
	Int32 item = (Int32)(OSInt)me->lbContent->GetSelectedItem();
	if (item)
	{
		me->EventMenuClicked((UInt16)item);
	}
}

SSWR::AVIReadCE::AVIRCEBaseForm::MenuInfo *__stdcall SSWR::AVIReadCE::AVIRCEBaseForm::NewMenuItem(const WChar *name, Int32 item)
{
	MenuInfo *info;
	info = MemAlloc(MenuInfo, 1);
	info->name = name;
	info->item = item;
	return info;
}

SSWR::AVIReadCE::AVIRCEBaseForm::AVIRCEBaseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 480, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"AVIRead");
	this->SetFont(0, 8.75, false);
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	Data::ArrayList<MenuInfo*> *menu;
	NEW_CLASS(this->menuItems, Data::Integer32Map<Data::ArrayList<MenuInfo*>*>());

	OSInt w;
	OSInt h;
	this->GetSize(&w, &h);
	NEW_CLASS(this->lbCategory, UI::GUIListBox(ui, this, true));
	this->lbCategory->SetRect(0, 0, (w >> 1) - 3, 23, false);
	this->lbCategory->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCategory->HandleSelectionChange(OnCategoryChg, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->lbContent, UI::GUIListBox(ui, this, true));
	this->lbContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbContent->HandleDoubleClicked(OnContentClick, this);
	
	NEW_CLASS(menu, Data::ArrayList<MenuInfo*>());
	menu->Add(NewMenuItem(L"Process Info", MNU_PROC_INFO));
	menu->Add(NewMenuItem(L"Exit", MNU_EXIT));
	this->menuItems->Put(MNU_MISC, menu);
	this->lbCategory->AddItem(L"Misc", (void*)MNU_MISC);
	NEW_CLASS(menu, Data::ArrayList<MenuInfo*>());
	menu->Add(NewMenuItem(L"About", MNU_ABOUT));
	this->menuItems->Put(MNU_HELP, menu);
	this->lbCategory->AddItem(L"Help", (void*)MNU_HELP);

/*	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(L"M&isc");
	mnu->AddItem(L"Open File", MNU_OPEN_FILE, MOD_CONTROL, 'O');
	mnu->AddItem(L"Open &ODBC", MNU_OPEN_ODBC, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"Stream Converter", MNU_STREAM_CONV, 0, 0);
	mnu->AddItem(L"Stream Terminal", MNU_STMTERM, 0, 0);
	mnu->AddItem(L"File Extractor", MNU_FILEEX, 0, 0);
	mnu->AddItem(L"File Hash", MNU_FILE_HASH, 0, 0);
	mnu->AddItem(L"Log Backup", MNU_LOG_BACKUP, 0, 0);
	mnu->AddItem(L"Text Viewer", MNU_TEXT_VIEWER, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"Sudoku Solver", MNU_SUDOKU_SOLVER, 0, 0);
	mnu->AddItem(L"Process Info", MNU_PROC_INFO, 0, 0);
	mnu->AddItem(L"Text Encrypt", MNU_TEXT_ENCRYPT, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"Protocol Decoder", MNU_PROTODEC, 0, 0);
	
	mnu = this->mnuMain->AddSubMenu(L"&Setting");
	mnu->AddItem(L"Set &CodePage", MNU_SET_CODEPAGE, 0, 0);
	mnu->AddItem(L"Set &Monitor Color", MNU_SET_COLOR, 0, 0);
	mnu->AddItem(L"Set &Audio Device", MNU_SET_AUDIO, 0, 0);
	mnu->AddItem(L"Set Monitor &DPI", MNU_SET_DPI, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"&Network");
	mnu->AddItem(L"Network Info", MNU_NET_INFO, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"HTTP Server", MNU_HTTP_SVR, 0, 0);
	mnu->AddItem(L"HTTP Client", MNU_HTTPCLIENT, 0, 0);
	mnu->AddItem(L"HTTP Proxy Client", MNU_HTTPPROXYCLIENT, 0, 0);
	mnu->AddItem(L"NTP Client", MNU_NTPCLIENT, 0, 0);
	mnu->AddItem(L"DNS Client", MNU_DNSCLIENT, 0, 0);
	mnu->AddItem(L"DNS Proxy", MNU_DNSPROXY, 0, 0);
	mnu->AddItem(L"SMTP Server", MNU_SMTPSERVER, 0, 0);
	mnu->AddItem(L"Ping", MNU_NET_PING, 0, 0);
	mnu->AddItem(L"UDP Capture", MNU_UDP_CAPTURE, 0, 0);
	mnu->AddItem(L"WHOIS Client", MNU_WHOISCLIENT, 0, 0);
//	mnu->AddItem(L"Proxy Server", MNU_PROXYSERVER, 0, 0);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu(L"Test");
	mnu2->AddItem(L"TCP Conn Test", MNU_TCPTEST, 0, 0);
	mnu2->AddItem(L"HTTP Test", MNU_HTTPTEST, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"M&edia");
	mnu->AddItem(L"&Generate Image", MNU_GEN_IMAGE, 0, 0);
	mnu->AddItem(L"Media Playlist", MNU_PLAYLIST, 0, 0);
	mnu->AddItem(L"MPEG Tool", MNU_MPEG_TOOL, 0, 0);
	mnu->AddItem(L"Audio Filter", MNU_AUDIOFILTER, 0, 0);
	mnu->AddItem(L"Image Batch Tool", MNU_IMAGE_BATCH, 0, 0);
	mnu->AddItem(L"Profiled Resizer", MNU_PROF_RESIZER, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"&Map");
	mnu->AddItem(L"Add ESRI Tile Map", MNU_ESRI_MAP, 0, 0);
	mnu2 = mnu->AddSubMenu(L"Add &OSM Layer");
	mnu2->AddItem(L"OSM Tile", MNU_OSM_TILE, 0, 0);
	mnu2->AddItem(L"Open Cycle Map", MNU_OSM_CYCLE, 0, 0);
	mnu2->AddItem(L"Transport", MNU_OSM_TRANSP, 0, 0);
	mnu2->AddItem(L"MapQuest", MNU_OSM_MAPQUEST, 0, 0);
	
	mnu = this->mnuMain->AddSubMenu(L"&Device");
	mnu->AddItem(L"&Capture Device", MNU_CAP_DEV, 0, 0);
	mnu->AddItem(L"&MTK GPS Tracker", MNU_MTK_GPS, 0, 0);
	mnu->AddItem(L"&GSM Modem", MNU_GSM_MODEM, 0, 0);
	mnu->AddItem(L"G&PS Tracker", MNU_GPS_TRACKER, 0, 0);
//	mnu->AddItem(L"&WIA Device", MNU_WIA_DEV, 0, 0);
	mnu->AddItem(L"&TV Control", MNU_TV_CONTROL, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"&Help");
	mnu->AddItem(L"&About...", MNU_ABOUT, 0, 0);*/

//	this->SetMenu(this->mnuMain);
}

SSWR::AVIReadCE::AVIRCEBaseForm::~AVIRCEBaseForm()
{
	Data::ArrayList<Data::ArrayList<MenuInfo*>*> *menuList;
	Data::ArrayList<MenuInfo*> *menu;
	MenuInfo *info;
	OSInt i;
	OSInt j;
	menuList = this->menuItems->GetValues();
	i = menuList->GetCount();
	while (i-- > 0)
	{
		menu = menuList->GetItem(i);
		j = menu->GetCount();
		while (j-- > 0)
		{
			info = menu->GetItem(j);
			MemFree(info);
		}
		DEL_CLASS(menu);
	}
	DEL_CLASS(this->menuItems);
}

void SSWR::AVIReadCE::AVIRCEBaseForm::EventMenuClicked(UInt16 cmdId)
{
	WChar sbuff[512];
	WChar sbuff2[16];
	Map::TileMap *tileMap;
	Map::IMapDrawLayer *mapLyr;

	switch (cmdId)
	{
	case MNU_PROC_INFO:
		{
			SSWR::AVIReadCE::AVIRCEProcInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIReadCE::AVIRCEProcInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_EXIT:
		this->Close();
		break;
	case MNU_ABOUT:
		{
			SSWR::AVIReadCE::AVIRCEAboutForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIReadCE::AVIRCEAboutForm(0, this->ui));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}
		break;
/*	case MNU_OPEN_ODBC:
		{
			SSWR::AVIRead::AVIRODBCForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRODBCForm(0, this->ui, this->core));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}
		break;
	case MNU_SET_CODEPAGE:
		{
			SSWR::AVIRead::AVIRCodePageForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRCodePageForm(0, this->ui, this->core));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}
		break;
	case MNU_SET_COLOR:
		{
			SSWR::AVIRead::AVIRColorSettingForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRColorSettingForm(0, this->ui, this->core));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}
		break;
	case MNU_GEN_IMAGE:
		{
			SSWR::AVIRead::AVIRGenImageForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRGenImageForm(0, this->ui, this->core));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}break;
	case MNU_CAP_DEV:
		{
			SSWR::AVIRead::AVIRCaptureDevForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRCaptureDevForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Media::MediaFile *mf;
				dlg->capture->GetName(sbuff);
				NEW_CLASS(mf, Media::MediaFile(sbuff));
				mf->AddSource(dlg->capture, 0);
				this->core->OpenObject(mf);
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_MTK_GPS:
		{
			IO::SerialPort *port;
			IO::MTKGPSNMEA *mtk;
			Map::GPSTrack *trk;

			NEW_CLASS(port, IO::SerialPort(IO::MTKGPSNMEA::GetMTKSerialPort(), 115200, true));
			if (!port->IsError())
			{
				NEW_CLASS(mtk, IO::MTKGPSNMEA(port));
				if (mtk->IsMTKDevice())
				{
					NEW_CLASS(trk, Map::GPSTrack(L"MTK_Tracker", true, 0));
					if (mtk->ParseLog(trk))
					{
						core->OpenObject(trk);
					}
					else
					{
						UI::MessageDialog::ShowDialog(L"Error in parsing log", L"MTK Tracker");
						DEL_CLASS(trk);
					}
				}
				else
				{
					UI::MessageDialog::ShowDialog(L"MTK Tracker not found", L"MTK Tracker");
				}
				DEL_CLASS(mtk);
				DEL_CLASS(port);
			}
		}
		break;
	case MNU_GSM_MODEM:
		{
			SSWR::AVIRead::AVIRSelSerialPortForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelSerialPortForm(hInst, 0, this->ui, this->core));
			frm->SetText(L"Select GSM Modem");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenGSMModem(frm->port);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_GPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelSerialPortForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelSerialPortForm(hInst, 0, this->ui, this->core));
			frm->SetText(L"Select GPS Tracker");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRGPSTrackerForm *gpsFrm;
				IO::GPSNMEA *gps;
				NEW_CLASS(gps, IO::GPSNMEA(frm->port));
				NEW_CLASS(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(this->ui->GetHInst(), 0, this->ui, this->core, frm->port, gps));
				this->core->ShowForm(gpsFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_WIA_DEV:
		{
			SSWR::AVIRead::AVIRWIADevForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWIADevForm(hInst, 0, this->ui, this->core));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				//this->core->OpenGSMModem(frm->port);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_OSM_TILE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, L"OSMTile");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile.openstreetmap.org/", sbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile.openstreetmap.org/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile.openstreetmap.org/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_CYCLE:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, L"OSMOpenCycleMap");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile.opencyclemap.org/cycle/", sbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile.opencyclemap.org/cycle/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile.opencyclemap.org/cycle/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_TRANSP:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, L"OSMTransport");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile2.opencyclemap.org/transport/", sbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile2.opencyclemap.org/transport/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile2.opencyclemap.org/transport/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_MAPQUEST:
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, L"OSMMapQuest");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://otile1.mqcdn.com/tiles/1.0.0/osm/", sbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile2.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile3.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile4.mqcdn.com/tiles/1.0.0/osm/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_ESRI_MAP:
		{
			SSWR::AVIRead::AVIRESRIMapForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRESRIMapForm(hInst, 0, this->ui));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Crypto::Hash::CRC32R crc(Crypto::Hash::CRC32R::GetPolynormialIEEE());
				Map::ESRI::ESRITileMap *map;
				UInt32 crcVal;
				const WChar *url = frm->GetSelectedURL();
				crc.Calc((UInt8*)url, Text::StrCharCnt(url) * sizeof(WChar));
				crc.GetValue((UInt8*)&crcVal);
				IO::Path::GetProcessFileName(sbuff);
				Text::StrInt32(sbuff2, crcVal);
				Text::StrConcat(IO::Path::AppendPath(sbuff, sbuff2), L"\\");
				NEW_CLASS(map, Map::ESRI::ESRITileMap(url, sbuff, this->core->GetSocketFactory()));
				NEW_CLASS(mapLyr, Map::TileMapLayer(map, this->core->GetParserList()));
				this->core->OpenObject(mapLyr);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_TEXT_VIEWER:
		{
			UtilUI::TextViewerForm *frm;
			NEW_CLASS(frm, UtilUI::TextViewerForm(hInst, 0, this->ui));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_NET_INFO:
		{
			SSWR::AVIRead::AVIRNetInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNetInfoForm(hInst, 0, this->ui));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_SUDOKU_SOLVER:
		{
			SSWR::AVIRead::AVIRSudokuForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSudokuForm(hInst, 0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;*/
/*	case MNU_OPEN_FILE:
		{
			SSWR::AVIReadCE::AVIRCEOpenFileForm *frm;
			NEW_CLASS(frm, SSWR::AVIReadCE::AVIRCEOpenFileForm(hInst, 0, this->ui));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				const WChar *fname = frm->GetFileName();
				OSInt i = Text::StrIndexOf(fname, ':');
				if (i < 0 || i == 1)
				{
					IO::StmData::FileData *fd;
					NEW_CLASS(fd, IO::StmData::FileData(fname, false));
					if (fd->GetDataSize() == 0)
					{
						UI::MessageDialog::ShowDialog(L"Error in loading file", L"AVIRead");
					}
					else
					{
						this->core->LoadData(fd);
					}
					DEL_CLASS(fd);
				}
				else
				{
					IO::ParsedObject *pobj = Net::URL::OpenObject(fname, this->core->GetSocketFactory());
					if (pobj == 0)
					{
						UI::MessageDialog::ShowDialog(L"Error in loading file", L"AVIRead");
					}
					else
					{
						this->core->OpenObject(pobj);
					}
				}
			}
			DEL_CLASS(frm);
		}
		break;*/
/*	case MNU_PLAYLIST:
		{
			Media::Playlist *playlist;
			NEW_CLASS(playlist, Media::Playlist(L"Untitled", this->core->GetParserList()));
			this->core->OpenObject(playlist);
		}
		break;
	case MNU_HTTP_SVR:
		{
			SSWR::AVIRead::AVIRHTTPSvrForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHTTPSvrForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TV_CONTROL:
		{
			SSWR::AVIRead::AVIRTVControlForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTVControlForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STMTERM:
		{
			SSWR::AVIRead::AVIRStreamTermForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStreamTermForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NTPCLIENT:
		{
			SSWR::AVIRead::AVIRNTPClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNTPClientForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SET_AUDIO:
		{
			SSWR::AVIRead::AVIRSetAudioForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSetAudioForm(hInst, 0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_MPEG_TOOL:
		{
			SSWR::AVIRead::AVIRMPEGToolForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMPEGToolForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SET_DPI:
		{
			SSWR::AVIRead::AVIRSetDPIForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSetDPIForm(hInst, 0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_DNSCLIENT:
		{
			SSWR::AVIRead::AVIRDNSClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDNSClientForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILEEX:
		{
			SSWR::AVIRead::AVIRFileExForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFileExForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DNSPROXY:
		{
			SSWR::AVIRead::AVIRDNSProxyForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDNSProxyForm(0, this->ui, this->core));
			if (frm->IsError())
			{
				DEL_CLASS(frm);
				UI::MessageDialog::ShowDialog(L"Error in starting DNS proxy. Port is in use?", L"Error");
			}
			else
			{
				this->core->ShowForm(frm);
			}
		}
		break;
	case MNU_SMTPSERVER:
		{
			SSWR::AVIRead::AVIRSMTPServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSMTPServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_AUDIOFILTER:
		{
			SSWR::AVIRead::AVIRAudioFilterForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRAudioFilterForm(0, this->ui, this->core, false));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NET_PING:
		{
			SSWR::AVIRead::AVIRNetPingForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNetPingForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IMAGE_BATCH:
		{
			SSWR::AVIRead::AVIRImageBatchForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRImageBatchForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILE_HASH:
		{
			SSWR::AVIRead::AVIRFileHashForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFileHashForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_UDP_CAPTURE:
		{
			SSWR::AVIRead::AVIRUDPCaptureForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRUDPCaptureForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPCLIENT:
		{
			SSWR::AVIRead::AVIRHTTPClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHTTPClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WHOISCLIENT:
		{
			SSWR::AVIRead::AVIRWHOISClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWHOISClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TCPTEST:
		{
			SSWR::AVIRead::AVIRTCPTestForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTCPTestForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPTEST:
		{
			SSWR::AVIRead::AVIRHTTPTestForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHTTPTestForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROXYSERVER:
		{
			SSWR::AVIRead::AVIRProxyServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRProxyServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPPROXYCLIENT:
		{
			SSWR::AVIRead::AVIRHTTPProxyClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHTTPProxyClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROTODEC:
		{
			SSWR::AVIRead::AVIRProtoDecForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRProtoDecForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROF_RESIZER:
		{
			SSWR::AVIRead::AVIRProfiledResizerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRProfiledResizerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOG_BACKUP:
		{
			SSWR::AVIRead::AVIRLogBackupForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLogBackupForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TEXT_ENCRYPT:
		{
			SSWR::AVIRead::AVIREncryptForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIREncryptForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAM_CONV:
		{
			SSWR::AVIRead::AVIRStreamConvForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStreamConvForm(hInst, 0, this->ui, this->core));
			this->core->ShowForm(frm);
		}*/
	}
}
