#include "Stdafx.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
#include "DB/OLEDBConn.h"
#endif
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/SensorManager.h"
#include "IO/SerialPort.h"
#include "IO/Device/MTKGPSNMEA.h"
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
#include "SSWR/AVIRead/AVIRAsmConvForm.h"
#include "SSWR/AVIRead/AVIRAudioFilterForm.h"
#include "SSWR/AVIRead/AVIRBaseForm.h"
#include "SSWR/AVIRead/AVIRBenchmarkForm.h"
#include "SSWR/AVIRead/AVIRBluetoothForm.h"
#include "SSWR/AVIRead/AVIRBluetoothLEForm.h"
#include "SSWR/AVIRead/AVIRBruteForceForm.h"
#include "SSWR/AVIRead/AVIRCalculator1Form.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRChineseForm.h"
#include "SSWR/AVIRead/AVIRClipboardViewerForm.h"
#include "SSWR/AVIRead/AVIRCodeImageGenForm.h"
#include "SSWR/AVIRead/AVIRCodePageForm.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRCoordConvForm.h"
#include "SSWR/AVIRead/AVIRCPUInfoForm.h"
#include "SSWR/AVIRead/AVIRDHT22Form.h"
#include "SSWR/AVIRead/AVIRDNSClientForm.h"
#include "SSWR/AVIRead/AVIRDNSProxyForm.h"
#include "SSWR/AVIRead/AVIRDragDropViewerForm.h"
#include "SSWR/AVIRead/AVIRDS18B20Form.h"
#include "SSWR/AVIRead/AVIRDWQB30Form.h"
#include "SSWR/AVIRead/AVIREDIDViewerForm.h"
#include "SSWR/AVIRead/AVIReGaugeSvrForm.h"
#include "SSWR/AVIRead/AVIREmailServerForm.h"
#include "SSWR/AVIRead/AVIRElectronicScaleForm.h"
#include "SSWR/AVIRead/AVIREncryptForm.h"
#include "SSWR/AVIRead/AVIRESRIMapForm.h"
#include "SSWR/AVIRead/AVIRFileAnalyseForm.h"
#include "SSWR/AVIRead/AVIRFileExForm.h"
#include "SSWR/AVIRead/AVIRFileHashForm.h"
#include "SSWR/AVIRead/AVIRFileSizePackForm.h"
#include "SSWR/AVIRead/AVIRGenImageForm.h"
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
#include "SSWR/AVIRead/AVIRIPScanForm.h"
#include "SSWR/AVIRead/AVIRJSTextForm.h"
#include "SSWR/AVIRead/AVIRJTT808ServerForm.h"
#include "SSWR/AVIRead/AVIRLineCounterForm.h"
#include "SSWR/AVIRead/AVIRLogBackupForm.h"
#include "SSWR/AVIRead/AVIRLogExtractForm.h"
#include "SSWR/AVIRead/AVIRLogMergeForm.h"
#include "SSWR/AVIRead/AVIRLogServerForm.h"
#include "SSWR/AVIRead/AVIRMACManagerForm.h"
#include "SSWR/AVIRead/AVIRMODBUSMasterForm.h"
#include "SSWR/AVIRead/AVIRMQTTBrokerForm.h"
#include "SSWR/AVIRead/AVIRMQTTPublishForm.h"
#include "SSWR/AVIRead/AVIRMQTTSubscribeForm.h"
#include "SSWR/AVIRead/AVIRNetInfoForm.h"
#include "SSWR/AVIRead/AVIRNetPingForm.h"
#include "SSWR/AVIRead/AVIRNTPClientForm.h"
#include "SSWR/AVIRead/AVIRODBCStrForm.h"
#include "SSWR/AVIRead/AVIROLEDBForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIROSMCacheCfgForm.h"
#include "SSWR/AVIRead/AVIRPaintCntForm.h"
#include "SSWR/AVIRead/AVIRPCIDeviceForm.h"
#include "SSWR/AVIRead/AVIRPerformanceLogForm.h"
#include "SSWR/AVIRead/AVIRPingMonitorForm.h"
#include "SSWR/AVIRead/AVIRPowerForm.h"
#include "SSWR/AVIRead/AVIRProcInfoForm.h"
#include "SSWR/AVIRead/AVIRProfiledResizerForm.h"
#include "SSWR/AVIRead/AVIRProtoDecForm.h"
#include "SSWR/AVIRead/AVIRProxyServerForm.h"
#include "SSWR/AVIRead/AVIRSDCardForm.h"
#include "SSWR/AVIRead/AVIRSelIOPinForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "SSWR/AVIRead/AVIRSetDPIForm.h"
#include "SSWR/AVIRead/AVIRSetLocationSvcForm.h"
#include "SSWR/AVIRead/AVIRSNBDongleForm.h"
#include "SSWR/AVIRead/AVIRStreamConvForm.h"
#include "SSWR/AVIRead/AVIRStreamEchoForm.h"
#include "SSWR/AVIRead/AVIRStreamLatencyForm.h"
#include "SSWR/AVIRead/AVIRStreamTermForm.h"
#include "SSWR/AVIRead/AVIRSudokuForm.h"
#include "SSWR/AVIRead/AVIRTCPSpdCliForm.h"
#include "SSWR/AVIRead/AVIRTCPSpdSvrForm.h"
#include "SSWR/AVIRead/AVIRTCPTestForm.h"
#include "SSWR/AVIRead/AVIRThreadSpeedForm.h"
#include "SSWR/AVIRead/AVIRTimedCaptureForm.h"
#include "SSWR/AVIRead/AVIRTimeTestForm.h"
#include "SSWR/AVIRead/AVIRTVControlForm.h"
#include "SSWR/AVIRead/AVIRUDPCaptureForm.h"
#include "SSWR/AVIRead/AVIRUDPTestForm.h"
#include "SSWR/AVIRead/AVIRUSBDeviceForm.h"
#include "SSWR/AVIRead/AVIRVideoCheckerForm.h"
#include "SSWR/AVIRead/AVIRVideoInfoForm.h"
#include "SSWR/AVIRead/AVIRVoiceModemForm.h"
#include "SSWR/AVIRead/AVIRWHOISClientForm.h"
#include "SSWR/AVIRead/AVIRWIADevForm.h"
#include "SSWR/AVIRead/AVIRWifiCaptureForm.h"
#include "SSWR/AVIRead/AVIRWMIForm.h"
#include "SSWR/SHPConv/SHPConvMainForm.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"
#include "UI/GUITextBox.h"
//#include "UtilUI/TextViewerForm.h"
#include "Text/MyStringW.h"

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
	MNU_OPEN_ODBC,
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
	MNU_BRUTEFORCE
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRBaseForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRBaseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBaseForm>();
	IO::Path::PathType pt;
	NN<IO::StmData::FileData> fd;
	NN<IO::DirectoryPackage> pkg;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Cannot parse:"));
	Bool found = false;
	me->core->BeginLoad();
	UIntOS i = 0;
	while (i < files.GetCount())
	{
		pt = IO::Path::GetPathType(files[i]->ToCString());
		if (pt == IO::Path::PathType::Directory)
		{
			Bool valid = false;
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
			if (files[i]->EndsWithICase(UTF8STRC(".GDB")))
			{
				NN<DB::OLEDBConn> conn;
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Provider=ESRI.GeoDB.OleDB.1;Data Source="));
				sb.Append(files[i]);
				sb.AppendC(UTF8STRC(";Extended Properties=workspacetype=esriDataSourcesGDB.FileGDBWorkspaceFactory.1"));
				sb.AppendC(UTF8STRC(";Geometry=OBJECT"));
				UnsafeArray<const WChar> wptr = Text::StrToWCharNew(sb.ToString());
				NEW_CLASSNN(conn, DB::OLEDBConn(wptr, me->core->GetLog()));
				Text::StrDelNew(wptr);
				if (conn->GetConnError() == DB::OLEDBConn::CE_NONE)
				{
					valid = true;
					me->core->OpenObject(conn);
				}
				else
				{
					conn.Delete();
				}
			}
#endif
			if (!valid)
			{
				NEW_CLASSNN(pkg, IO::DirectoryPackage(files[i]->ToCString()));
				NN<Parser::ParserList> parsers = me->core->GetParserList();
				IO::ParserType pt = IO::ParserType::Unknown;
				NN<IO::ParsedObject> pobj;
				if (parsers->ParseObject(pkg).SetTo(pobj))
				{
					pkg.Delete();
					me->core->OpenObject(pobj);
				}
				else
				{
					me->core->OpenObject(pkg);
				}
			}
		}
		else if (pt == IO::Path::PathType::File)
		{
			NEW_CLASSNN(fd, IO::StmData::FileData(files[i]->ToCString(), false));
			if (!me->core->LoadData(fd, nullptr))
			{
				sb.AppendC(UTF8STRC("\n"));
				sb.Append(files[i]);
				found = true;
			}
			fd.Delete();
		}
		i++;
	}
	me->core->EndLoad();
	if (found)
	{
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("Error"), me);
	}
}

SSWR::AVIRead::AVIRBaseForm::AVIRBaseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 480, ui)
{
	this->core = core;
#if defined(CPU_X86_32)
	this->SetText(CSTR("AVIRead (x86 32-bit)"));
#elif defined(CPU_X86_64)
	this->SetText(CSTR("AVIRead (x86 64-bit)"));
#elif defined(CPU_ARM)
	this->SetText(CSTR("AVIRead (ARM 32-bit)"));
#elif defined(CPU_ARM64)
	this->SetText(CSTR("AVIRead (ARM 64-bit)"));
#elif defined(CPU_MIPS)
	this->SetText(CSTR("AVIRead (MIPS 32-bit)"));
#else
	this->SetText(CSTR("AVIRead"));
#endif
	this->HandleDropFiles(FileHandler, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NN<UI::GUIMenu> mnu;
	NN<UI::GUIMenu> mnu2;

	//RegisterDragDrop(this->hWnd, this);

	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("M&isc"));
	mnu->AddItem(CSTR("Open &ODBC"), MNU_OPEN_ODBC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Open O&LE DB"), MNU_OLEDB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Open File"), MNU_OPEN_FILE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Sudoku Solver"), MNU_SUDOKU_SOLVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Process Info"), MNU_PROC_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Text Encrypt"), MNU_TEXT_ENCRYPT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Javascript Text"), MNU_JSTEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Clipboard Viewer"), MNU_CLIPBOARD_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Drag Drop Viewer"), MNU_DRAG_DROP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Paint Counter"), MNU_PAINT_CNT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Assembly Convert"), MNU_ASM_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
#if defined(WIN32)
	mnu->AddItem(CSTR("WMI Browser"), MNU_WMI, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
#endif
	mnu->AddItem(CSTR("Chinese"), MNU_CHINESE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("GUI Event"), MNU_GUIEVENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Calculator"));
	mnu2->AddItem(CSTR("Simple Calculator"), MNU_CALCULATOR1, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Brute Force"), MNU_BRUTEFORCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Protocol Decoder"), MNU_PROTODEC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Hash Speed"), MNU_HASHTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("RAM Speed"), MNU_BENCHMARK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Thread Speed"), MNU_THREADSPEED, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Performance Log"), MNU_PERFORMANCELOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&IO"));
	mnu->AddItem(CSTR("Stream Converter"), MNU_STREAM_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Stream Terminal"), MNU_STMTERM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("File Extractor"), MNU_FILEEX, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("File Hash"), MNU_FILE_HASH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Log Backup"), MNU_LOG_BACKUP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Text Viewer"), MNU_TEXT_VIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("File Size Pack"), MNU_FILE_SIZE_PACK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("File Analyse"), MNU_FILE_ANALYSE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Line Counter"), MNU_LINECOUNTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Log Merge"), MNU_LOGMERGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Log Extract"), MNU_LOGEXTRACT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("MODBUS Master"), MNU_MODBUSMASTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Stream Echo"), MNU_STREAMECHO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Stream Latency"), MNU_STREAMLATENCY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&Setting"));
	mnu->AddItem(CSTR("Set &CodePage"), MNU_SET_CODEPAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Set &Monitor Color"), MNU_SET_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Set &Audio Device"), MNU_SET_AUDIO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Set Monitor &DPI"), MNU_SET_DPI, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Set &Location Service"), MNU_SETLOCATIONSVC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&Network"));
	mnu->AddItem(CSTR("Network Info"), MNU_NET_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("HTTP Server"), MNU_HTTP_SVR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HTTP Client"), MNU_HTTPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HTTP Downloader"), MNU_HTTPDOWNLOADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HTTP Proxy Client"), MNU_HTTPPROXYCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("NTP Client"), MNU_NTPCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("DNS Client"), MNU_DNSCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("DNS Proxy"), MNU_DNSPROXY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SMTP Server"), MNU_SMTPSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Ping"), MNU_NET_PING, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Ping Monitor"), MNU_PINGMONITOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("UDP Capture"), MNU_UDP_CAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("WHOIS Client"), MNU_WHOISCLIENT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
//	mnu->AddItem(CSTR("Proxy Server"), MNU_PROXYSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Wifi Capture"), MNU_WIFICAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("IP Scan"), MNU_IPSCAN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("MQTT"));
	mnu2->AddItem(CSTR("MQTT Broker"), MNU_MQTT_BROKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("MQTT Publish"), MNU_MQTT_PUBLISH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("MQTT Subscribe"), MNU_MQTT_SUBSCRIBE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu(CSTR("Test"));
	mnu2->AddItem(CSTR("TCP Conn Test"), MNU_TCPTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("TCP Speed Server"), MNU_TCPSPDSVR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("TCP Speed Client"), MNU_TCPSPDCLI, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("HTTP Test"), MNU_HTTPTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("UDP Test"), MNU_UDPTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Other"));
	mnu2->AddItem(CSTR("GPSDev Viewer"), MNU_GPSDEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("JT/T808 Server"), MNU_JTT808, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("eGauge Server"), MNU_EGAUGESVR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Log Server"), MNU_LOGSERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("MAC Manager"), MNU_MACMANAGER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("M&edia"));
	mnu->AddItem(CSTR("&Generate Image"), MNU_GEN_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Media Playlist"), MNU_PLAYLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Audio Filter"), MNU_AUDIOFILTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Image Batch Tool"), MNU_IMAGE_BATCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Profiled Resizer"), MNU_PROF_RESIZER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HQMP3"), MNU_HQMP3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HQMP3HQ"), MNU_HQMP3HQ, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("ICC Info"), MNU_ICCINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Code Image Generator"), MNU_CODEIMAGEGEN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("EDID Viewer"), MNU_EDIDVIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Image Viewer"), MNU_IMAGEVIEWER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Timed Capture"), MNU_TIMEDCAPTURE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Screen Capture"), MNU_SCREENCAPTURE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_PRINTSCREEN);
	mnu->AddItem(CSTR("Video Info"), MNU_VIDEOINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Video Checker"), MNU_VIDEOCHECKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Image PSNR"), MNU_IMAGE_PSNR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Image Batch Convert"), MNU_IMAGE_BATCH_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&Map"));
	mnu->AddItem(CSTR("Add ESRI Tile Map"), MNU_ESRI_MAP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Add &OSM Layer"));
	mnu2->AddItem(CSTR("OSM Tile"), MNU_OSM_TILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Open Cycle Map"), MNU_OSM_CYCLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Transport"), MNU_OSM_TRANSP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("MapQuest"), MNU_OSM_MAPQUEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Landscape"), MNU_OSM_LANDSCAPE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Outdoors"), MNU_OSM_OUTDOORS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Transport Dark"), MNU_OSM_TRANSP_DARK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Spinal Map"), MNU_OSM_SPINAL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddSeperator();
	mnu2->AddItem(CSTR("Local file (Dir)..."), MNU_OSM_LOCAL_DIR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Local file (File)..."), MNU_OSM_LOCAL_FILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
//	mnu2->AddItem(CSTR("OSM Cache Server Test"), MNU_TEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Coord Converter"), MNU_COORD_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("OSM Cache Server"), MNU_OSM_CACHE_SERVER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SHPConv"), MNU_SHPCONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	
	mnu = this->mnuMain->AddSubMenu(CSTR("&Device"));
	mnu->AddItem(CSTR("&Capture Device"), MNU_CAP_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&MTK GPS Tracker"), MNU_MTK_GPS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&GSM Modem"), MNU_GSM_MODEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Voice Modem"), MNU_VOICEMODEM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("G&PS Tracker"), MNU_GPS_TRACKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&WIA Device"), MNU_WIA_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&TV Control"), MNU_TV_CONTROL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("CP&U Info"), MNU_CPUINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("GPU Info"), MNU_GPUINFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("i-Buddy"), MNU_IBUDDY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Denso Wave QB-30"), MNU_DWQB30, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SnB Dongle"), MNU_SNB_DONGLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Bluetooth"), MNU_BLUETOOTH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Bluetooth LE"), MNU_BLUETOOTHLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Accelerometer"), MNU_ACCELEROMETER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Power Control"), MNU_POWERCONTROL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Timing Test"), MNU_TIMETEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("IOPin"));
	mnu2->AddItem(CSTR("IOPin Test"), MNU_IOPINTEST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("DHT22 / AM2305"), MNU_DHT22, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("DS18B20"), MNU_DS18B20, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Electronic Scale"), MNU_ESCALE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("PCI Devices"), MNU_PCIDEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("USB Devices"), MNU_USBDEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HID Devices"), MNU_HIDDEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SD Cards"), MNU_SDCARD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnuMain->AddSubMenu(CSTR("&Help"));
	mnu->AddItem(CSTR("&About..."), MNU_ABOUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	this->SetMenu(this->mnuMain);
}

SSWR::AVIRead::AVIRBaseForm::~AVIRBaseForm()
{
	this->core->CloseAllForm();
}

void SSWR::AVIRead::AVIRBaseForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[16];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<Map::TileMap> tileMap;
	NN<Map::MapDrawLayer> mapLyr;

	switch (cmdId)
	{
	case MNU_ABOUT:
		{
			SSWR::AVIRead::AVIRAboutForm dlg(nullptr, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_OPEN_ODBC:
		{
			SSWR::AVIRead::AVIRODBCStrForm dlg(nullptr, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_SET_CODEPAGE:
		{
			SSWR::AVIRead::AVIRCodePageForm dlg(nullptr, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_SET_COLOR:
		{
			SSWR::AVIRead::AVIRColorSettingForm dlg(nullptr, this->ui, this->core, this->GetHMonitor());
			dlg.ShowDialog(this);
		}
		break;
	case MNU_GEN_IMAGE:
		{
			SSWR::AVIRead::AVIRGenImageForm dlg(nullptr, this->ui, this->core);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_CAP_DEV:
		{
			SSWR::AVIRead::AVIRCaptureDevForm dlg(nullptr, this->ui, this->core);
			NN<Media::VideoCapturer> capture;
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK && dlg.capture.SetTo(capture))
			{
				NN<Media::MediaFile> mf;
				sptr = capture->GetSourceName(sbuff).Or(sbuff);
				NEW_CLASSNN(mf, Media::MediaFile(CSTRP(sbuff, sptr)));
				mf->AddSource(capture, 0);
				this->core->OpenObject(mf);
			}
		}
		break;
	case MNU_MTK_GPS:
		{
			NN<IO::SerialPort> port;
			NN<IO::Device::MTKGPSNMEA> mtk;
			NN<Map::GPSTrack> trk;

			NEW_CLASSNN(port, IO::SerialPort(IO::Device::MTKGPSNMEA::GetMTKSerialPort(), 115200, IO::SerialPort::PARITY_NONE, true));
			if (!port->IsError())
			{
				NEW_CLASSNN(mtk, IO::Device::MTKGPSNMEA(port, false));
				if (mtk->IsMTKDevice())
				{
					NEW_CLASSNN(trk, Map::GPSTrack(CSTR("MTK_Tracker"), true, 0, nullptr));
					if (mtk->ParseLog(trk))
					{
						core->OpenObject(trk);
					}
					else
					{
						this->ui->ShowMsgOK(CSTR("Error in parsing log"), CSTR("MTK Tracker"), this);
						trk.Delete();
					}
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("MTK Tracker not found"), CSTR("MTK Tracker"), this);
				}
				mtk.Delete();
				port.Delete();
			}
		}
		break;
	case MNU_GSM_MODEM:
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(nullptr, this->ui, this->core, false, nullptr, this->core->GetLog());
			frm.SetText(CSTR("Select GSM Modem"));
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->core->OpenGSMModem(frm.GetStream());
			}
		}
		break;
	case MNU_GPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(nullptr, this->ui, this->core, true, nullptr, this->core->GetLog());
			frm.SetText(CSTR("Select GPS Tracker"));
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<SSWR::AVIRead::AVIRGPSTrackerForm> gpsFrm;
				NN<IO::GPSNMEA> gps;
				NEW_CLASSNN(gps, IO::GPSNMEA(frm.GetStream(), true));
				NEW_CLASSNN(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(nullptr, this->ui, this->core, gps, true));
				this->core->ShowForm(gpsFrm);
			}
		}
		break;
	case MNU_WIA_DEV:
		{
			SSWR::AVIRead::AVIRWIADevForm frm(nullptr, this->ui, this->core);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				
			}
		}
		break;
	case MNU_OSM_TILE:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.openstreetmap.org/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_CYCLE:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOpenCycleMap"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/cycle/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/cycle/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/cycle/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_TRANSP:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransport"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/transport/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_LANDSCAPE:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMLandscape"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/landscape/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/landscape/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/landscape/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_OUTDOORS:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOutdoors"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/outdoors/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/outdoors/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/outdoors/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_TRANSP_DARK:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransportDark"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/transport-dark/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/transport-dark/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/transport-dark/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_SPINAL:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMSpinalMap"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.thunderforest.com/spinal-map/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://b.tile.thunderforest.com/spinal-map/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://c.tile.thunderforest.com/spinal-map/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_OSM_MAPQUEST:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMMapQuest"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://otile1.mqcdn.com/tiles/1.0.0/osm/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), nullptr));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://otile3.mqcdn.com/tiles/1.0.0/osm/"));
		NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap)->AddAlternateURL(CSTR("http://otile4.mqcdn.com/tiles/1.0.0/osm/"));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_ESRI_MAP:
		{
			SSWR::AVIRead::AVIRESRIMapForm frm(nullptr, this->ui, this->core, nullptr);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Crypto::Hash::CRC32R crc(Crypto::Hash::CRC32::GetPolynormialIEEE());
				NN<Map::ESRI::ESRITileMap> map;
				UInt8 crcVal[4];
				Text::String *url = frm.GetSelectedURL();
				crc.Calc((UInt8*)url->v, url->leng * sizeof(UTF8Char));
				crc.GetValue(crcVal);
				sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
				sptr2 = Text::StrInt32(sbuff2, ReadMInt32(crcVal));
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTRP(sbuff2, sptr2));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				*sptr = 0;
				NEW_CLASSNN(map, Map::ESRI::ESRITileMap(url, CSTRP(sbuff, sptr), this->core->GetSocketFactory(), nullptr));
				NEW_CLASSNN(mapLyr, Map::TileMapLayer(map, this->core->GetParserList()));
				this->core->OpenObject(mapLyr);
			}
		}
		break;
/*	case MNU_TEXT_VIEWER:
		{
			UtilUI::TextViewerForm frm(0, this->ui, this->core->GetMonitorMgr(), this->core->GetCurrCodePage());
			frm.ShowDialog(this);
		}
		break;*/
	case MNU_NET_INFO:
		{
			SSWR::AVIRead::AVIRNetInfoForm frm(nullptr, this->ui, this->core);
			frm.ShowDialog(this);
		}
		break;
	case MNU_SUDOKU_SOLVER:
		{
			SSWR::AVIRead::AVIRSudokuForm frm(nullptr, this->ui, this->core);
			frm.ShowDialog(this);
		}
		break;
	case MNU_OPEN_FILE:
		{
			SSWR::AVIRead::AVIROpenFileForm frm(nullptr, this->ui, this->core, IO::ParserType::Unknown);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Text::String> fname = frm.GetFileName();
				UIntOS i = fname->IndexOf(':');
				if (i == INVALID_INDEX || i == 1)
				{
					if (IO::Path::GetPathType(fname->ToCString()) == IO::Path::PathType::Directory)
					{
						NN<IO::DirectoryPackage> dp;
						NEW_CLASSNN(dp, IO::DirectoryPackage(fname));
						this->core->OpenObject(dp);	
					}
					else
					{
						NN<IO::StmData::FileData> fd;
						NEW_CLASSNN(fd, IO::StmData::FileData(fname, false));
						if (fd->GetDataSize() == 0)
						{
							this->ui->ShowMsgOK(CSTR("Error in loading file"), CSTR("AVIRead"), this);
						}
						else
						{
							this->core->LoadDataType(fd, nullptr, frm.GetParserType());
						}
						fd.Delete();
					}
				}
				else
				{
					NN<IO::ParsedObject> pobj;
					if (!Net::URL::OpenObject(fname->ToCString(), nullptr, this->core->GetSocketFactory(), nullptr, 30000, this->core->GetLog()).SetTo(pobj))
					{
						this->ui->ShowMsgOK(CSTR("Error in loading file"), CSTR("AVIRead"), this);
					}
					else
					{
						this->core->OpenObject(pobj);
					}
				}
			}
		}
		break;
	case MNU_PLAYLIST:
		{
			NN<Media::Playlist> playlist;
			NEW_CLASSNN(playlist, Media::Playlist(CSTR("Untitled"), this->core->GetParserList()));
			this->core->OpenObject(playlist);
		}
		break;
	case MNU_HTTP_SVR:
		{
			NN<SSWR::AVIRead::AVIRHTTPSvrForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHTTPSvrForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TV_CONTROL:
		{
			NN<SSWR::AVIRead::AVIRTVControlForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRTVControlForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STMTERM:
		{
			NN<SSWR::AVIRead::AVIRStreamTermForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRStreamTermForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NTPCLIENT:
		{
			NN<SSWR::AVIRead::AVIRNTPClientForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRNTPClientForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SET_AUDIO:
		{
			SSWR::AVIRead::AVIRSetAudioForm frm(nullptr, this->ui, this->core);
			frm.ShowDialog(this);
		}
		break;
	case MNU_FILE_ANALYSE:
		{
			NN<SSWR::AVIRead::AVIRFileAnalyseForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRFileAnalyseForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROC_INFO:
		{
			NN<SSWR::AVIRead::AVIRProcInfoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRProcInfoForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SET_DPI:
		{
			SSWR::AVIRead::AVIRSetDPIForm frm(nullptr, this->ui, this->core);
			frm.ShowDialog(this);
		}
		break;
	case MNU_DNSCLIENT:
		{
			NN<SSWR::AVIRead::AVIRDNSClientForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRDNSClientForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILEEX:
		{
			NN<SSWR::AVIRead::AVIRFileExForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRFileExForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DNSPROXY:
		{
			NN<SSWR::AVIRead::AVIRDNSProxyForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRDNSProxyForm(nullptr, this->ui, this->core));
			if (frm->IsError())
			{
				frm.Delete();
				this->ui->ShowMsgOK(CSTR("Error in starting DNS proxy. Port is in use?"), CSTR("Error"), this);
			}
			else
			{
				this->core->ShowForm(frm);
			}
		}
		break;
	case MNU_SMTPSERVER:
		{
			NN<SSWR::AVIRead::AVIREmailServerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIREmailServerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_AUDIOFILTER:
		{
			NN<SSWR::AVIRead::AVIRAudioFilterForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRAudioFilterForm(nullptr, this->ui, this->core, false));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_NET_PING:
		{
			NN<SSWR::AVIRead::AVIRNetPingForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRNetPingForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IMAGE_BATCH:
		{
			NN<SSWR::AVIRead::AVIRImageBatchForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRImageBatchForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILE_HASH:
		{
			NN<SSWR::AVIRead::AVIRFileHashForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRFileHashForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_UDP_CAPTURE:
		{
			NN<SSWR::AVIRead::AVIRUDPCaptureForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRUDPCaptureForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPCLIENT:
		{
			NN<SSWR::AVIRead::AVIRHTTPClientForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHTTPClientForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_WHOISCLIENT:
		{
			NN<SSWR::AVIRead::AVIRWHOISClientForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRWHOISClientForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TCPTEST:
		{
			NN<SSWR::AVIRead::AVIRTCPTestForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRTCPTestForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPTEST:
		{
			NN<SSWR::AVIRead::AVIRHTTPTestForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHTTPTestForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROXYSERVER:
		{
			NN<SSWR::AVIRead::AVIRProxyServerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRProxyServerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPPROXYCLIENT:
		{
			NN<SSWR::AVIRead::AVIRHTTPProxyClientForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHTTPProxyClientForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROTODEC:
		{
			NN<SSWR::AVIRead::AVIRProtoDecForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRProtoDecForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PROF_RESIZER:
		{
			NN<SSWR::AVIRead::AVIRProfiledResizerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRProfiledResizerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOG_BACKUP:
		{
			NN<SSWR::AVIRead::AVIRLogBackupForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRLogBackupForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TEXT_ENCRYPT:
		{
			NN<SSWR::AVIRead::AVIREncryptForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIREncryptForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAM_CONV:
		{
			NN<SSWR::AVIRead::AVIRStreamConvForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRStreamConvForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HQMP3:
		{
			NN<SSWR::AVIRead::AVIRHQMPForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHQMPForm(nullptr, this->ui, this->core, SSWR::AVIRead::AVIRHQMPForm::QM_LQ));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HQMP3HQ:
		{
			NN<SSWR::AVIRead::AVIRHQMPForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHQMPForm(nullptr, this->ui, this->core, SSWR::AVIRead::AVIRHQMPForm::QM_HQ));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HQMP3UQ:
		{
			NN<SSWR::AVIRead::AVIRHQMPForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHQMPForm(nullptr, this->ui, this->core, SSWR::AVIRead::AVIRHQMPForm::QM_UQ));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CPUINFO:
		{
			NN<SSWR::AVIRead::AVIRCPUInfoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRCPUInfoForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IBUDDY:
		{
			NN<SSWR::AVIRead::AVIRIBuddyForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRIBuddyForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DWQB30:
		{
			NN<SSWR::AVIRead::AVIRDWQB30Form> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRDWQB30Form(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ICCINFO:
		{
			NN<SSWR::AVIRead::AVIRICCInfoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRICCInfoForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CLIPBOARD_VIEWER:
		{
			NN<SSWR::AVIRead::AVIRClipboardViewerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRClipboardViewerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BENCHMARK:
		{
			NN<SSWR::AVIRead::AVIRBenchmarkForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRBenchmarkForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_FILE_SIZE_PACK:
		{
			NN<SSWR::AVIRead::AVIRFileSizePackForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRFileSizePackForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PAINT_CNT:
		{
			NN<SSWR::AVIRead::AVIRPaintCntForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRPaintCntForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ASM_CONV:
		{
			NN<SSWR::AVIRead::AVIRAsmConvForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRAsmConvForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_COORD_CONV:
		{
			NN<SSWR::AVIRead::AVIRCoordConvForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRCoordConvForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_GPSDEV:
		{
			NN<SSWR::AVIRead::AVIRGPSDevForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGPSDevForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HTTPDOWNLOADER:
		{
			NN<SSWR::AVIRead::AVIRHTTPDownloaderForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHTTPDownloaderForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_DRAG_DROP:
		{
			NN<SSWR::AVIRead::AVIRDragDropViewerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRDragDropViewerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_JSTEXT:
		{
			NN<SSWR::AVIRead::AVIRJSTextForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRJSTextForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_UDPTEST:
		{
			NN<SSWR::AVIRead::AVIRUDPTestForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRUDPTestForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CODEIMAGEGEN:
		{
			NN<SSWR::AVIRead::AVIRCodeImageGenForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRCodeImageGenForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_EDIDVIEWER:
		{
			NN<SSWR::AVIRead::AVIREDIDViewerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIREDIDViewerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
/*	case MNU_GPUINFO:
		{
			NN<SSWR::AVIRead::AVIRGPUInfoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGPUInfoForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;*/
	case MNU_WMI:
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
		{
			SSWR::AVIRead::AVIRWMIForm frm(nullptr, this->ui, this->core);
			frm.ShowDialog(this);
		}
#endif
		break;
	case MNU_SNB_DONGLE:
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(nullptr, this->ui, this->core, false);
			frm.SetText(CSTR("Select SNB Dongle"));
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<SSWR::AVIRead::AVIRSNBDongleForm> snbFrm;
				NEW_CLASSNN(snbFrm, SSWR::AVIRead::AVIRSNBDongleForm(nullptr, this->ui, this->core, frm.GetStream()));
				this->core->ShowForm(snbFrm);
			}
		}
		break;
	case MNU_CHINESE:
		{
			NN<SSWR::AVIRead::AVIRChineseForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRChineseForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BLUETOOTH:
		{
			NN<SSWR::AVIRead::AVIRBluetoothForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRBluetoothForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OSM_CACHE_SERVER:
		{
			NN<SSWR::AVIRead::AVIROSMCacheCfgForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIROSMCacheCfgForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TEST:
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMCacheTest"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://127.0.0.1/"), CSTRP(sbuff, sptr), 18, this->core->GetSocketFactory(), 0));
		NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
		this->core->OpenObject(mapLyr);
		break;
	case MNU_IMAGEVIEWER:
		{
			NN<SSWR::AVIRead::AVIRImageViewerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRImageViewerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LINECOUNTER:
		{
			NN<SSWR::AVIRead::AVIRLineCounterForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRLineCounterForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OSM_LOCAL_DIR:
		{
			NN<UI::GUIFolderDialog> dlg = this->ui->NewFolderDialog();//L"SSWR", L"AVIRead", L"OSMLocal");
			if (dlg->ShowDialog(this->GetHandle()))
			{
				NN<IO::DirectoryPackage> pkg;
				NEW_CLASSNN(pkg, IO::DirectoryPackage(dlg->GetFolder()));
				NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(pkg));
				NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, this->core->GetParserList()));
				this->core->OpenObject(mapLyr);
			}
			dlg.Delete();
		}
		break;
	case MNU_OSM_LOCAL_FILE:
		{
			NN<Parser::ParserList> parsers = this->core->GetParserList();
			NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"OSMLocalFile", false);
			parsers->PrepareSelector(dlg, IO::ParserType::PackageFile);
			if (dlg->ShowDialog(this->GetHandle()))
			{
				NN<IO::StmData::FileData> fd;
				NN<IO::PackageFile> pkg;
				NEW_CLASSNN(fd, IO::StmData::FileData(dlg->GetFileName(), false));
				if (Optional<IO::PackageFile>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::PackageFile)).SetTo(pkg))
				{
					NEW_CLASSNN(tileMap, Map::OSM::OSMLocalTileMap(pkg));
					NEW_CLASSNN(mapLyr, Map::TileMapLayer(tileMap, parsers));
					this->core->OpenObject(mapLyr);
				}
				fd.Delete();
			}
		}
		break;
	case MNU_TCPSPDCLI:
		{
			NN<SSWR::AVIRead::AVIRTCPSpdCliForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRTCPSpdCliForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TCPSPDSVR:
		{
			NN<SSWR::AVIRead::AVIRTCPSpdSvrForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRTCPSpdSvrForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ACCELEROMETER:
		{
			IO::SensorManager sensors;
			IntOS cnt = sensors.GetAccelerometerCnt();
			if (cnt > 0)
			{
				NN<IO::SensorAccelerometer> acc;
				if (sensors.CreateAccelerometer(0).SetTo(acc))
				{
					NN<SSWR::AVIRead::AVIRAccelerometerForm> frm;
					NEW_CLASSNN(frm, SSWR::AVIRead::AVIRAccelerometerForm(nullptr, this->ui, this->core, acc));
					this->core->ShowForm(frm);
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("Error in accessing accelerometer"), CSTR("Accelerometer"), this);
				}
			}
			else
			{
				this->ui->ShowMsgOK(CSTR("No accelerometers found"), CSTR("Accelerometer"), this);
			}
		}
		break;
	case MNU_WIFICAPTURE:
		{
			NN<SSWR::AVIRead::AVIRWifiCaptureForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRWifiCaptureForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_POWERCONTROL:
		{
			NN<SSWR::AVIRead::AVIRPowerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRPowerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_TIMETEST:
		{
			NN<SSWR::AVIRead::AVIRTimeTestForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRTimeTestForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IOPINTEST:
		{
			SSWR::AVIRead::AVIRSelIOPinForm frm(nullptr, this->ui, this->core);
			frm.SetText(CSTR("Select IO Pin"));
			NN<IO::IOPin> ioPin;
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.ioPin.SetTo(ioPin))
			{
				NN<SSWR::AVIRead::AVIRIOPinTestForm> testFrm;
				NEW_CLASSNN(testFrm, SSWR::AVIRead::AVIRIOPinTestForm(nullptr, this->ui, this->core, ioPin));
				this->core->ShowForm(testFrm);
			}
		}
		break;
	case MNU_DHT22:
		{
			SSWR::AVIRead::AVIRSelIOPinForm frm(nullptr, this->ui, this->core);
			frm.SetText(CSTR("Select DHT22 IO Pin"));
			NN<IO::IOPin> ioPin;
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.ioPin.SetTo(ioPin))
			{
				NN<SSWR::AVIRead::AVIRDHT22Form> testFrm;
				NEW_CLASSNN(testFrm, SSWR::AVIRead::AVIRDHT22Form(nullptr, this->ui, this->core, ioPin));
				this->core->ShowForm(testFrm);
			}
		}
		break;
	case MNU_DS18B20:
		{
			SSWR::AVIRead::AVIRSelIOPinForm frm(nullptr, this->ui, this->core);
			frm.SetText(CSTR("Select DS18B20 IO Pin"));
			NN<IO::IOPin> ioPin;
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.ioPin.SetTo(ioPin))
			{
				NN<SSWR::AVIRead::AVIRDS18B20Form> testFrm;
				NEW_CLASSNN(testFrm, SSWR::AVIRead::AVIRDS18B20Form(nullptr, this->ui, this->core, ioPin));
				this->core->ShowForm(testFrm);
			}
		}
		break;
	case MNU_TIMEDCAPTURE:
		{
			NN<SSWR::AVIRead::AVIRTimedCaptureForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRTimedCaptureForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_GUIEVENT:
		{
			NN<SSWR::AVIRead::AVIRGUIEventForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGUIEventForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_CALCULATOR1:
		{
			NN<SSWR::AVIRead::AVIRCalculator1Form> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRCalculator1Form(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SETLOCATIONSVC:
		{
			NN<SSWR::AVIRead::AVIRSetLocationSvcForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRSetLocationSvcForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
/*	case MNU_SCREENCAPTURE:
		{
			NN<Media::ScreenCapturer> capturer;
			Media::StaticImage *img;
			NEW_CLASSNN(capturer, Media::ScreenCapturer(this->core->GetMonitorMgr(), this->core->GetColorManager()));
			img = capturer->CaptureScreen(this->GetHMonitor());
			DEL_CLASS(capturer);
			if (img)
			{
				NN<Media::ImageList> imgList;
				NEW_CLASSNN(imgList, Media::ImageList((const UTF8Char*)"ScreenCapture"));
				imgList->AddImage(img, 0);
				this->core->OpenObject(imgList);
			}
		}
		break;*/
	case MNU_HASHTEST:
		{
			NN<SSWR::AVIRead::AVIRHashTestForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHashTestForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_THREADSPEED:
		{
			NN<SSWR::AVIRead::AVIRThreadSpeedForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRThreadSpeedForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_ESCALE:
		{
			NN<SSWR::AVIRead::AVIRElectronicScaleForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRElectronicScaleForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_VOICEMODEM:
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(nullptr, this->ui, this->core, false);
			frm.SetText(CSTR("Select Voice Modem"));
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<SSWR::AVIRead::AVIRVoiceModemForm> innerFrm;
				NN<IO::Device::RockwellModemController> modem;
				NN<IO::ATCommandChannel> channel;
				NEW_CLASSNN(channel, IO::ATCommandChannel(frm.GetStream(), false));
				NEW_CLASSNN(modem, IO::Device::RockwellModemController(channel, false));
				NEW_CLASSNN(innerFrm, SSWR::AVIRead::AVIRVoiceModemForm(nullptr, this->ui, this->core, modem, channel, frm.GetStream()));
				this->core->ShowForm(innerFrm);
			}
		}
		break;
	case MNU_PERFORMANCELOG:
		{
			NN<SSWR::AVIRead::AVIRPerformanceLogForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRPerformanceLogForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_JTT808:
		{
			NN<SSWR::AVIRead::AVIRJTT808ServerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRJTT808ServerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_BROKER:
		{
			NN<SSWR::AVIRead::AVIRMQTTBrokerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRMQTTBrokerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_PUBLISH:
		{
			NN<SSWR::AVIRead::AVIRMQTTPublishForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRMQTTPublishForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MQTT_SUBSCRIBE:
		{
			NN<SSWR::AVIRead::AVIRMQTTSubscribeForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRMQTTSubscribeForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_OLEDB:
		{
#if defined(WIN32) || defined(_WIN64) || (defined(_MSC_VER) && defined(_WIN32))
			NN<SSWR::AVIRead::AVIROLEDBForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIROLEDBForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
#endif
		}
		break;
	case MNU_PCIDEVICE:
		{
			NN<SSWR::AVIRead::AVIRPCIDeviceForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRPCIDeviceForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_USBDEVICE:
		{
			NN<SSWR::AVIRead::AVIRUSBDeviceForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRUSBDeviceForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SDCARD:
		{
			NN<SSWR::AVIRead::AVIRSDCardForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRSDCardForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_SHPCONV:
		{
			NN<SSWR::SHPConv::SHPConvMainForm> frm;
			NEW_CLASSNN(frm, SSWR::SHPConv::SHPConvMainForm(nullptr, this->ui, this->core->GetDrawEngine(), this->core->GetMonitorMgr()));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOGMERGE:
		{
			NN<SSWR::AVIRead::AVIRLogMergeForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRLogMergeForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOGEXTRACT:
		{
			NN<SSWR::AVIRead::AVIRLogExtractForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRLogExtractForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_VIDEOINFO:
		{
			NN<SSWR::AVIRead::AVIRVideoInfoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRVideoInfoForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_VIDEOCHECKER:
		{
			NN<SSWR::AVIRead::AVIRVideoCheckerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRVideoCheckerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_HIDDEVICE:
		{
			NN<SSWR::AVIRead::AVIRHIDDeviceForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHIDDeviceForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IMAGE_PSNR:
		{
			NN<SSWR::AVIRead::AVIRImagePSNRForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRImagePSNRForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IMAGE_BATCH_CONV:
		{
			NN<SSWR::AVIRead::AVIRImageBatchConvForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRImageBatchConvForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MODBUSMASTER:
		{
			NN<SSWR::AVIRead::AVIRMODBUSMasterForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRMODBUSMasterForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAMECHO:
		{
			NN<SSWR::AVIRead::AVIRStreamEchoForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRStreamEchoForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_STREAMLATENCY:
		{
			NN<SSWR::AVIRead::AVIRStreamLatencyForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRStreamLatencyForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_EGAUGESVR:
		{
			NN<SSWR::AVIRead::AVIReGaugeSvrForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIReGaugeSvrForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_LOGSERVER:
		{
			NN<SSWR::AVIRead::AVIRLogServerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRLogServerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_PINGMONITOR:
		{
			NN<SSWR::AVIRead::AVIRPingMonitorForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRPingMonitorForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_MACMANAGER:
		{
			NN<SSWR::AVIRead::AVIRMACManagerForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRMACManagerForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_IPSCAN:
		{
			NN<SSWR::AVIRead::AVIRIPScanForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRIPScanForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BLUETOOTHLE:
		{
			NN<SSWR::AVIRead::AVIRBluetoothLEForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRBluetoothLEForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	case MNU_BRUTEFORCE:
		{
			NN<SSWR::AVIRead::AVIRBruteForceForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRBruteForceForm(nullptr, this->ui, this->core));
			this->core->ShowForm(frm);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRBaseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
