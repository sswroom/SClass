#ifndef _SM_SSWR_AVIREAD_AVIRCAPTUREDEVFORM
#define _SM_SSWR_AVIREAD_AVIRCAPTUREDEVFORM
#include "Media/VideoCaptureMgr.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCaptureDevForm : public UI::GUIForm
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
		public:
			Optional<Media::VideoCapturer> capture;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Media::VideoCaptureMgr captureMgr;
			Optional<Media::VideoCapturer> currCapture;
			Data::ArrayListNN<Media::VideoCaptureMgr::DeviceInfo> devInfoList;
			Data::ArrayListNN<CaptureFormat> currFormats;
			NN<UI::GUILabel> lblDevice;
			NN<UI::GUIListBox> lbDevice;
			NN<UI::GUILabel> lblFormat;
			NN<UI::GUIComboBox> cboFormat;
			NN<UI::GUITextBox> txtDeviceInfo;

			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnDevChg(AnyType userObj);

			void ReleaseFormats();
		public:
			AVIRCaptureDevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCaptureDevForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
