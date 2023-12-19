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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::VideoCaptureMgr *captureMgr;
			Media::IVideoCapture *currCapture;
			Data::ArrayList<Media::VideoCaptureMgr::DeviceInfo*> *devInfoList;
			Data::ArrayList<CaptureFormat*> *currFormats;
			Media::FrameInfo videoInfo;
			Media::CS::CSConverter *csConv;
			Exporter::GUIJPGExporter *exporter;
			Bool isStarted;
			Int32 jpgQuality;
			UInt32 interval;
			Data::Duration lastSaveTime;
			UInt32 frameCnt;
			UInt32 saveCnt;
			Media::TimedImageList *timedImageList;

			UI::GUIListBox *lbDevice;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUILabel> lblFormat;
			NotNullPtr<UI::GUIComboBox> cboFormat;
			NotNullPtr<UI::GUILabel> lblInterval;
			NotNullPtr<UI::GUITextBox> txtInterval;
			NotNullPtr<UI::GUILabel> lblJPGQuality;
			NotNullPtr<UI::GUITextBox> txtJPGQuality;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUITextBox> txtDeviceInfo;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUILabel> lblFrameCnt;
			NotNullPtr<UI::GUITextBox> txtFrameCnt;
			NotNullPtr<UI::GUILabel> lblSaveCnt;
			NotNullPtr<UI::GUITextBox> txtSaveCnt;

			static void __stdcall OnDevChg(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange frChg, void *userData);

			void StopCapture();
			void ReleaseFormats();
		public:
			AVIRTimedCaptureForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimedCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
