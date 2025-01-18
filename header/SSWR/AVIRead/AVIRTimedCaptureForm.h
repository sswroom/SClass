#ifndef _SM_SSWR_AVIREAD_AVIRTIMEDCAPTUREFORM
#define _SM_SSWR_AVIREAD_AVIRTIMEDCAPTUREFORM
#include "Exporter/GUIJPGExporter.h"
#include "Media/TimedImageList.h"
#include "Media/CS/CSConverter.h"
#include "Media/VideoCaptureMgr.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTimedCaptureForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Math::Size2D<UOSInt> size;
				UInt32 fourcc;
				UInt32 bpp;
				Media::PixelFormat pf;
				UInt32 frameRateNumer;
				UInt32 frameRateDenom;
			} CaptureFormat;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::VideoCaptureMgr> captureMgr;
			Optional<Media::IVideoCapture> currCapture;
			Data::ArrayListNN<Media::VideoCaptureMgr::DeviceInfo> devInfoList;
			Data::ArrayListNN<CaptureFormat> currFormats;
			Media::FrameInfo videoInfo;
			Optional<Media::CS::CSConverter> csConv;
			NN<Exporter::GUIJPGExporter> exporter;
			Bool isStarted;
			Int32 jpgQuality;
			UInt32 interval;
			Data::Duration lastSaveTime;
			UInt32 frameCnt;
			UInt32 saveCnt;
			Optional<Media::TimedImageList> timedImageList;

			NN<UI::GUIListBox> lbDevice;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpControl;
			NN<UI::GUILabel> lblFormat;
			NN<UI::GUIComboBox> cboFormat;
			NN<UI::GUILabel> lblInterval;
			NN<UI::GUITextBox> txtInterval;
			NN<UI::GUILabel> lblJPGQuality;
			NN<UI::GUITextBox> txtJPGQuality;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITextBox> txtDeviceInfo;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblFrameCnt;
			NN<UI::GUITextBox> txtFrameCnt;
			NN<UI::GUILabel> lblSaveCnt;
			NN<UI::GUITextBox> txtSaveCnt;

			static void __stdcall OnDevChg(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange frChg, AnyType userData);

			void StopCapture();
			void ReleaseFormats();
		public:
			AVIRTimedCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimedCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
