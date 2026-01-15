#include "Stdafx.h"
#include "Text/StringBuilder.hpp"
#include "Crypto/Hash/CRC32R.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/Device/MTKGPSNMEA.h"
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
#include "UI/GUITextBox.h"
#include "UtilUI/TextViewerForm.h"

typedef enum
{
	MNU_MISC,
	MNU_HELP,
	MNU_ABOUT = 100,
	MNU_PROC_INFO,
	MNU_EXIT
} MenuItems;

void __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::FileHandler(AnyType userObj, const UTF8Char **files, UIntOS nFiles)
{
	NN<SSWR::AVIReadCE::AVIRCEBaseForm> me = userObj.GetNN<AVIReadCE::AVIRCEBaseForm>();
	IO::Path::PathType pt;
	NN<IO::DirectoryPackage> pkg;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Cannot parse:"));
	Bool found = false;
	me->core->BeginLoad();
	IntOS i = 0;
	while (i < nFiles)
	{
		UIntOS fileNameLen = Text::StrCharCnt(files[i]);
		pt = IO::Path::GetPathType(Text::CStringNN(files[i], fileNameLen));
		if (pt == IO::Path::PathType::Directory)
		{
			NEW_CLASSNN(pkg, IO::DirectoryPackage({files[i], fileNameLen}));
			me->core->OpenObject(pkg);
		}
		else if (pt == IO::Path::PathType::File)
		{
			IO::StmData::FileData fd({files[i], fileNameLen}, false);
			if (!me->core->LoadData(fd, 0))
			{
				sb.AppendC(UTF8STRC("\n"));
				sb.AppendSlow(files[i]);
				found = true;
			}
		}
		i++;
	}
	me->core->EndLoad();
	if (found)
	{
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("Error"), me);
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::OnCategoryChg(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEBaseForm> me = userObj.GetNN<AVIReadCE::AVIRCEBaseForm>();
	Int32 item = (Int32)me->lbCategory->GetSelectedItem().GetIntOS();
	NN<Data::ArrayListNN<MenuInfo>> menu;
	NN<MenuInfo> info;
	UIntOS i;
	UIntOS j;
	me->lbContent->ClearItems();
	if (me->menuItems.Get(item).SetTo(menu))
	{
		i = 0;
		j = menu->GetCount();
		while (i < j)
		{
			info = menu->GetItemNoCheck(i);
			me->lbContent->AddItem(info->name, (void*)info->item);
			i++;
		}
	}
}

void __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::OnContentClick(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEBaseForm> me = userObj.GetNN<AVIReadCE::AVIRCEBaseForm>();
	Int32 item = (Int32)me->lbContent->GetSelectedItem().GetIntOS();
	if (item)
	{
		me->EventMenuClicked((UInt16)item);
	}
}

NN<SSWR::AVIReadCE::AVIRCEBaseForm::MenuInfo> __stdcall SSWR::AVIReadCE::AVIRCEBaseForm::NewMenuItem(Text::CStringNN name, Int32 item)
{
	NN<MenuInfo> info;
	info = MemAllocNN(MenuInfo);
	info->name = Text::String::New(name);
	info->item = item;
	return info;
}

SSWR::AVIReadCE::AVIRCEBaseForm::AVIRCEBaseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 480, ui)
{
	this->core = core;
	this->SetText(CSTR("AVIRead"));
	this->SetFont(nullptr, 8.75, false);
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	NN<Data::ArrayListNN<MenuInfo>> menu;

	Math::Size2D<UIntOS> sz = this->GetSizeP();
	this->lbCategory = ui->NewListBox(*this, true);
	this->lbCategory->SetRect(0, 0, (sz.x >> 1) - 3, 23, false);
	this->lbCategory->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCategory->HandleSelectionChange(OnCategoryChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->lbContent = ui->NewListBox(*this, true);
	this->lbContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbContent->HandleDoubleClicked(OnContentClick, this);
	
	NEW_CLASSNN(menu, Data::ArrayListNN<MenuInfo>());
	menu->Add(NewMenuItem(CSTR("Process Info"), MNU_PROC_INFO));
	menu->Add(NewMenuItem(CSTR("Exit"), MNU_EXIT));
	this->menuItems.Put(MNU_MISC, menu);
	this->lbCategory->AddItem(CSTR("Misc"), (void*)MNU_MISC);
	NEW_CLASSNN(menu, Data::ArrayListNN<MenuInfo>());
	menu->Add(NewMenuItem(CSTR("About"), MNU_ABOUT));
	this->menuItems.Put(MNU_HELP, menu);
	this->lbCategory->AddItem(CSTR("Help"), (void*)MNU_HELP);

/*	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(L"M&isc");
	mnu->AddItem(L"Open File"), MNU_OPEN_FILE, MOD_CONTROL, 'O');
	mnu->AddItem(L"Open &ODBC"), MNU_OPEN_ODBC, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"Stream Converter"), MNU_STREAM_CONV, 0, 0);
	mnu->AddItem(L"Stream Terminal"), MNU_STMTERM, 0, 0);
	mnu->AddItem(L"File Extractor"), MNU_FILEEX, 0, 0);
	mnu->AddItem(L"File Hash"), MNU_FILE_HASH, 0, 0);
	mnu->AddItem(L"Log Backup"), MNU_LOG_BACKUP, 0, 0);
	mnu->AddItem(L"Text Viewer"), MNU_TEXT_VIEWER, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"Sudoku Solver"), MNU_SUDOKU_SOLVER, 0, 0);
	mnu->AddItem(L"Process Info"), MNU_PROC_INFO, 0, 0);
	mnu->AddItem(L"Text Encrypt"), MNU_TEXT_ENCRYPT, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"Protocol Decoder"), MNU_PROTODEC, 0, 0);
	
	mnu = this->mnuMain->AddSubMenu(L"&Setting");
	mnu->AddItem(L"Set &CodePage"), MNU_SET_CODEPAGE, 0, 0);
	mnu->AddItem(L"Set &Monitor Color"), MNU_SET_COLOR, 0, 0);
	mnu->AddItem(L"Set &Audio Device"), MNU_SET_AUDIO, 0, 0);
	mnu->AddItem(L"Set Monitor &DPI"), MNU_SET_DPI, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"&Network");
	mnu->AddItem(L"Network Info"), MNU_NET_INFO, 0, 0);
	mnu->AddSeperator();
	mnu->AddItem(L"HTTP Server"), MNU_HTTP_SVR, 0, 0);
	mnu->AddItem(L"HTTP Client"), MNU_HTTPCLIENT, 0, 0);
	mnu->AddItem(L"HTTP Proxy Client"), MNU_HTTPPROXYCLIENT, 0, 0);
	mnu->AddItem(L"NTP Client"), MNU_NTPCLIENT, 0, 0);
	mnu->AddItem(L"DNS Client"), MNU_DNSCLIENT, 0, 0);
	mnu->AddItem(L"DNS Proxy"), MNU_DNSPROXY, 0, 0);
	mnu->AddItem(L"SMTP Server"), MNU_SMTPSERVER, 0, 0);
	mnu->AddItem(L"Ping"), MNU_NET_PING, 0, 0);
	mnu->AddItem(L"UDP Capture"), MNU_UDP_CAPTURE, 0, 0);
	mnu->AddItem(L"WHOIS Client"), MNU_WHOISCLIENT, 0, 0);
//	mnu->AddItem(L"Proxy Server"), MNU_PROXYSERVER, 0, 0);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu(L"Test");
	mnu2->AddItem(L"TCP Conn Test"), MNU_TCPTEST, 0, 0);
	mnu2->AddItem(L"HTTP Test"), MNU_HTTPTEST, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"M&edia");
	mnu->AddItem(L"&Generate Image"), MNU_GEN_IMAGE, 0, 0);
	mnu->AddItem(L"Media Playlist"), MNU_PLAYLIST, 0, 0);
	mnu->AddItem(L"MPEG Tool"), MNU_MPEG_TOOL, 0, 0);
	mnu->AddItem(L"Audio Filter"), MNU_AUDIOFILTER, 0, 0);
	mnu->AddItem(L"Image Batch Tool"), MNU_IMAGE_BATCH, 0, 0);
	mnu->AddItem(L"Profiled Resizer"), MNU_PROF_RESIZER, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"&Map");
	mnu->AddItem(L"Add ESRI Tile Map"), MNU_ESRI_MAP, 0, 0);
	mnu2 = mnu->AddSubMenu(L"Add &OSM Layer");
	mnu2->AddItem(L"OSM Tile"), MNU_OSM_TILE, 0, 0);
	mnu2->AddItem(L"Open Cycle Map"), MNU_OSM_CYCLE, 0, 0);
	mnu2->AddItem(L"Transport"), MNU_OSM_TRANSP, 0, 0);
	mnu2->AddItem(L"MapQuest"), MNU_OSM_MAPQUEST, 0, 0);
	
	mnu = this->mnuMain->AddSubMenu(L"&Device");
	mnu->AddItem(L"&Capture Device"), MNU_CAP_DEV, 0, 0);
	mnu->AddItem(L"&MTK GPS Tracker"), MNU_MTK_GPS, 0, 0);
	mnu->AddItem(L"&GSM Modem"), MNU_GSM_MODEM, 0, 0);
	mnu->AddItem(L"G&PS Tracker"), MNU_GPS_TRACKER, 0, 0);
//	mnu->AddItem(L"&WIA Device"), MNU_WIA_DEV, 0, 0);
	mnu->AddItem(L"&TV Control"), MNU_TV_CONTROL, 0, 0);

	mnu = this->mnuMain->AddSubMenu(L"&Help");
	mnu->AddItem(L"&About..."), MNU_ABOUT, 0, 0);*/

//	this->SetMenu(this->mnuMain);
}

SSWR::AVIReadCE::AVIRCEBaseForm::~AVIRCEBaseForm()
{
	NN<Data::ArrayListNN<MenuInfo>> menu;
	NN<MenuInfo> info;
	UIntOS i;
	UIntOS j;
	i = this->menuItems.GetCount();
	while (i-- > 0)
	{
		menu = this->menuItems.GetItemNoCheck(i);
		j = menu->GetCount();
		while (j-- > 0)
		{
			info = menu->GetItemNoCheck(j);
			info->name->Release();
			MemFreeNN(info);
		}
		menu.Delete();
	}
}

void SSWR::AVIReadCE::AVIRCEBaseForm::EventMenuClicked(UInt16 cmdId)
{
	WChar wbuff[512];
	WChar wbuff2[16];
	Map::TileMap *tileMap;
	Map::MapDrawLayer *mapLyr;

	switch (cmdId)
	{
	case MNU_PROC_INFO:
		{
			NN<SSWR::AVIReadCE::AVIRCEProcInfoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIReadCE::AVIRCEProcInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_EXIT:
		this->Close();
		break;
	case MNU_ABOUT:
		{
			SSWR::AVIReadCE::AVIRCEAboutForm dlg(0, this->ui);
			dlg.ShowDialog(this);
		}
		break;
/*	case MNU_OPEN_ODBC:
		{
			SSWR::AVIRead::AVIRODBCForm dlg(0, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_SET_CODEPAGE:
		{
			SSWR::AVIRead::AVIRCodePageForm dlg(0, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_SET_COLOR:
		{
			SSWR::AVIRead::AVIRColorSettingForm dlg(0, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_GEN_IMAGE:
		{
			SSWR::AVIRead::AVIRGenImageForm dlg(0, this->ui, this->core);
			dlg.ShowDialog(this);
		}break;
	case MNU_CAP_DEV:
		{
			SSWR::AVIRead::AVIRCaptureDevForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Media::MediaFile *mf;
				dlg.capture->GetName(wbuff);
				NEW_CLASS(mf, Media::MediaFile(wbuff));
				mf->AddSource(dlg.capture, 0);
				this->core->OpenObject(mf);
			}
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
			SSWR::AVIRead::AVIRSelSerialPortForm frm(hInst, 0, this->ui, this->core);
			frm->SetText(L"Select GSM Modem");
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenGSMModem(frm.port);
			}
		}
		break;
	case MNU_GPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelSerialPortForm frm(hInst, 0, this->ui, this->core);
			frm.SetText(L"Select GPS Tracker");
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRGPSTrackerForm *gpsFrm;
				IO::GPSNMEA *gps;
				NEW_CLASS(gps, IO::GPSNMEA(frm->port));
				NEW_CLASS(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(this->ui->GetHInst(), 0, this->ui, this->core, frm->port, gps));
				this->core->ShowForm(gpsFrm);
			}
		}
		break;
	case MNU_WIA_DEV:
		{
			SSWR::AVIRead::AVIRWIADevForm frm(hInst, 0, this->ui, this->core);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				//this->core->OpenGSMModem(frm.port);
			}
		}
		break;
	case MNU_OSM_TILE:
		IO::Path::GetProcessFileName(wbuff);
		IO::Path::AppendPath(wbuff, L"OSMTile");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile.openstreetmap.org/", wbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile.openstreetmap.org/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile.openstreetmap.org/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_CYCLE:
		IO::Path::GetProcessFileName(wbuff);
		IO::Path::AppendPath(wbuff, L"OSMOpenCycleMap");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile.opencyclemap.org/cycle/", wbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile.opencyclemap.org/cycle/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile.opencyclemap.org/cycle/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_TRANSP:
		IO::Path::GetProcessFileName(wbuff);
		IO::Path::AppendPath(wbuff, L"OSMTransport");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile2.opencyclemap.org/transport/", wbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile2.opencyclemap.org/transport/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile2.opencyclemap.org/transport/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_MAPQUEST:
		IO::Path::GetProcessFileName(wbuff);
		IO::Path::AppendPath(wbuff, L"OSMMapQuest");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://otile1.mqcdn.com/tiles/1.0.0/osm/", wbuff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile2.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile3.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile4.mqcdn.com/tiles/1.0.0/osm/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_ESRI_MAP:
		{
			SSWR::AVIRead::AVIRESRIMapForm frm(hInst, 0, this->ui);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Crypto::Hash::CRC32R crc(Crypto::Hash::CRC32R::GetPolynormialIEEE());
				Map::ESRI::ESRITileMap *map;
				UInt32 crcVal;
				const WChar *url = frm.GetSelectedURL();
				crc.Calc((UInt8*)url, Text::StrCharCnt(url) * sizeof(WChar));
				crc.GetValue((UInt8*)&crcVal);
				IO::Path::GetProcessFileName(wbuff);
				Text::StrInt32(wbuff2, crcVal);
				Text::StrConcat(IO::Path::AppendPath(wbuff, wbuff2), L"\\");
				NEW_CLASS(map, Map::ESRI::ESRITileMap(url, wbuff, this->core->GetSocketFactory()));
				NEW_CLASS(mapLyr, Map::TileMapLayer(map, this->core->GetParserList()));
				this->core->OpenObject(mapLyr);
			}
		}
		break;
	case MNU_TEXT_VIEWER:
		{
			UtilUI::TextViewerForm frm(hInst, 0, this->ui);
			frm.ShowDialog(this);
		}
		break;
	case MNU_NET_INFO:
		{
			SSWR::AVIRead::AVIRNetInfoForm frm(hInst, 0, this->ui);
			frm.ShowDialog(this);
		}
		break;
	case MNU_SUDOKU_SOLVER:
		{
			SSWR::AVIRead::AVIRSudokuForm frm(hInst, 0, this->ui, this->core);
			frm.ShowDialog(this);
		}
		break;*/
/*	case MNU_OPEN_FILE:
		{
			SSWR::AVIReadCE::AVIRCEOpenFileForm frm(hInst, 0, this->ui);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				const WChar *fname = frm.GetFileName();
				UIntOS i = Text::StrIndexOfChar(fname, ':');
				if (i == INVALID_INDEX || i == 1)
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
			SSWR::AVIRead::AVIRSetAudioForm frm(hInst, 0, this->ui, this->core);
			frm.ShowDialog(this);
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
			SSWR::AVIRead::AVIRSetDPIForm frm(hInst, 0, this->ui, this->core);
			frm.ShowDialog(this);
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
