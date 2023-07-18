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
			Parser::ParserList *parsers;
			Map::MapManager mapMgr;
			Media::ColorManager colorMgr;
			IO::SiLabDriver *siLabDriver;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			Net::WebBrowser *browser;
			Text::EncodingFactory encFact;
			Media::DrawEngine *eng;
			UI::GUICore *ui;
			Exporter::ExporterList exporters;
			UInt32 currCodePage;
			IO::LogTool log;
			Data::ArrayListNN<Text::String> audDevList;
			Media::AudioDevice audDevice;
			Int32 audAPIType;
			Media::MonitorMgr monMgr;
			IO::VirtualIOPinMgr vioPinMgr;
			IO::GPIOControl *gpioCtrl;
			Bool forwardedUI;

			Data::ArrayList<UI::GUIForm *> frms;
			SSWR::AVIRead::AVIRGISForm *gisForm;

			Bool batchLoad;
			Data::ArrayList<Map::MapDrawLayer*> *batchLyrs;

		protected:
			static void __stdcall FormClosed(void *userObj, UI::GUIForm *frm);
			void InitForm(UI::GUIForm *frm);

			AVIRCore(UI::GUICore *ui);
		public:
			virtual ~AVIRCore();

			virtual void OpenObject(IO::ParsedObject *pobj) = 0;
			virtual void SaveData(UI::GUIForm *ownerForm, IO::ParsedObject *pobj, const WChar *dialogName) = 0;
			virtual Media::Printer *SelectPrinter(UI::GUIForm *frm) = 0;

			void OpenGSMModem(IO::Stream *modemPort);
			IO::Stream *OpenStream(IO::StreamType *st, UI::GUIForm *ownerFrm, Int32 defBaudRate, Bool allowReadOnly);
			void OpenHex(IO::StreamData *fd, IO::FileAnalyse::IFileAnalyse *fileAnalyse);

			void BeginLoad();
			void EndLoad();
			Bool LoadData(IO::StreamData *data, IO::PackageFile *pkgFile);
			Bool LoadDataType(IO::StreamData *data, IO::PackageFile *pkgFile, IO::ParserType targetType);

			Parser::ParserList *GetParserList();
			Map::MapManager *GetMapManager();
			Media::ColorManager *GetColorMgr();
			NotNullPtr<Net::SocketFactory> GetSocketFactory();
			Media::DrawEngine *GetDrawEngine();
			Text::EncodingFactory *GetEncFactory();
			IO::SiLabDriver *GetSiLabDriver();
			Net::WebBrowser *GetWebBrowser();
			IO::VirtualIOPinMgr *GetVirtualIOPinMgr();
			IO::GPIOControl *GetGPIOControl();
			Media::AudioDevice *GetAudioDevice();

			UInt32 GetCurrCodePage();
			void SetCodePage(UInt32 codePage);
			IO::LogTool *GetLog();
			Double GetMonitorHDPI(MonitorHandle *hMonitor);
			void SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI);
			Double GetMonitorDDPI(MonitorHandle *hMonitor);
			void SetMonitorDDPI(MonitorHandle *hMonitor, Double monitorDDPI);
			Media::MonitorMgr *GetMonitorMgr();
			
			void SetAudioDeviceList(Data::ArrayListNN<Text::String> *audDevList);
			Data::ArrayListNN<Text::String> *GetAudioDeviceList();
			Int32 GetAudioAPIType();
			Media::IAudioRenderer *BindAudio(Media::IAudioSource *audSrc);

			Bool GenLinePreview(Media::DrawImage *img, Media::DrawEngine *eng, UOSInt lineThick, UInt32 lineColor, Media::ColorConv *colorConv);
			Bool GenLineStylePreview(Media::DrawImage *img, Media::DrawEngine *eng, Map::MapEnv *env, UOSInt lineStyle, Media::ColorConv *colorConv);
			Bool GenFontStylePreview(Media::DrawImage *img, Media::DrawEngine *eng, Map::MapEnv *env, UOSInt fontStyle, Media::ColorConv *colorConv);
			Bool GenFontPreview(Media::DrawImage *img, Media::DrawEngine *eng, Text::CString fontName, Double fontSizePt, UInt32 fontColor, Media::ColorConv *colorConv);
			void ShowForm(UI::GUIForm *frm);
			void CloseAllForm();
			void SetGISForm(SSWR::AVIRead::AVIRGISForm *frm);
			SSWR::AVIRead::AVIRGISForm *GetGISForm();

			static Text::CString IOPinTypeGetName(IOPinType iopt);
		};
	}
}
#endif
