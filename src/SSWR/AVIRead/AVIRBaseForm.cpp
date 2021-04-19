#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
#include "DB/OLEDBConn.h"
#endif
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/SensorManager.h"
#include "IO/SerialPort.h"
#include "IO/Device/GoProCameraControl.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "IO/Device/OlympusCameraControl.h"
#include "IO/Device/RockwellModemController.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Map/ESRI/ESRITileMap.h"
#include "Media/ImageList.h"
#include "Media/MediaFile.h"
#include "Media/Playlist.h"
#include "Media/ScreenCapturer.h"
#include "Net/URL.h"
#include "SSWR/AVIRead/AVIRAboutForm.h"
#include "SSWR/AVIRead/AVIRAccelerometerForm.h"
#include "SSWR/AVIRead/AVIRARPPingForm.h"
#include "SSWR/AVIRead/AVIRARPScanForm.h"
#include "SSWR/AVIRead/AVIRAsmConvForm.h"
#include "SSWR/AVIRead/AVIRAudioFilterForm.h"
#include "SSWR/AVIRead/AVIRBaseForm.h"
#include "SSWR/AVIRead/AVIRBenchmarkForm.h"
#include "SSWR/AVIRead/AVIRBluetoothForm.h"
#include "SSWR/AVIRead/AVIRBluetoothLEForm.h"
#include "SSWR/AVIRead/AVIRBruteForceForm.h"
#include "SSWR/AVIRead/AVIRCalculator1Form.h"
#include "SSWR/AVIRead/AVIRCameraControlForm.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRChineseForm.h"
#include "SSWR/AVIRead/AVIRClipboardViewerForm.h"
#include "SSWR/AVIRead/AVIRCodeImageGenForm.h"
#include "SSWR/AVIRead/AVIRCodePageForm.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRCoordConvForm.h"
#include "SSWR/AVIRead/AVIRCoordInfoForm.h"
#include "SSWR/AVIRead/AVIRCOVID19Form.h"
#include "SSWR/AVIRead/AVIRCPUInfoForm.h"
#include "SSWR/AVIRead/AVIRDBManagerForm.h"
#include "SSWR/AVIRead/AVIRDHCPServerForm.h"
#include "SSWR/AVIRead/AVIRDHT22Form.h"
#include "SSWR/AVIRead/AVIRDNSClientForm.h"
#include "SSWR/AVIRead/AVIRDNSProxyForm.h"
#include "SSWR/AVIRead/AVIRDragDropViewerForm.h"
#include "SSWR/AVIRead/AVIRDS18B20Form.h"
#include "SSWR/AVIRead/AVIRDWQB30Form.h"
#include "SSWR/AVIRead/AVIREDIDViewerForm.h"
#include "SSWR/AVIRead/AVIReGaugeSvrForm.h"
#include "SSWR/AVIRead/AVIRElectronicScaleForm.h"
#include "SSWR/AVIRead/AVIREncryptForm.h"
#include "SSWR/AVIRead/AVIRESRIMapForm.h"
#include "SSWR/AVIRead/AVIRFileAnalyseForm.h"
#include "SSWR/AVIRead/AVIRFileExForm.h"
#include "SSWR/AVIRead/AVIRFileHashForm.h"
#include "SSWR/AVIRead/AVIRFileSizePackForm.h"
#include "SSWR/AVIRead/AVIRGenImageForm.h"
#include "SSWR/AVIRead/AVIRGPIOStatusForm.h"
#include "SSWR/AVIRead/AVIRGPSDevForm.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "SSWR/AVIRead/AVIRGPUInfoForm.h"
#include "SSWR/AVIRead/AVIRGUIEventForm.h"
#include "SSWR/AVIRead/AVIRHashTestForm.h"
#include "SSWR/AVIRead/AVIRHIDDeviceForm.h"
#include "SSWR/AVIRead/AVIRHQMPForm.h"
#include "SSWR/AVIRead/AVIRHTTPClientForm.h"
#include "SSWR/AVIRead/AVIRHTTPDownloaderForm.h"
#include "SSWR/AVIRead/AVIRHTTPProxyClientForm.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "SSWR/AVIRead/AVIRHTTPTestForm.h"
#include "SSWR/AVIRead/AVIRIBuddyForm.h"
#include "SSWR/AVIRead/AVIRICCInfoForm.h"
#include "SSWR/AVIRead/AVIRImageBatchConvForm.h"
#include "SSWR/AVIRead/AVIRImageBatchForm.h"
#include "SSWR/AVIRead/AVIRImagePSNRForm.h"
#include "SSWR/AVIRead/AVIRImageViewerForm.h"
#include "SSWR/AVIRead/AVIRIOPinTestForm.h"
#include "SSWR/AVIRead/AVIRIPScanDetectorForm.h"
#include "SSWR/AVIRead/AVIRIPScanForm.h"
#include "SSWR/AVIRead/AVIRJSTextForm.h"
#include "SSWR/AVIRead/AVIRJTT808ServerForm.h"
#include "SSWR/AVIRead/AVIRLDAPClientForm.h"
#include "SSWR/AVIRead/AVIRLDAPExplorerForm.h"
#include "SSWR/AVIRead/AVIRLineCounterForm.h"
#include "SSWR/AVIRead/AVIRLogBackupForm.h"
#include "SSWR/AVIRead/AVIRLogExtractForm.h"
#include "SSWR/AVIRead/AVIRLogMergeForm.h"
#include "SSWR/AVIRead/AVIRLogServerForm.h"
#include "SSWR/AVIRead/AVIRMACGenForm.h"
#include "SSWR/AVIRead/AVIRMACManagerForm.h"
#include "SSWR/AVIRead/AVIRMODBUSMasterForm.h"
#include "SSWR/AVIRead/AVIRMQTTBrokerForm.h"
#include "SSWR/AVIRead/AVIRMQTTExplorerForm.h"
#include "SSWR/AVIRead/AVIRMQTTPublishForm.h"
#include "SSWR/AVIRead/AVIRMQTTSubscribeForm.h"
#include "SSWR/AVIRead/AVIRMSSQLConnForm.h"
#include "SSWR/AVIRead/AVIRMySQLClientForm.h"
#include "SSWR/AVIRead/AVIRMySQLServerForm.h"
#include "SSWR/AVIRead/AVIRNetInfoForm.h"
#include "SSWR/AVIRead/AVIRNetPingForm.h"
#include "SSWR/AVIRead/AVIRNetRAWCaptureForm.h"
#include "SSWR/AVIRead/AVIRNTPClientForm.h"
#include "SSWR/AVIRead/AVIRNTPServerForm.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "SSWR/AVIRead/AVIRODBCStrForm.h"
#include "SSWR/AVIRead/AVIROLEDBForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIROSMCacheCfgForm.h"
#include "SSWR/AVIRead/AVIRPaintCntForm.h"
#include "SSWR/AVIRead/AVIRPasswordHashForm.h"
#include "SSWR/AVIRead/AVIRPCIDeviceForm.h"
#include "SSWR/AVIRead/AVIRPerformanceLogForm.h"
#include "SSWR/AVIRead/AVIRPingMonitorForm.h"
#include "SSWR/AVIRead/AVIRPowerForm.h"
#include "SSWR/AVIRead/AVIRProcInfoForm.h"
#include "SSWR/AVIRead/AVIRProfiledResizerForm.h"
#include "SSWR/AVIRead/AVIRProtoDecForm.h"
#include "SSWR/AVIRead/AVIRProxyServerForm.h"
#include "SSWR/AVIRead/AVIRRAWMonitorForm.h"
#include "SSWR/AVIRead/AVIRRESTfulForm.h"
#include "SSWR/AVIRead/AVIRRSSReaderForm.h"
#include "SSWR/AVIRead/AVIRSDCardForm.h"
#include "SSWR/AVIRead/AVIRSelIOPinForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "SSWR/AVIRead/AVIRSetDPIForm.h"
#include "SSWR/AVIRead/AVIRSetLocationSvcForm.h"
#include "SSWR/AVIRead/AVIRSMTPServerForm.h"
#include "SSWR/AVIRead/AVIRSNBDongleForm.h"
#include "SSWR/AVIRead/AVIRSNMPClientForm.h"
#include "SSWR/AVIRead/AVIRSNMPManagerForm.h"
#include "SSWR/AVIRead/AVIRSNMPMIBForm.h"
#include "SSWR/AVIRead/AVIRSNMPTrapMonitorForm.h"
#include "SSWR/AVIRead/AVIRSNSManagerForm.h"
#include "SSWR/AVIRead/AVIRStreamConvForm.h"
#include "SSWR/AVIRead/AVIRStreamEchoForm.h"
#include "SSWR/AVIRead/AVIRStreamLatencyForm.h"
#include "SSWR/AVIRead/AVIRStreamTermForm.h"
#include "SSWR/AVIRead/AVIRSudokuForm.h"
#include "SSWR/AVIRead/AVIRSyslogServerForm.h"
#include "SSWR/AVIRead/AVIRTCPPortScanForm.h"
#include "SSWR/AVIRead/AVIRTCPSpdCliForm.h"
#include "SSWR/AVIRead/AVIRTCPSpdSvrForm.h"
#include "SSWR/AVIRead/AVIRTCPTestForm.h"
#include "SSWR/AVIRead/AVIRTFTPClientForm.h"
#include "SSWR/AVIRead/AVIRTFTPServerForm.h"
#include "SSWR/AVIRead/AVIRThreadSpeedForm.h"
#include "SSWR/AVIRead/AVIRTimedCaptureForm.h"
#include "SSWR/AVIRead/AVIRTimedFileCopyForm.h"
#include "SSWR/AVIRead/AVIRTimeTestForm.h"
#include "SSWR/AVIRead/AVIRTraceRouteForm.h"
#include "SSWR/AVIRead/AVIRTVControlForm.h"
#include "SSWR/AVIRead/AVIRUDPCaptureForm.h"
#include "SSWR/AVIRead/AVIRUDPTestForm.h"
#include "SSWR/AVIRead/AVIRUSBDeviceForm.h"
#include "SSWR/AVIRead/AVIRUserAgentBatchForm.h"
#include "SSWR/AVIRead/AVIRUserAgentParseForm.h"
#include "SSWR/AVIRead/AVIRVideoCheckerForm.h"
#include "SSWR/AVIRead/AVIRVideoInfoForm.h"
#include "SSWR/AVIRead/AVIRVoiceModemForm.h"
#include "SSWR/AVIRead/AVIRWebSite7gogoForm.h"
#include "SSWR/AVIRead/AVIRWebSite48IdolForm.h"
#include "SSWR/AVIRead/AVIRWebSiteInstagramForm.h"
#include "SSWR/AVIRead/AVIRWebSiteTwitterForm.h"
#include "SSWR/AVIRead/AVIRWellFormatForm.h"
#include "SSWR/AVIRead/AVIRWHOISClientForm.h"
#include "SSWR/AVIRead/AVIRWIADevForm.h"
#include "SSWR/AVIRead/AVIRWifiCaptureForm.h"
#include "SSWR/AVIRead/AVIRWifiCaptureLiteForm.h"
#include "SSWR/AVIRead/AVIRWiFiLogManagerForm.h"
#include "SSWR/AVIRead/AVIRWifiScanForm.h"
#include "SSWR/AVIRead/AVIRWMIForm.h"
#include "SSWR/AVIRead/AVIRWOLForm.h"
#include "SSWR/AVIRead/AVIRXMLWalkForm.h"
#include "SSWR/SHPConv/SHPConvMainForm.h"
#include "Text/MyStringW.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"
#include "UI/GUITextBox.h"
#include "UI/MessageDialog.h"
#include "UtilUI/TextViewerForm.h"

typedef enum
{
	MNU_ABOUT = 100,
	MNU_SET_CODEPAGE,
	MNU_SET_COLOR,
	MNU_GEN_IMAGE,
	MNU_CAP_DEV,
	MNU_OSM_TILE,
	MNU_OSM_CYCLE,
	MNU_OSM_TRANSP,
	MNU_OSM_LANDSCAPE,
	MNU_OSM_OUTDOORS,
	MNU_OSM_TRANSP_DARK,
	MNU_OSM_SPINAL,
	MNU_OSM_MAPQUEST,
	MNU_MTK_GPS,
	MNU_ESRI_MAP,
	MNU_GSM_MODEM,
	MNU_GPS_TRACKER,
	MNU_WIA_DEV,
	MNU_OPEN_ODBC_DSN,
	MNU_TEXT_VIEWER,
	MNU_NET_INFO,
	MNU_SUDOKU_SOLVER,
	MNU_OPEN_FILE,
	MNU_PLAYLIST,
	MNU_HTTP_SVR,
	MNU_TV_CONTROL,
	MNU_STMTERM,
	MNU_NTPCLIENT,
	MNU_SET_AUDIO,
	MNU_FILE_ANALYSE,
	MNU_PROC_INFO,
	MNU_SET_DPI,
	MNU_DNSCLIENT,
	MNU_DNSPROXY,
	MNU_SMTPSERVER,
	MNU_FILEEX,
	MNU_AUDIOFILTER,
	MNU_NET_PING,
	MNU_IMAGE_BATCH,
	MNU_FILE_HASH,
	MNU_UDP_CAPTURE,
	MNU_HTTPCLIENT,
	MNU_WHOISCLIENT,
	MNU_TCPTEST,
	MNU_HTTPTEST,
	MNU_PROXYSERVER,
	MNU_HTTPPROXYCLIENT,
	MNU_PROTODEC,
	MNU_PROF_RESIZER,
	MNU_LOG_BACKUP,
	MNU_TEXT_ENCRYPT,
	MNU_STREAM_CONV,
	MNU_HQMP3,
	MNU_HQMP3HQ,
	MNU_HQMP3UQ,
	MNU_CPUINFO,
	MNU_IBUDDY,
	MNU_DWQB30,
	MNU_ICCINFO,
	MNU_CLIPBOARD_VIEWER,
	MNU_BENCHMARK,
	MNU_FILE_SIZE_PACK,
	MNU_PAINT_CNT,
	MNU_ASM_CONV,
	MNU_COORD_CONV,
	MNU_GPSDEV,
	MNU_HTTPDOWNLOADER,
	MNU_DRAG_DROP,
	MNU_JSTEXT,
	MNU_UDPTEST,
	MNU_CODEIMAGEGEN,
	MNU_EDIDVIEWER,
	MNU_GPUINFO,
	MNU_WMI,
	MNU_SNB_DONGLE,
	MNU_CHINESE,
	MNU_BLUETOOTH,
	MNU_OSM_CACHE_SERVER,
	MNU_TEST,
	MNU_IMAGEVIEWER,
	MNU_LINECOUNTER,
	MNU_OSM_LOCAL_DIR,
	MNU_TCPSPDCLI,
	MNU_TCPSPDSVR,
	MNU_ACCELEROMETER,
	MNU_WIFICAPTURE,
	MNU_POWERCONTROL,
	MNU_OSM_LOCAL_FILE,
	MNU_TIMETEST,
	MNU_IOPINTEST,
	MNU_DHT22,
	MNU_DS18B20,
	MNU_TIMEDCAPTURE,
	MNU_GUIEVENT,
	MNU_CALCULATOR1,
	MNU_SETLOCATIONSVC,
	MNU_SCREENCAPTURE,
	MNU_HASHTEST,
	MNU_THREADSPEED,
	MNU_ESCALE,
	MNU_VOICEMODEM,
	MNU_PERFORMANCELOG,
	MNU_JTT808,
	MNU_MQTT_BROKER,
	MNU_MQTT_PUBLISH,
	MNU_MQTT_SUBSCRIBE,
	MNU_OLEDB,
	MNU_PCIDEVICE,
	MNU_USBDEVICE,
	MNU_SDCARD,
	MNU_SHPCONV,
	MNU_LOGMERGE,
	MNU_LOGEXTRACT,
	MNU_VIDEOINFO,
	MNU_VIDEOCHECKER,
	MNU_HIDDEVICE,
	MNU_IMAGE_PSNR,
	MNU_IMAGE_BATCH_CONV,
	MNU_MODBUSMASTER,
	MNU_STREAMECHO,
	MNU_STREAMLATENCY,
	MNU_EGAUGESVR,
	MNU_LOGSERVER,
	MNU_PINGMONITOR,
	MNU_MACMANAGER,
	MNU_IPSCAN,
	MNU_BLUETOOTHLE,
	MNU_BRUTEFORCE,
	MNU_TRACEROUTE,
	MNU_RAWMONITOR,
	MNU_TFTPCLIENT,
	MNU_TFTPSERVER,
	MNU_NTPSERVER,
	MNU_TCPPORTSCAN,
	MNU_DHCPSERVER,
	MNU_SNMPCLIENT,
	MNU_SNMPTRAPMON,
	MNU_SNMPMIB,
	MNU_SNMPMANAGER,
	MNU_MACGEN,
	MNU_XMLWALK,
	MNU_WIFISCAN,
	MNU_OLYMPUSCAMERA,
	MNU_WIFICAPTURELITE,
	MNU_SYSLOGSERVER,
	MNU_CAMERACONTROLANY,
	MNU_GOPROCAMERA,
	MNU_USERAGENTPARSE,
	MNU_USERAGENTBATCH,
	MNU_WIFILOGMANAGER,
	MNU_WELLFORMAT,
	MNU_RSSREADER,
	MNU_WEBSITE_48IDOL,
	MNU_WEBSITE_TWITTER,
	MNU_SNSMANAGER,
	MNU_WEBSITE_7GOGO,
	MNU_WEBSITE_INSTAGRAM,
	MNU_GPIOSTATUS,
	MNU_NETRAWCAPTURE,
	MNU_ARPSCAN,
	MNU_IPSCANDETECTOR,
	MNU_MYSQLSERVER,
	MNU_PASSWORDHASH,
	MNU_MYSQLCLIENT,
	MNU_WOL,
	MNU_MQTT_EXPLORER,
	MNU_ARPPING,
	MNU_COVID19,
	MNU_LDAPCLIENT,
	MNU_LDAPEXPLORER,
	MNU_RESTFUL,
	MNU_OPEN_ODBC_STR,
	MNU_COORD_INFO,
	MNU_MSSQL_CONN,
	MNU_DB_MANAGER,
	MNU_TIMED_FILE_COPY
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRBaseForm::FileHandler(void *userObj, const UTF8Char **files, OSInt nFiles)
{
	SSWR::AVIRead::AVIRBaseForm *me = (AVIRead::AVIRBaseForm*)userObj;
	IO::Path::PathType pt;
	IO::StmData::FileData *fd;
	IO::DirectoryPackage *pkg;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char *)"Cannot parse:");
	Bool found = false;
	me->core->BeginLoad();
	OSInt i = 0;
	while (i < nFiles)
	{
		pt = IO::Path::GetPathType(files[i]);
		if (pt == IO::Path::PT_DIRECTORY)
		{
			Bool valid = false;
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
			if (Text::StrEndsWithICase(files[i], (const UTF8Char*)".GDB"))
			{
				DB::OLEDBConn *conn;
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Provider=ESRI.GeoDB.OleDB.1;Data Source=");
				sb.Append(files[i]);
				sb.Append((const UTF8Char*)";Extended Properties=workspacetype=esriDataSourcesGDB.FileGDBWorkspaceFactory.1");
				sb.Append((const UTF8Char*)";Geometry=OBJECT");
				const WChar *wptr = Text::StrToWCharNew(sb.ToString());
				NEW_CLASS(conn, DB::OLEDBConn(wptr, 0));
				Text::StrDelNew(wptr);
				if (conn->GetConnError() == DB::OLEDBConn::CE_NONE)
				{
					valid = true;
					me->core->OpenObject(conn);
				}
				else
				{
					DEL_CLASS(conn);
				}
			}
#endif
			if (!valid)
			{
				NEW_CLASS(pkg, IO::DirectoryPackage(files[i]));
				Parser::ParserList *parsers = me->core->GetParserList();
				IO::ParsedObject::ParserType pt = IO::ParsedObject::PT_UNKNOWN;
				IO::ParsedObject *pobj = parsers->ParseObject(pkg, &pt);
				if (pobj)
				{
					DEL_CLASS(pkg);
					me->core->OpenObject(pobj);
				}
				else
				{
					me->core->OpenObject(pkg);
				}
			}
		}
		else if (pt == IO::Path::PT_FILE)
		{
			NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
			if (!me->core->LoadData(fd, 0))
			{
				sb.Append((const UTF8Char *)"\n");
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
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char *)"Error", me);
	}
}

SSWR::AVIRead::AVIRBaseForm::AVIRBaseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 480, ui)
{
	this->core = core;
#if defined(CPU_X86_32)
	this->SetText((const UTF8Char*)"AVIRead (x86 32-bit)");
#elif defined(CPU_X86_64)
	this->SetText((const UTF8Char*)"AVIRead (x86 64-bit)");
#elif defined(CPU_ARM)
	this->SetText((const UTF8Char*)"AVIRead (ARM 32-bit)");
#elif defined(CPU_ARM64)
	this->SetText((const UTF8Char*)"AVIRead (ARM 64-bit)");
#elif defined(CPU_MIPS)
	this->SetText((const UTF8Char*)"AVIRead (MIPS 32-bit)");
#else
	this->SetText((const UTF8Char*)"AVIRead");
#endif
	this->HandleDropFiles(FileHandler, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	UI::GUIMenu *mnu;
	UI::GUIMenu *mnu2;

	//RegisterDragDrop(this->hWnd, this);

	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"M&isc");
	mnu->AddItem((const UTF8Char*)"Open File", MNU_OPEN_FILE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Sudoku Solver", MNU_SUDOKU_SOLVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Process Info", MNU_PROC_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Clipboard Viewer", MNU_CLIPBOARD_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Drag Drop Viewer", MNU_DRAG_DROP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Paint Counter", MNU_PAINT_CNT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
#if defined(WIN32)
	mnu->AddItem((const UTF8Char*)"WMI Browser", MNU_WMI, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
#endif
	mnu->AddItem((const UTF8Char*)"Chinese", MNU_CHINESE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"GUI Event", MNU_GUIEVENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Calculator");
	mnu2->AddItem((const UTF8Char*)"Simple Calculator", MNU_CALCULATOR1, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"COVID-19", MNU_COVID19, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Protocol Decoder", MNU_PROTODEC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Text Encrypt", MNU_TEXT_ENCRYPT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Assembly Convert", MNU_ASM_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Javascript Text", MNU_JSTEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Password Hash", MNU_PASSWORDHASH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Brute Force", MNU_BRUTEFORCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Hash Speed", MNU_HASHTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"RAM Speed", MNU_BENCHMARK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Thread Speed", MNU_THREADSPEED, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Performance Log", MNU_PERFORMANCELOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Database");
	mnu->AddItem((const UTF8Char*)"Database Manager", MNU_DB_MANAGER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Open &ODBC DSN", MNU_OPEN_ODBC_DSN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Open &ODBC String", MNU_OPEN_ODBC_STR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Open O&LE DB", MNU_OLEDB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Open MSSQL Conn", MNU_MSSQL_CONN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&IO");
	mnu->AddItem((const UTF8Char*)"Stream Converter", MNU_STREAM_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Stream Terminal", MNU_STMTERM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Stream Echo", MNU_STREAMECHO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Stream Latency", MNU_STREAMLATENCY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Text Viewer", MNU_TEXT_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"File Extractor", MNU_FILEEX, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"File Hash", MNU_FILE_HASH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"File Size Pack", MNU_FILE_SIZE_PACK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"File Analyse", MNU_FILE_ANALYSE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Line Counter", MNU_LINECOUNTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"XML Walk", MNU_XMLWALK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Well Format", MNU_WELLFORMAT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Log Merge", MNU_LOGMERGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Log Backup", MNU_LOG_BACKUP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Log Extract", MNU_LOGEXTRACT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Timed File Copy", MNU_TIMED_FILE_COPY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Setting");
	mnu->AddItem((const UTF8Char*)"Set &CodePage", MNU_SET_CODEPAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Set &Monitor Color", MNU_SET_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Set &Audio Device", MNU_SET_AUDIO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Set Monitor &DPI", MNU_SET_DPI, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Set &Location Service", MNU_SETLOCATIONSVC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Network");
	mnu->AddItem((const UTF8Char*)"Network Info", MNU_NET_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"UDP Capture", MNU_UDP_CAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Wi-Fi");
	mnu2->AddItem((const UTF8Char*)"Wi-Fi Scan", MNU_WIFISCAN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Wi-Fi Capture", MNU_WIFICAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Wi-Fi Capture Lite", MNU_WIFICAPTURELITE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Wi-Fi Log Manager", MNU_WIFILOGMANAGER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"RAW Monitor");
	mnu2->AddItem((const UTF8Char*)"Ping Monitor", MNU_PINGMONITOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"RAW Monitor", MNU_RAWMONITOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"RAW Capture", MNU_NETRAWCAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"HTTP");
	mnu2->AddItem((const UTF8Char*)"HTTP Server", MNU_HTTP_SVR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"HTTP Client", MNU_HTTPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"HTTP Downloader", MNU_HTTPDOWNLOADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"HTTP Proxy Client", MNU_HTTPPROXYCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
//	mnu2->AddItem((const UTF8Char*)"Proxy Server", MNU_PROXYSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"RESTful Server", MNU_RESTFUL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"NTP");
	mnu2->AddItem((const UTF8Char*)"NTP Server", MNU_NTPSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"NTP Client", MNU_NTPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"DNS");
	mnu2->AddItem((const UTF8Char*)"DNS Client", MNU_DNSCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"DNS Proxy", MNU_DNSPROXY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"TFTP");
	mnu2->AddItem((const UTF8Char*)"TFTP Client", MNU_TFTPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"TFTP Server", MNU_TFTPSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"MQTT");
	mnu2->AddItem((const UTF8Char*)"MQTT Broker", MNU_MQTT_BROKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"MQTT Publish", MNU_MQTT_PUBLISH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"MQTT Subscribe", MNU_MQTT_SUBSCRIBE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"MQTT Explorer", MNU_MQTT_EXPLORER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"SNMP");
	mnu2->AddItem((const UTF8Char*)"SNMP Client", MNU_SNMPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SNMP Trap Monitor", MNU_SNMPTRAPMON, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SNMP MIB", MNU_SNMPMIB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SNMP Manager", MNU_SNMPMANAGER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"LDAP");
	mnu2->AddItem((const UTF8Char*)"LDAP Client", MNU_LDAPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"LDAP Explorer", MNU_LDAPEXPLORER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"MySQL");
	mnu2->AddItem((const UTF8Char*)"MySQL Client", MNU_MYSQLCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"MySQL Server", MNU_MYSQLSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"WHOIS Client", MNU_WHOISCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"DHCP Server", MNU_DHCPSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"SMTP Server", MNU_SMTPSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"RSS Reader", MNU_RSSREADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"eGauge Server", MNU_EGAUGESVR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Log Server", MNU_LOGSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Syslog Server", MNU_SYSLOGSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"SNS Manager", MNU_SNSMANAGER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"WebSite");
	mnu2->AddItem((const UTF8Char*)"48Idol", MNU_WEBSITE_48IDOL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Twitter", MNU_WEBSITE_TWITTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"755", MNU_WEBSITE_7GOGO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Instagram", MNU_WEBSITE_INSTAGRAM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Wake-On-LAN", MNU_WOL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Other");
	mnu2->AddItem((const UTF8Char*)"GPSDev Viewer", MNU_GPSDEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"JT/T808 Server", MNU_JTT808, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Throughput Test");
	mnu2->AddItem((const UTF8Char*)"TCP Conn Test", MNU_TCPTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"TCP Speed Server", MNU_TCPSPDSVR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"TCP Speed Client", MNU_TCPSPDCLI, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"HTTP Test", MNU_HTTPTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"UDP Test", MNU_UDPTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"ICMP Utils");
	mnu2->AddItem((const UTF8Char*)"IP Scan", MNU_IPSCAN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Trace Route", MNU_TRACEROUTE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Ping", MNU_NET_PING, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"ARP Utils");
	mnu2->AddItem((const UTF8Char*)"ARP Scan", MNU_ARPSCAN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"IP Scan Detector", MNU_IPSCANDETECTOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"ARP Ping", MNU_ARPPING, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"TCP Port Scan", MNU_TCPPORTSCAN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"MAC Manager", MNU_MACMANAGER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"MAC Generator", MNU_MACGEN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"User Agent");
	mnu2->AddItem((const UTF8Char*)"User Agent Parse", MNU_USERAGENTPARSE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"User Agent Batch", MNU_USERAGENTBATCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"M&edia");
	mnu->AddItem((const UTF8Char*)"&Generate Image", MNU_GEN_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Media Playlist", MNU_PLAYLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Audio Filter", MNU_AUDIOFILTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Image Batch Tool", MNU_IMAGE_BATCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Profiled Resizer", MNU_PROF_RESIZER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"HQMP3", MNU_HQMP3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"HQMP3HQ", MNU_HQMP3HQ, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"ICC Info", MNU_ICCINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Code Image Generator", MNU_CODEIMAGEGEN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"EDID Viewer", MNU_EDIDVIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Image Viewer", MNU_IMAGEVIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Timed Capture", MNU_TIMEDCAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Screen Capture", MNU_SCREENCAPTURE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_PRINTSCREEN);
	mnu->AddItem((const UTF8Char*)"Video Info", MNU_VIDEOINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Video Checker", MNU_VIDEOCHECKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Image PSNR", MNU_IMAGE_PSNR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Image Batch Convert", MNU_IMAGE_BATCH_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Map");
	mnu->AddItem((const UTF8Char*)"Add ESRI Tile Map", MNU_ESRI_MAP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Add &OSM Layer");
	mnu2->AddItem((const UTF8Char*)"OSM Tile", MNU_OSM_TILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Open Cycle Map", MNU_OSM_CYCLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Transport", MNU_OSM_TRANSP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"MapQuest", MNU_OSM_MAPQUEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Landscape", MNU_OSM_LANDSCAPE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Outdoors", MNU_OSM_OUTDOORS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Transport Dark", MNU_OSM_TRANSP_DARK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Spinal Map", MNU_OSM_SPINAL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddSeperator();
	mnu2->AddItem((const UTF8Char*)"Local file (Dir)...", MNU_OSM_LOCAL_DIR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Local file (File)...", MNU_OSM_LOCAL_FILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
//	mnu2->AddItem((const UTF8Char*)"OSM Cache Server Test", MNU_TEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Coord Converter", MNU_COORD_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Coordinate System Info", MNU_COORD_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"OSM Cache Server", MNU_OSM_CACHE_SERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"SHPConv", MNU_SHPCONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Device");
	mnu->AddItem((const UTF8Char*)"&Capture Device", MNU_CAP_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"&WIA Device", MNU_WIA_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"CP&U Info", MNU_CPUINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"GPU Info", MNU_GPUINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Bluetooth", MNU_BLUETOOTH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Bluetooth LE", MNU_BLUETOOTHLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Accelerometer", MNU_ACCELEROMETER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Power Control", MNU_POWERCONTROL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"IOPin");
	mnu2->AddItem((const UTF8Char*)"GPIO Status", MNU_GPIOSTATUS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"IOPin Test", MNU_IOPINTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"DHT22 / AM2305", MNU_DHT22, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"DS18B20", MNU_DS18B20, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"PCI Devices", MNU_PCIDEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"USB Devices", MNU_USBDEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"HID Devices", MNU_HIDDEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"SD Cards", MNU_SDCARD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Timing Test", MNU_TIMETEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"&TV Control", MNU_TV_CONTROL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"i-Buddy", MNU_IBUDDY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Electronic Scale", MNU_ESCALE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Denso Wave QB-30", MNU_DWQB30, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"SnB Dongle", MNU_SNB_DONGLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"&MTK GPS Tracker", MNU_MTK_GPS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"&GSM Modem", MNU_GSM_MODEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"&Voice Modem", MNU_VOICEMODEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"MODBUS Master", MNU_MODBUSMASTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"G&PS Tracker", MNU_GPS_TRACKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Camera Control");
	mnu2->AddItem((const UTF8Char*)"Any Camera", MNU_CAMERACONTROLANY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Olympus Camera", MNU_OLYMPUSCAMERA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"GoPro Camera", MNU_GOPROCAMERA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Help");
	mnu->AddItem((const UTF8Char*)"&About...", MNU_ABOUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	this->SetMenu(this->mnuMain);
}

SSWR::AVIRead::AVIRBaseForm::~AVIRBaseForm()
{
	this->core->CloseAllForm();
}

void SSWR::AVIRead::AVIRBaseForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char u8buff[512];
	UTF8Char u8buff2[16];
	UTF8Char *u8ptr;
	Map::TileMap *tileMap;
	Map::IMapDrawLayer *mapLyr;

	switch (cmdId)
	{
	case MNU_ABOUT:
		{
			SSWR::AVIRead::AVIRAboutForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRAboutForm(0, this->ui, this->core));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}
		break;
	case MNU_OPEN_ODBC_DSN:
		{
			SSWR::AVIRead::AVIRODBCDSNForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRODBCDSNForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenObject(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_OPEN_ODBC_STR:
		{
			SSWR::AVIRead::AVIRODBCStrForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRODBCStrForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenObject(dlg->GetDBConn());
			}
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
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRColorSettingForm(0, this->ui, this->core, this->GetHMonitor()));
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
		}
		break;
	case MNU_CAP_DEV:
		{
			SSWR::AVIRead::AVIRCaptureDevForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRCaptureDevForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Media::MediaFile *mf;
				dlg->capture->GetSourceName(u8buff);
				NEW_CLASS(mf, Media::MediaFile(u8buff));
				mf->AddSource(dlg->capture, 0);
				this->core->OpenObject(mf);
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_MTK_GPS:
		{
			IO::SerialPort *port;
			IO::Device::MTKGPSNMEA *mtk;
			Map::GPSTrack *trk;

			NEW_CLASS(port, IO::SerialPort(IO::Device::MTKGPSNMEA::GetMTKSerialPort(), 115200, IO::SerialPort::PARITY_NONE, true));
			if (!port->IsError())
			{
				NEW_CLASS(mtk, IO::Device::MTKGPSNMEA(port, false));
				if (mtk->IsMTKDevice())
				{
					NEW_CLASS(trk, Map::GPSTrack((const UTF8Char*)"MTK_Tracker", true, 0, 0));
					if (mtk->ParseLog(trk))
					{
						core->OpenObject(trk);
					}
					else
					{
						UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in parsing log", (const UTF8Char *)"MTK Tracker", this);
						DEL_CLASS(trk);
					}
				}
				else
				{
					UI::MessageDialog::ShowDialog((const UTF8Char *)"MTK Tracker not found", (const UTF8Char *)"MTK Tracker", this);
				}
				DEL_CLASS(mtk);
				DEL_CLASS(port);
			}
		}
		break;
	case MNU_GSM_MODEM:
		{
			SSWR::AVIRead::AVIRSelStreamForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this->core, false));
			frm->SetText((const UTF8Char*)"Select GSM Modem");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenGSMModem(frm->stm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_GPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelStreamForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this->core, true));
			frm->SetText((const UTF8Char*)"Select GPS Tracker");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRGPSTrackerForm *gpsFrm;
				IO::GPSNMEA *gps;
				NEW_CLASS(gps, IO::GPSNMEA(frm->stm, true));
				NEW_CLASS(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(0, this->ui, this->core, gps, true));
				this->core->ShowForm(gpsFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_WIA_DEV:
		{
			SSWR::AVIRead::AVIRWIADevForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWIADevForm(0, this->ui, this->core));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_OSM_TILE:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMTile");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.openstreetmap.org/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.openstreetmap.org/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.openstreetmap.org/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_CYCLE:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMOpenCycleMap");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/cycle/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/cycle/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/cycle/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_TRANSP:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMTransport");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/transport/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/transport/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/transport/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_LANDSCAPE:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMLandscape");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/landscape/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/landscape/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/landscape/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_OUTDOORS:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMOutdoors");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/outdoors/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/outdoors/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/outdoors/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_TRANSP_DARK:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMTransportDark");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/transport-dark/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/transport-dark/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/transport-dark/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_SPINAL:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMSpinalMap");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.thunderforest.com/spinal-map/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://b.tile.thunderforest.com/spinal-map/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://c.tile.thunderforest.com/spinal-map/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_MAPQUEST:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMMapQuest");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://otile1.mqcdn.com/tiles/1.0.0/osm/", u8buff, 18, this->core->GetSocketFactory()));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://otile2.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://otile3.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL((const UTF8Char*)"http://otile4.mqcdn.com/tiles/1.0.0/osm/");
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_ESRI_MAP:
		{
			SSWR::AVIRead::AVIRESRIMapForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRESRIMapForm(0, this->ui, this->core));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Crypto::Hash::CRC32R crc(Crypto::Hash::CRC32R::GetPolynormialIEEE());
				Map::ESRI::ESRITileMap *map;
				UInt8 crcVal[4];
				const UTF8Char *url = frm->GetSelectedURL();
				crc.Calc((UInt8*)url, Text::StrCharCnt(url) * sizeof(WChar));
				crc.GetValue(crcVal);
				IO::Path::GetProcessFileName(u8buff);
				Text::StrInt32(u8buff2, ReadMInt32(crcVal));
				u8ptr = IO::Path::AppendPath(u8buff, u8buff2);
				*u8ptr++ = (UTF8Char)IO::Path::PATH_SEPERATOR;
				*u8ptr = 0;
				NEW_CLASS(map, Map::ESRI::ESRITileMap(url, u8buff, this->core->GetSocketFactory()));
				NEW_CLASS(mapLyr, Map::TileMapLayer(map, this->core->GetParserList()));
				this->core->OpenObject(mapLyr);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_TEXT_VIEWER:
		{
			UtilUI::TextViewerForm *frm;
			NEW_CLASS(frm, UtilUI::TextViewerForm(0, this->ui, this->core->GetMonitorMgr(), this->core->GetCurrCodePage()));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_NET_INFO:
		{
			SSWR::AVIRead::AVIRNetInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNetInfoForm(0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_SUDOKU_SOLVER:
		{
			SSWR::AVIRead::AVIRSudokuForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSudokuForm(0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_OPEN_FILE:
		{
			SSWR::AVIRead::AVIROpenFileForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIROpenFileForm(0, this->ui, this->core, IO::ParsedObject::PT_UNKNOWN));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				const UTF8Char *fname = frm->GetFileName();
				OSInt i = Text::StrIndexOf(fname, ':');
				if (i < 0 || i == 1)
				{
					if (IO::Path::GetPathType(fname) == IO::Path::PT_DIRECTORY)
					{
						IO::DirectoryPackage *dp;
						NEW_CLASS(dp, IO::DirectoryPackage(fname));
						this->core->OpenObject(dp);	
					}
					else
					{
						IO::StmData::FileData *fd;
						NEW_CLASS(fd, IO::StmData::FileData(fname, false));
						if (fd->GetDataSize() == 0)
						{
							UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in loading file", (const UTF8Char*)"AVIRead", this);
						}
						else
						{
							this->core->LoadData(fd, 0);
						}
						DEL_CLASS(fd);
					}
				}
				else
				{
					IO::ParsedObject *pobj = Net::URL::OpenObject(fname, 0, this->core->GetSocketFactory());
					if (pobj == 0)
					{
						UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in loading file", (const UTF8Char *)"AVIRead", this);
					}
					else
					{
						this->core->OpenObject(pobj);
					}
				}
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_PLAYLIST:
		{
			Media::Playlist *playlist;
			NEW_CLASS(playlist, Media::Playlist((const UTF8Char*)"Untitled", this->core->GetParserList()));
			this->core->OpenObject(playlist);
		}
		break;
	case MNU_HTTP_SVR:
		{
			SSWR::AVIRead::AVIRHTTPSvrForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHTTPSvrForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TV_CONTROL:
		{
			SSWR::AVIRead::AVIRTVControlForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTVControlForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STMTERM:
		{
			SSWR::AVIRead::AVIRStreamTermForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStreamTermForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NTPCLIENT:
		{
			SSWR::AVIRead::AVIRNTPClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNTPClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SET_AUDIO:
		{
			SSWR::AVIRead::AVIRSetAudioForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSetAudioForm(0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_FILE_ANALYSE:
		{
			SSWR::AVIRead::AVIRFileAnalyseForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFileAnalyseForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROC_INFO:
		{
			SSWR::AVIRead::AVIRProcInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRProcInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SET_DPI:
		{
			SSWR::AVIRead::AVIRSetDPIForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSetDPIForm(0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
		break;
	case MNU_DNSCLIENT:
		{
			SSWR::AVIRead::AVIRDNSClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDNSClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILEEX:
		{
			SSWR::AVIRead::AVIRFileExForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFileExForm(0, this->ui, this->core));
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
				UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in starting DNS proxy. Port is in use?", (const UTF8Char *)"Error", this);
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
			NEW_CLASS(frm, SSWR::AVIRead::AVIREncryptForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAM_CONV:
		{
			SSWR::AVIRead::AVIRStreamConvForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStreamConvForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HQMP3:
		{
			SSWR::AVIRead::AVIRHQMPForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHQMPForm(0, this->ui, this->core, SSWR::AVIRead::AVIRHQMPForm::QM_LQ));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HQMP3HQ:
		{
			SSWR::AVIRead::AVIRHQMPForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHQMPForm(0, this->ui, this->core, SSWR::AVIRead::AVIRHQMPForm::QM_HQ));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HQMP3UQ:
		{
			SSWR::AVIRead::AVIRHQMPForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHQMPForm(0, this->ui, this->core, SSWR::AVIRead::AVIRHQMPForm::QM_UQ));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CPUINFO:
		{
			SSWR::AVIRead::AVIRCPUInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCPUInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IBUDDY:
		{
			SSWR::AVIRead::AVIRIBuddyForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRIBuddyForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DWQB30:
		{
			SSWR::AVIRead::AVIRDWQB30Form *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDWQB30Form(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ICCINFO:
		{
			SSWR::AVIRead::AVIRICCInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRICCInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CLIPBOARD_VIEWER:
		{
			SSWR::AVIRead::AVIRClipboardViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRClipboardViewerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BENCHMARK:
		{
			SSWR::AVIRead::AVIRBenchmarkForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRBenchmarkForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILE_SIZE_PACK:
		{
			SSWR::AVIRead::AVIRFileSizePackForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFileSizePackForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PAINT_CNT:
		{
			SSWR::AVIRead::AVIRPaintCntForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPaintCntForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ASM_CONV:
		{
			SSWR::AVIRead::AVIRAsmConvForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRAsmConvForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_COORD_CONV:
		{
			SSWR::AVIRead::AVIRCoordConvForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCoordConvForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_GPSDEV:
		{
			SSWR::AVIRead::AVIRGPSDevForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGPSDevForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPDOWNLOADER:
		{
			SSWR::AVIRead::AVIRHTTPDownloaderForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHTTPDownloaderForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DRAG_DROP:
		{
			SSWR::AVIRead::AVIRDragDropViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDragDropViewerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_JSTEXT:
		{
			SSWR::AVIRead::AVIRJSTextForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRJSTextForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_UDPTEST:
		{
			SSWR::AVIRead::AVIRUDPTestForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRUDPTestForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CODEIMAGEGEN:
		{
			SSWR::AVIRead::AVIRCodeImageGenForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCodeImageGenForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_EDIDVIEWER:
		{
			SSWR::AVIRead::AVIREDIDViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIREDIDViewerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_GPUINFO:
		{
			SSWR::AVIRead::AVIRGPUInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGPUInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WMI:
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
		{
			SSWR::AVIRead::AVIRWMIForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWMIForm(0, this->ui, this->core));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
		}
#endif
		break;
	case MNU_SNB_DONGLE:
		{
			SSWR::AVIRead::AVIRSelStreamForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this->core, false));
			frm->SetText((const UTF8Char*)"Select SNB Dongle");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRSNBDongleForm *snbFrm;
				NEW_CLASS(snbFrm, SSWR::AVIRead::AVIRSNBDongleForm(0, this->ui, this->core, frm->stm));
				this->core->ShowForm(snbFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_CHINESE:
		{
			SSWR::AVIRead::AVIRChineseForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRChineseForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BLUETOOTH:
		{
			SSWR::AVIRead::AVIRBluetoothForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRBluetoothForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OSM_CACHE_SERVER:
		{
			SSWR::AVIRead::AVIROSMCacheCfgForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIROSMCacheCfgForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TEST:
		IO::Path::GetProcessFileName(u8buff);
		IO::Path::AppendPath(u8buff, (const UTF8Char*)"OSMCacheTest");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://127.0.0.1/", u8buff, 18, this->core->GetSocketFactory()));
		NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_IMAGEVIEWER:
		{
			SSWR::AVIRead::AVIRImageViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRImageViewerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LINECOUNTER:
		{
			SSWR::AVIRead::AVIRLineCounterForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLineCounterForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OSM_LOCAL_DIR:
		{
			UI::FolderDialog *dlg;
			NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"OSMLocal"));
			if (dlg->ShowDialog(this->GetHandle()))
			{
				IO::DirectoryPackage *pkg;
				NEW_CLASS(pkg, IO::DirectoryPackage(dlg->GetFolder()));
				NEW_CLASS(tileMap, Map::OSM::OSMLocalTileMap(pkg));
				NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
				this->core->OpenObject(mapLyr);
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_OSM_LOCAL_FILE:
		{
			UI::FileDialog *dlg;
			Parser::ParserList *parsers = this->core->GetParserList();
			NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"OSMLocalFile", false));
			parsers->PrepareSelector(dlg, IO::ParsedObject::PT_PACKAGE_PARSER);
			if (dlg->ShowDialog(this->GetHandle()))
			{
				IO::StmData::FileData *fd;
				IO::PackageFile *pkg;
				NEW_CLASS(fd, IO::StmData::FileData(dlg->GetFileName(), false));
				pkg = (IO::PackageFile*)parsers->ParseFileType(fd, IO::ParsedObject::PT_PACKAGE_PARSER);
				DEL_CLASS(fd);
				if (pkg)
				{
					NEW_CLASS(tileMap, Map::OSM::OSMLocalTileMap(pkg));
					NEW_CLASS(mapLyr, Map::TileMapLayer(tileMap, parsers));
					this->core->OpenObject(mapLyr);
				}
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_TCPSPDCLI:
		{
			SSWR::AVIRead::AVIRTCPSpdCliForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTCPSpdCliForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TCPSPDSVR:
		{
			SSWR::AVIRead::AVIRTCPSpdSvrForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTCPSpdSvrForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ACCELEROMETER:
		{
			IO::SensorManager sensors;
			UOSInt cnt = sensors.GetAccelerometerCnt();
			if (cnt > 0)
			{
				IO::SensorAccelerometer *acc = sensors.CreateAccelerometer(0);
				if (acc)
				{
					SSWR::AVIRead::AVIRAccelerometerForm *frm;
					NEW_CLASS(frm, SSWR::AVIRead::AVIRAccelerometerForm(0, this->ui, this->core, acc));
					this->core->ShowForm(frm);
				}
				else
				{
					UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in accessing accelerometer", (const UTF8Char *)"Accelerometer", this);
				}
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char *)"No accelerometers found", (const UTF8Char *)"Accelerometer", this);
			}
		}
		break;
	case MNU_WIFICAPTURE:
		{
			SSWR::AVIRead::AVIRWifiCaptureForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWifiCaptureForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_POWERCONTROL:
		{
			SSWR::AVIRead::AVIRPowerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPowerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TIMETEST:
		{
			SSWR::AVIRead::AVIRTimeTestForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTimeTestForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IOPINTEST:
		{
			SSWR::AVIRead::AVIRSelIOPinForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelIOPinForm(0, this->ui, this->core));
			frm->SetText((const UTF8Char*)"Select IO Pin");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRIOPinTestForm *testFrm;
				NEW_CLASS(testFrm, SSWR::AVIRead::AVIRIOPinTestForm(0, this->ui, this->core, frm->ioPin));
				this->core->ShowForm(testFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_DHT22:
		{
			SSWR::AVIRead::AVIRSelIOPinForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelIOPinForm(0, this->ui, this->core));
			frm->SetText((const UTF8Char*)"Select DHT22 IO Pin");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRDHT22Form *testFrm;
				NEW_CLASS(testFrm, SSWR::AVIRead::AVIRDHT22Form(0, this->ui, this->core, frm->ioPin));
				this->core->ShowForm(testFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_DS18B20:
		{
			SSWR::AVIRead::AVIRSelIOPinForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelIOPinForm(0, this->ui, this->core));
			frm->SetText((const UTF8Char*)"Select DS18B20 IO Pin");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRDS18B20Form *testFrm;
				NEW_CLASS(testFrm, SSWR::AVIRead::AVIRDS18B20Form(0, this->ui, this->core, frm->ioPin));
				this->core->ShowForm(testFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_TIMEDCAPTURE:
		{
			SSWR::AVIRead::AVIRTimedCaptureForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTimedCaptureForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_GUIEVENT:
		{
			SSWR::AVIRead::AVIRGUIEventForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGUIEventForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CALCULATOR1:
		{
			SSWR::AVIRead::AVIRCalculator1Form *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCalculator1Form(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SETLOCATIONSVC:
		{
			SSWR::AVIRead::AVIRSetLocationSvcForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSetLocationSvcForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SCREENCAPTURE:
		{
			Media::ScreenCapturer *capturer;
			Media::StaticImage *img;
			NEW_CLASS(capturer, Media::ScreenCapturer(this->core->GetMonitorMgr(), this->core->GetColorMgr()));
			img = capturer->CaptureScreen(this->GetHMonitor());
			DEL_CLASS(capturer);
			if (img)
			{
				Media::ImageList *imgList;
				NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"ScreenCapture"));
				imgList->AddImage(img, 0);
				this->core->OpenObject(imgList);
			}
		}
		break;
	case MNU_HASHTEST:
		{
			SSWR::AVIRead::AVIRHashTestForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHashTestForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_THREADSPEED:
		{
			SSWR::AVIRead::AVIRThreadSpeedForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRThreadSpeedForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ESCALE:
		{
			SSWR::AVIRead::AVIRElectronicScaleForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRElectronicScaleForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_VOICEMODEM:
		{
			SSWR::AVIRead::AVIRSelStreamForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this->core, false));
			frm->SetText((const UTF8Char*)"Select Voice Modem");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRVoiceModemForm *innerFrm;
				IO::Device::RockwellModemController *modem;
				IO::ATCommandChannel *channel;
				NEW_CLASS(channel, IO::ATCommandChannel(frm->stm, false));
				NEW_CLASS(modem, IO::Device::RockwellModemController(channel, false));

				NEW_CLASS(innerFrm, SSWR::AVIRead::AVIRVoiceModemForm(0, this->ui, this->core, modem, channel, frm->stm));
				this->core->ShowForm(innerFrm);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_PERFORMANCELOG:
		{
			SSWR::AVIRead::AVIRPerformanceLogForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPerformanceLogForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_JTT808:
		{
			SSWR::AVIRead::AVIRJTT808ServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRJTT808ServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_BROKER:
		{
			SSWR::AVIRead::AVIRMQTTBrokerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMQTTBrokerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_PUBLISH:
		{
			SSWR::AVIRead::AVIRMQTTPublishForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMQTTPublishForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_SUBSCRIBE:
		{
			SSWR::AVIRead::AVIRMQTTSubscribeForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMQTTSubscribeForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OLEDB:
		{
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
			SSWR::AVIRead::AVIROLEDBForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIROLEDBForm(0, this->ui, this->core));
			if (frm->ShowDialog(this))
			{
				this->core->OpenObject(frm->GetDBConn());
			}
			DEL_CLASS(frm);
#endif
		}
		break;
	case MNU_PCIDEVICE:
		{
			SSWR::AVIRead::AVIRPCIDeviceForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPCIDeviceForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_USBDEVICE:
		{
			SSWR::AVIRead::AVIRUSBDeviceForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRUSBDeviceForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SDCARD:
		{
			SSWR::AVIRead::AVIRSDCardForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSDCardForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SHPCONV:
		{
			SSWR::SHPConv::SHPConvMainForm *frm;
			NEW_CLASS(frm, SSWR::SHPConv::SHPConvMainForm(0, this->ui, this->core->GetDrawEngine(), this->core->GetMonitorMgr()));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOGMERGE:
		{
			SSWR::AVIRead::AVIRLogMergeForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLogMergeForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOGEXTRACT:
		{
			SSWR::AVIRead::AVIRLogExtractForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLogExtractForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_VIDEOINFO:
		{
			SSWR::AVIRead::AVIRVideoInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRVideoInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_VIDEOCHECKER:
		{
			SSWR::AVIRead::AVIRVideoCheckerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRVideoCheckerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HIDDEVICE:
		{
			SSWR::AVIRead::AVIRHIDDeviceForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRHIDDeviceForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IMAGE_PSNR:
		{
			SSWR::AVIRead::AVIRImagePSNRForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRImagePSNRForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IMAGE_BATCH_CONV:
		{
			SSWR::AVIRead::AVIRImageBatchConvForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRImageBatchConvForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MODBUSMASTER:
		{
			SSWR::AVIRead::AVIRMODBUSMasterForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMODBUSMasterForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAMECHO:
		{
			SSWR::AVIRead::AVIRStreamEchoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStreamEchoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAMLATENCY:
		{
			SSWR::AVIRead::AVIRStreamLatencyForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStreamLatencyForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_EGAUGESVR:
		{
			SSWR::AVIRead::AVIReGaugeSvrForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIReGaugeSvrForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOGSERVER:
		{
			SSWR::AVIRead::AVIRLogServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLogServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PINGMONITOR:
		{
			SSWR::AVIRead::AVIRPingMonitorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPingMonitorForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MACMANAGER:
		{
			SSWR::AVIRead::AVIRMACManagerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMACManagerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IPSCAN:
		{
			SSWR::AVIRead::AVIRIPScanForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRIPScanForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BLUETOOTHLE:
		{
			SSWR::AVIRead::AVIRBluetoothLEForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRBluetoothLEForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BRUTEFORCE:
		{
			SSWR::AVIRead::AVIRBruteForceForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRBruteForceForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TRACEROUTE:
		{
			SSWR::AVIRead::AVIRTraceRouteForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTraceRouteForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_RAWMONITOR:
		{
			SSWR::AVIRead::AVIRRAWMonitorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRRAWMonitorForm(0, this->ui, this->core, 0));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TFTPCLIENT:
		{
			SSWR::AVIRead::AVIRTFTPClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTFTPClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TFTPSERVER:
		{
			SSWR::AVIRead::AVIRTFTPServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTFTPServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NTPSERVER:
		{
			SSWR::AVIRead::AVIRNTPServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNTPServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TCPPORTSCAN:
		{
			SSWR::AVIRead::AVIRTCPPortScanForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTCPPortScanForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DHCPSERVER:
		{
			SSWR::AVIRead::AVIRDHCPServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDHCPServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SNMPCLIENT:
		{
			SSWR::AVIRead::AVIRSNMPClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSNMPClientForm(0, this->ui, this->core));
			if (frm->IsError())
			{
				DEL_CLASS(frm);
			}
			else
			{
				this->core->ShowForm(frm);
			}
		}
		break;
	case MNU_SNMPTRAPMON:
		{
			SSWR::AVIRead::AVIRSNMPTrapMonitorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSNMPTrapMonitorForm(0, this->ui, this->core));
			if (frm->IsError())
			{
				DEL_CLASS(frm);
			}
			else
			{
				this->core->ShowForm(frm);
			}
		}
		break;
	case MNU_SNMPMIB:
		{
			SSWR::AVIRead::AVIRSNMPMIBForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSNMPMIBForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SNMPMANAGER:
		{
			SSWR::AVIRead::AVIRSNMPManagerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSNMPManagerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MACGEN:
		{
			SSWR::AVIRead::AVIRMACGenForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMACGenForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_XMLWALK:
		{
			SSWR::AVIRead::AVIRXMLWalkForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRXMLWalkForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WIFISCAN:
		{
			SSWR::AVIRead::AVIRWifiScanForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWifiScanForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OLYMPUSCAMERA:
		{
			SSWR::AVIRead::AVIRCameraControlForm *frm;
			IO::CameraControl *camera = IO::Device::OlympusCameraControl::CreateControl(this->core->GetSocketFactory(), this->core->GetEncFactory());
			if (camera)
			{
				NEW_CLASS(frm, SSWR::AVIRead::AVIRCameraControlForm(0, this->ui, this->core, camera));
				this->core->ShowForm(frm);
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Olympus Camera not found", (const UTF8Char*)"Olympus Camera", this);
			}
		}
		break;
	case MNU_WIFICAPTURELITE:
		{
			SSWR::AVIRead::AVIRWifiCaptureLiteForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWifiCaptureLiteForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SYSLOGSERVER:
		{
			SSWR::AVIRead::AVIRSyslogServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSyslogServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CAMERACONTROLANY:
		{
			SSWR::AVIRead::AVIRCameraControlForm *frm;
			IO::CameraControl *camera;
			camera = IO::Device::OlympusCameraControl::CreateControl(this->core->GetSocketFactory(), this->core->GetEncFactory());
			if (camera == 0) camera = IO::Device::GoProCameraControl::CreateControl(this->core->GetSocketFactory());
			if (camera)
			{
				NEW_CLASS(frm, SSWR::AVIRead::AVIRCameraControlForm(0, this->ui, this->core, camera));
				this->core->ShowForm(frm);
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Camera not found", (const UTF8Char*)"Camera Control", this);
			}
		}
		break;
	case MNU_GOPROCAMERA:
		{
			SSWR::AVIRead::AVIRCameraControlForm *frm;
			IO::CameraControl *camera = IO::Device::GoProCameraControl::CreateControl(this->core->GetSocketFactory());
			if (camera)
			{
				NEW_CLASS(frm, SSWR::AVIRead::AVIRCameraControlForm(0, this->ui, this->core, camera));
				this->core->ShowForm(frm);
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"GoPro Camera not found", (const UTF8Char*)"GoPro Camera", this);
			}
		}
		break;
	case MNU_USERAGENTPARSE:
		{
			SSWR::AVIRead::AVIRUserAgentParseForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRUserAgentParseForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_USERAGENTBATCH:
		{
			SSWR::AVIRead::AVIRUserAgentBatchForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRUserAgentBatchForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WIFILOGMANAGER:
		{
			SSWR::AVIRead::AVIRWiFiLogManagerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWiFiLogManagerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WELLFORMAT:
		{
			SSWR::AVIRead::AVIRWellFormatForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWellFormatForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_RSSREADER:
		{
			SSWR::AVIRead::AVIRRSSReaderForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRRSSReaderForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WEBSITE_48IDOL:
		{
			SSWR::AVIRead::AVIRWebSite48IdolForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWebSite48IdolForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WEBSITE_TWITTER:
		{
			SSWR::AVIRead::AVIRWebSiteTwitterForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWebSiteTwitterForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SNSMANAGER:
		{
			SSWR::AVIRead::AVIRSNSManagerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSNSManagerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WEBSITE_7GOGO:
		{
			SSWR::AVIRead::AVIRWebSite7gogoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWebSite7gogoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WEBSITE_INSTAGRAM:
		{
			SSWR::AVIRead::AVIRWebSiteInstagramForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWebSiteInstagramForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_GPIOSTATUS:
		{
			SSWR::AVIRead::AVIRGPIOStatusForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGPIOStatusForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NETRAWCAPTURE:
		{
			SSWR::AVIRead::AVIRNetRAWCaptureForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRNetRAWCaptureForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ARPSCAN:
		{
			SSWR::AVIRead::AVIRARPScanForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRARPScanForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IPSCANDETECTOR:
		{
			SSWR::AVIRead::AVIRIPScanDetectorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRIPScanDetectorForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MYSQLSERVER:
		{
			SSWR::AVIRead::AVIRMySQLServerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMySQLServerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PASSWORDHASH:
		{
			SSWR::AVIRead::AVIRPasswordHashForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPasswordHashForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MYSQLCLIENT:
		{
			SSWR::AVIRead::AVIRMySQLClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMySQLClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WOL:
		{
			SSWR::AVIRead::AVIRWOLForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRWOLForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_EXPLORER:
		{
			SSWR::AVIRead::AVIRMQTTExplorerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMQTTExplorerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ARPPING:
		{
			SSWR::AVIRead::AVIRARPPingForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRARPPingForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_COVID19:
		{
			SSWR::AVIRead::AVIRCOVID19Form *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCOVID19Form(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LDAPCLIENT:
		{
			SSWR::AVIRead::AVIRLDAPClientForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLDAPClientForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LDAPEXPLORER:
		{
			SSWR::AVIRead::AVIRLDAPExplorerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLDAPExplorerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_RESTFUL:
		{
			SSWR::AVIRead::AVIRRESTfulForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRRESTfulForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_COORD_INFO:
		{
			SSWR::AVIRead::AVIRCoordInfoForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCoordInfoForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MSSQL_CONN:
		{
			SSWR::AVIRead::AVIRMSSQLConnForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRMSSQLConnForm(0, this->ui, this->core));
			if (dlg->IsDriverNotFound())
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"No known driver found", (const UTF8Char *)"MSSQL Conn", this);
			}
			else if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenObject(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_DB_MANAGER:
		{
			SSWR::AVIRead::AVIRDBManagerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRDBManagerForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TIMED_FILE_COPY:
		{
			SSWR::AVIRead::AVIRTimedFileCopyForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTimedFileCopyForm(0, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRBaseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
