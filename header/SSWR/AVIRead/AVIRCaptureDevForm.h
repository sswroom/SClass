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
			Media::IVideoCapture *capture;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::VideoCaptureMgr captureMgr;
			Media::IVideoCapture *currCapture;
			Data::ArrayList<Media::VideoCaptureMgr::DeviceInfo*> devInfoList;
			Data::ArrayList<CaptureFormat*> currFormats;
			UI::GUILabel *lblDevice;
			UI::GUIListBox *lbDevice;
			UI::GUILabel *lblFormat;
			UI::GUIComboBox *cboFormat;
			UI::GUITextBox *txtDeviceInfo;

			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnDevChg(void *userObj);

			void ReleaseFormats();
		public:
			AVIRCaptureDevForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCaptureDevForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
