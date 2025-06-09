#ifndef _SM_SSWR_AVIREAD_AVIRCORE
#define _SM_SSWR_AVIREAD_AVIRCORE
#include "Data/ArrayListNN.h"
#include "Exporter/ExporterList.h"
#include "IO/GPIOControl.h"
#include "IO/LogTool.h"
#include "IO/SiLabDriver.h"
#include "IO/VirtualIOPinMgr.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Map/CesiumTile.h"
#include "Map/MapManager.h"
#include "Media/AudioDevice.h"
#include "Media/ColorConv.h"
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "Media/Printer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Parser/ParserList.h"
#include "Text/CString.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGPSTrackerForm;
		class AVIRGISForm;

		class AVIRCore
		{
		public:
			typedef enum
			{
				IOPT_GPIO = 1,
				IOPT_VIOPIN = 2
			} IOPinType;

		protected:
			NN<Parser::ParserList> parsers;
			Map::MapManager mapMgr;
			Media::ColorManager colorMgr;
			Optional<IO::SiLabDriver> siLabDriver;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Net::WebBrowser> browser;
			Text::EncodingFactory encFact;
			NN<Media::DrawEngine> eng;
			NN<UI::GUICore> ui;
			Exporter::ExporterList exporters;
			UInt32 currCodePage;
			IO::LogTool log;
			Data::ArrayListStringNN audDevList;
			Media::AudioDevice audDevice;
			Int32 audAPIType;
			Media::MonitorMgr monMgr;
			IO::VirtualIOPinMgr vioPinMgr;
			Optional<IO::GPIOControl> gpioCtrl;
			Bool forwardedUI;

			Data::ArrayListNN<UI::GUIForm> frms;
			Optional<SSWR::AVIRead::AVIRGISForm> gisForm;

			Bool batchLoad;
			Optional<Data::ArrayListNN<Map::MapDrawLayer>> batchLyrs;
			Optional<Data::ArrayListNN<Map::CesiumTile>> batchCesiumTiles;

		protected:
			static void __stdcall FormClosed(AnyType userObj, NN<UI::GUIForm> frm);
			void InitForm(NN<UI::GUIForm> frm);

			AVIRCore(NN<UI::GUICore> ui);
		public:
			virtual ~AVIRCore();

			virtual void OpenObject(NN<IO::ParsedObject> pobj) = 0;
			virtual void SaveData(NN<UI::GUIForm> ownerForm, NN<IO::ParsedObject> pobj, UnsafeArray<const WChar> dialogName) = 0;
			virtual Optional<Media::Printer> SelectPrinter(Optional<UI::GUIForm> frm) = 0;

			void OpenGSMModem(Optional<IO::Stream> modemPort);
			Optional<IO::Stream> OpenStream(OptOut<IO::StreamType> st, Optional<UI::GUIForm> ownerFrm, Int32 defBaudRate, Bool allowReadOnly);
			void OpenHex(NN<IO::StreamData> fd, Optional<IO::FileAnalyse::FileAnalyser> fileAnalyse);

			void BeginLoad();
			void EndLoad();
			Bool LoadData(NN<IO::StreamData> data, Optional<IO::PackageFile> pkgFile);
			Bool LoadDataType(NN<IO::StreamData> data, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);

			NN<Parser::ParserList> GetParserList();
			NN<Map::MapManager> GetMapManager();
			NN<Media::ColorManager> GetColorMgr();
			NN<Net::SocketFactory> GetSocketFactory();
			NN<Net::TCPClientFactory> GetTCPClientFactory();
			NN<Media::DrawEngine> GetDrawEngine();
			NN<Text::EncodingFactory> GetEncFactory();
			Optional<IO::SiLabDriver> GetSiLabDriver();
			NN<Net::WebBrowser> GetWebBrowser();
			NN<IO::VirtualIOPinMgr> GetVirtualIOPinMgr();
			Optional<IO::GPIOControl> GetGPIOControl();
			NN<Media::AudioDevice> GetAudioDevice();

			UInt32 GetCurrCodePage();
			void SetCodePage(UInt32 codePage);
			NN<IO::LogTool> GetLog();
			Double GetMonitorHDPI(Optional<MonitorHandle> hMonitor);
			void SetMonitorHDPI(Optional<MonitorHandle> hMonitor, Double monitorHDPI);
			Double GetMonitorDDPI(Optional<MonitorHandle> hMonitor);
			void SetMonitorDDPI(Optional<MonitorHandle> hMonitor, Double monitorDDPI);
			NN<Media::MonitorMgr> GetMonitorMgr();
			
			void SetAudioDeviceList(Optional<Data::ArrayListStringNN> audDevList);
			NN<Data::ArrayListStringNN> GetAudioDeviceList();
			Int32 GetAudioAPIType();
			Optional<Media::AudioRenderer> BindAudio(Optional<Media::AudioSource> audSrc);

			Bool GenLinePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, Double lineThick, UInt32 lineColor, NN<Media::ColorConv> colorConv);
			Bool GenLineStylePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, UOSInt lineStyle, NN<Media::ColorConv> colorConv);
			Bool GenFontStylePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, UOSInt fontStyle, NN<Media::ColorConv> colorConv);
			Bool GenFontPreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, Text::CStringNN fontName, Double fontSizePt, UInt32 fontColor, NN<Media::ColorConv> colorConv);
			void ShowForm(NN<UI::GUIForm> frm);
			void CloseAllForm();
			void SetGISForm(Optional<SSWR::AVIRead::AVIRGISForm> frm);
			Optional<SSWR::AVIRead::AVIRGISForm> GetGISForm();

			static Text::CStringNN IOPinTypeGetName(IOPinType iopt);
		};
	}
}
#endif
