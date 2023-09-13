#ifndef _SM_SSWR_AVIREAD_AVIRIMAGECONTROL
#define _SM_SSWR_AVIREAD_AVIRIMAGECONTROL
#include "Data/ICaseStringUTF8Map.h"
#include "Data/StringUTF8Map.h"
#include "Data/SyncLinkedList.h"
#include "Media/RGBColorFilter.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "UI/GUICustomDrawVScroll.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageControl : public UI::GUICustomDrawVScroll, public Media::IColorHandler
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
				Text::CString fileName;
				NotNullPtr<Text::String> filePath;
				NotNullPtr<Text::String> cacheFile;
				ImageSetting setting;
				Media::DrawImage *previewImg;
				Media::DrawImage *previewImg2;
			} ImageStatus;

			typedef struct
			{
				Double *gammaParam;
				UInt32 gammaCnt;
			} CameraCorr;

		public:
			typedef void (__stdcall *DispImageChanged)(void *userObj, Text::CString fileName, const ImageSetting *setting);
			typedef void (__stdcall *ProgressUpdated)(void *userObj, UOSInt finCnt);
			typedef void (__stdcall *KeyDownHandler)(void *userObj, UI::GUIControl::GUIKey key);
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::Resizer::LanczosResizer8_C8 *dispResizer;

			Sync::Mutex ioMut;
			Sync::Mutex folderMut;
			Text::String *folderPath;
			Bool folderChanged;
			Sync::Event folderThreadEvt;
			Sync::Event folderCtrlEvt;
			Sync::Mutex imgMut;
			Data::ICaseStringUTF8Map<ImageStatus*> imgMap;
			Bool imgMapUpdated;
			Bool imgUpdated;
			UInt32 previewSize;
			UOSInt currSel;
			ImageStatus *dispImg;
			Bool dispImgChg;
			Sync::Mutex filterMut;
			Media::RGBColorFilter filter;
			DispImageChanged dispHdlr;
			void *dispHdlrObj;
			ProgressUpdated progHdlr;
			void *progHdlrObj;
			Data::SyncLinkedList exportList;
			UOSInt exportCurrCnt;
			Sync::Mutex exportMut;
			ExportFormat exportFmt;
			KeyDownHandler keyHdlr;
			void *keyObj;

			Sync::Mutex cameraMut;
			Data::StringMap<CameraCorr*> cameraMap;

			Int32 threadState; //0 = not started, 1 = idle, 2 = initDir, 3 = exporting
			Int32 threadCtrlCode; //0 = no control, 1 = initDir, 2 = stop thread, 3 = force idle

			static UInt32 __stdcall FolderThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

			void InitDir();
			void ExportQueued();

			void ThreadCancelTasks();
			void EndFolder();
			Bool GetCameraName(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Media::EXIFData> exif);
			Double *GetCameraGamma(Text::CString cameraName, OutParam<UInt32> gammaCnt);
		public:
			AVIRImageControl(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<SSWR::AVIRead::AVIRCore> core, UI::GUIForm *frm, NotNullPtr<Media::ColorManagerSess> colorSess);
			virtual ~AVIRImageControl();

			virtual Text::CStringNN GetObjectClass() const;
			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);

			virtual void SetDPI(Double hdpi, Double ddpi);

			virtual void OnDraw(NotNullPtr<Media::DrawImage> img);
			virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
			virtual void OnKeyDown(UInt32 keyCode);

			void SetFolder(Text::CString folderPath);
			Text::String *GetFolder();
			Bool SaveSetting();
			void SetDispImageHandler(DispImageChanged hdlr, void *userObj);
			void SetProgressHandler(ProgressUpdated hdlr, void *userObj);
			Media::StaticImage *LoadImage(const UTF8Char *fileName);
			Media::StaticImage *LoadOriImage(const UTF8Char *fileName);
			void ApplySetting(NotNullPtr<Media::StaticImage> srcImg, NotNullPtr<Media::StaticImage> destImg, NotNullPtr<ImageSetting> setting);
			void UpdateImgPreview(ImageStatus *img);
			void UpdateImgSetting(ImageSetting *setting);
			Bool IsLoadingDir();
			void SetExportFormat(ExportFormat fmt);
			UOSInt ExportSelected();
			void MoveUp();
			void MoveDown();
			void SelectAll();

			void HandleKeyDown(KeyDownHandler keyHdlr, void *keyObj);
		};
	}
}
#endif
