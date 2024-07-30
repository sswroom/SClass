#ifndef _SM_SSWR_AVIREAD_AVIRCORE
#define _SM_SSWR_AVIREAD_AVIRCORE
#include "Data/ArrayListNN.h"
#include "Exporter/ExporterList.h"
#include "IO/GPIOControl.h"
#include "IO/LogTool.h"
#include "IO/SiLabDriver.h"
#include "IO/VirtualIOPinMgr.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
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
			IO::SiLabDriver *siLabDriver;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Net::WebBrowser *browser;
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
			IO::GPIOControl *gpioCtrl;
			Bool forwardedUI;

			Data::ArrayListNN<UI::GUIForm> frms;
			SSWR::AVIRead::AVIRGISForm *gisForm;

			Bool batchLoad;
			Optional<Data::ArrayListNN<Map::MapDrawLayer>> batchLyrs;

		protected:
			static void __stdcall FormClosed(AnyType userObj, NN<UI::GUIForm> frm);
			void InitForm(NN<UI::GUIForm> frm);

			AVIRCore(NN<UI::GUICore> ui);
		public:
			virtual ~AVIRCore();

			virtual void OpenObject(NN<IO::ParsedObject> pobj) = 0;
			virtual void SaveData(UI::GUIForm *ownerForm, NN<IO::ParsedObject> pobj, const WChar *dialogName) = 0;
			virtual Media::Printer *SelectPrinter(UI::GUIForm *frm) = 0;

			void OpenGSMModem(IO::Stream *modemPort);
			IO::Stream *OpenStream(OptOut<IO::StreamType> st, Optional<UI::GUIForm> ownerFrm, Int32 defBaudRate, Bool allowReadOnly);
			void OpenHex(NN<IO::StreamData> fd, IO::FileAnalyse::IFileAnalyse *fileAnalyse);

			void BeginLoad();
			void EndLoad();
			Bool LoadData(NN<IO::StreamData> data, IO::PackageFile *pkgFile);
			Bool LoadDataType(NN<IO::StreamData> data, IO::PackageFile *pkgFile, IO::ParserType targetType);

			NN<Parser::ParserList> GetParserList();
			Map::MapManager *GetMapManager();
			NN<Media::ColorManager> GetColorMgr();
			NN<Net::SocketFactory> GetSocketFactory();
			NN<Net::TCPClientFactory> GetTCPClientFactory();
			NN<Media::DrawEngine> GetDrawEngine();
			NN<Text::EncodingFactory> GetEncFactory();
			IO::SiLabDriver *GetSiLabDriver();
			Net::WebBrowser *GetWebBrowser();
			IO::VirtualIOPinMgr *GetVirtualIOPinMgr();
			IO::GPIOControl *GetGPIOControl();
			Media::AudioDevice *GetAudioDevice();

			UInt32 GetCurrCodePage();
			void SetCodePage(UInt32 codePage);
			NN<IO::LogTool> GetLog();
			Double GetMonitorHDPI(MonitorHandle *hMonitor);
			void SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI);
			Double GetMonitorDDPI(MonitorHandle *hMonitor);
			void SetMonitorDDPI(MonitorHandle *hMonitor, Double monitorDDPI);
			NN<Media::MonitorMgr> GetMonitorMgr();
			
			void SetAudioDeviceList(Data::ArrayListStringNN *audDevList);
			Data::ArrayListStringNN *GetAudioDeviceList();
			Int32 GetAudioAPIType();
			Media::IAudioRenderer *BindAudio(Media::IAudioSource *audSrc);

			Bool GenLinePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, Double lineThick, UInt32 lineColor, NN<Media::ColorConv> colorConv);
			Bool GenLineStylePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, UOSInt lineStyle, NN<Media::ColorConv> colorConv);
			Bool GenFontStylePreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, NN<Map::MapEnv> env, UOSInt fontStyle, NN<Media::ColorConv> colorConv);
			Bool GenFontPreview(NN<Media::DrawImage> img, NN<Media::DrawEngine> eng, Text::CStringNN fontName, Double fontSizePt, UInt32 fontColor, NN<Media::ColorConv> colorConv);
			void ShowForm(NN<UI::GUIForm> frm);
			void CloseAllForm();
			void SetGISForm(SSWR::AVIRead::AVIRGISForm *frm);
			SSWR::AVIRead::AVIRGISForm *GetGISForm();

			static Text::CStringNN IOPinTypeGetName(IOPinType iopt);
		};
	}
}
#endif
