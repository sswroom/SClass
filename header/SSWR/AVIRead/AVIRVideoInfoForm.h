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
				Int64 sampleCnt;
				UInt32 lastSampleTime;
				Bool isEnd;
				Media::IAudioSource *adecoder;
				Media::NullRenderer *renderer;
				Media::IVideoSource *vdecoder;
				Sync::Event *evt;
			} DecodeStatus;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::MediaFile *currFile;
			Data::ArrayList<DecodeStatus*> *decStatus;

			UI::GUIPanel *pnlFile;
			UI::GUILabel *lblFile;
			UI::GUITextBox *txtFile;
			UI::GUIButton *btnDecode;
			UI::GUILabel *lblDecode;
			UI::GUIListBox *lbStream;
			UI::GUIHSplitter *hspStream;
			UI::GUITextBox *txtStream;

			static void __stdcall OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange frChg, void *userData);
			static void __stdcall OnAudioEnd(void *userData);
			static void __stdcall OnFileHandler(void *userObj, const UTF8Char **files, OSInt nFiles);
			static void __stdcall OnStreamChg(void *userObj);
			static void __stdcall OnDecodeClicked(void *userObj);
			Bool OpenFile(const UTF8Char *fileName);
			void AppendTime(Text::StringBuilderUTF *sb, Int32 t);
			void ClearDecode();
		public:
			AVIRVideoInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRVideoInfoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
