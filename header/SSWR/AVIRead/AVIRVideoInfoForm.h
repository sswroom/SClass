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
				Optional<Media::IAudioSource> adecoder;
				Optional<Media::NullRenderer> renderer;
				Optional<Media::IVideoSource> vdecoder;
				NN<Sync::Event> evt;
			} DecodeStatus;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Media::MediaFile> currFile;
			Data::ArrayListNN<DecodeStatus> decStatus;

			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnDecode;
			NN<UI::GUILabel> lblDecode;
			NN<UI::GUIListBox> lbStream;
			NN<UI::GUIHSplitter> hspStream;
			NN<UI::GUITextBox> txtStream;

			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange frChg, AnyType userData);
			static void __stdcall OnAudioEnd(AnyType userData);
			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnStreamChg(AnyType userObj);
			static void __stdcall OnDecodeClicked(AnyType userObj);
			Bool OpenFile(Text::CStringNN fileName);
			void ClearDecode();
		public:
			AVIRVideoInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRVideoInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
