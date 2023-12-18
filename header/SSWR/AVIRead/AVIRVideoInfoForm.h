#ifndef _SM_SSWR_AVIREAD_AVIRVIDEOINFOFORM
#define _SM_SSWR_AVIREAD_AVIRVIDEOINFOFORM
#include "Media/MediaFile.h"
#include "Media/NullRenderer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Event.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVideoInfoForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt64 sampleCnt;
				Data::Duration lastSampleTime;
				Bool isEnd;
				Media::IAudioSource *adecoder;
				Media::NullRenderer *renderer;
				Media::IVideoSource *vdecoder;
				Sync::Event *evt;
			} DecodeStatus;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::MediaFile *currFile;
			Data::ArrayList<DecodeStatus*> *decStatus;

			NotNullPtr<UI::GUIPanel> pnlFile;
			NotNullPtr<UI::GUILabel> lblFile;
			UI::GUITextBox *txtFile;
			NotNullPtr<UI::GUIButton> btnDecode;
			NotNullPtr<UI::GUILabel> lblDecode;
			UI::GUIListBox *lbStream;
			NotNullPtr<UI::GUIHSplitter> hspStream;
			UI::GUITextBox *txtStream;

			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange frChg, void *userData);
			static void __stdcall OnAudioEnd(void *userData);
			static void __stdcall OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnStreamChg(void *userObj);
			static void __stdcall OnDecodeClicked(void *userObj);
			Bool OpenFile(Text::CStringNN fileName);
			void ClearDecode();
		public:
			AVIRVideoInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRVideoInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
