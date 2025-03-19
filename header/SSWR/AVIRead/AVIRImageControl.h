#ifndef _SM_SSWR_AVIREAD_AVIRIMAGECONTROL
#define _SM_SSWR_AVIREAD_AVIRIMAGECONTROL
#include "Data/ICaseStringUTF8Map.h"
#include "Data/StringMapNN.h"
#include "Data/SyncLinkedList.h"
#include "Media/RGBColorFilter.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "UI/GUICustomDrawVScroll.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageControl : public UI::GUICustomDrawVScroll, public Media::ColorHandler
		{
		public:
			typedef enum
			{
				EF_JPG,
				EF_TIF
			} ExportFormat;
			typedef struct
			{
				Int32 flags; //bit 0: selected, bit 1: output, bit 2: preview invalid, bit 3: display in screen
				Double brightness;
				Double contrast;
				Double gamma;
				Bool cropEnabled;
				Int32 cropLeft;
				Int32 cropTop;
				Int32 cropWidth;
				Int32 cropHeight;
			} ImageSetting;

		private:
			typedef struct
			{
				Text::CStringNN fileName;
				NN<Text::String> filePath;
				NN<Text::String> cacheFile;
				ImageSetting setting;
				Optional<Media::DrawImage> previewImg;
				Optional<Media::DrawImage> previewImg2;
			} ImageStatus;

			typedef struct
			{
				UnsafeArrayOpt<Double> gammaParam;
				UInt32 gammaCnt;
			} CameraCorr;

		public:
			typedef void (CALLBACKFUNC DispImageChanged)(AnyType userObj, Text::CString fileName, Optional<const ImageSetting> setting);
			typedef void (CALLBACKFUNC ProgressUpdated)(AnyType userObj, UOSInt finCnt);
			typedef void (CALLBACKFUNC KeyDownHandler)(AnyType userObj, UI::GUIControl::GUIKey key);
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::Resizer::LanczosResizerRGB_C8> dispResizer;

			Sync::Mutex ioMut;
			Sync::Mutex folderMut;
			Optional<Text::String> folderPath;
			Bool folderChanged;
			Sync::Event folderThreadEvt;
			Sync::Event folderCtrlEvt;
			Sync::Mutex imgMut;
			Data::StringMapNN<ImageStatus> imgMap;
			Bool imgMapUpdated;
			Bool imgUpdated;
			UInt32 previewSize;
			UOSInt currSel;
			Optional<ImageStatus> dispImg;
			Bool dispImgChg;
			Sync::Mutex filterMut;
			Media::RGBColorFilter filter;
			DispImageChanged dispHdlr;
			AnyType dispHdlrObj;
			ProgressUpdated progHdlr;
			AnyType progHdlrObj;
			Data::SyncLinkedList exportList;
			UOSInt exportCurrCnt;
			Sync::Mutex exportMut;
			ExportFormat exportFmt;
			KeyDownHandler keyHdlr;
			AnyType keyObj;

			Sync::Mutex cameraMut;
			Data::StringMapNN<CameraCorr> cameraMap;

			Int32 threadState; //0 = not started, 1 = idle, 2 = initDir, 3 = exporting
			Int32 threadCtrlCode; //0 = no control, 1 = initDir, 2 = stop thread, 3 = force idle

			static UInt32 __stdcall FolderThread(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);

			void InitDir();
			void ExportQueued();

			void ThreadCancelTasks();
			void EndFolder();
			Bool GetCameraName(NN<Text::StringBuilderUTF8> sb, NN<Media::EXIFData> exif);
			UnsafeArrayOpt<Double> GetCameraGamma(Text::CStringNN cameraName, OutParam<UInt32> gammaCnt);
		public:
			AVIRImageControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUIForm> frm, NN<Media::ColorManagerSess> colorSess);
			virtual ~AVIRImageControl();

			virtual Text::CStringNN GetObjectClass() const;
			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);

			virtual void SetDPI(Double hdpi, Double ddpi);

			virtual void OnDraw(NN<Media::DrawImage> img);
			virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
			virtual void OnKeyDown(UInt32 keyCode);

			void SetFolder(Text::CString folderPath);
			Optional<Text::String> GetFolder();
			Bool SaveSetting();
			void SetDispImageHandler(DispImageChanged hdlr, AnyType userObj);
			void SetProgressHandler(ProgressUpdated hdlr, AnyType userObj);
			Optional<Media::StaticImage> LoadImage(Text::CStringNN fileName);
			Optional<Media::StaticImage> LoadOriImage(Text::CStringNN fileName);
			void ApplySetting(NN<Media::StaticImage> srcImg, NN<Media::StaticImage> destImg, NN<ImageSetting> setting);
			void UpdateImgPreview(NN<ImageStatus> img);
			void UpdateImgSetting(NN<ImageSetting> setting);
			Bool IsLoadingDir();
			void SetExportFormat(ExportFormat fmt);
			UOSInt ExportSelected();
			void MoveUp();
			void MoveDown();
			void SelectAll();

			void HandleKeyDown(KeyDownHandler keyHdlr, AnyType keyObj);
		};
	}
}
#endif
