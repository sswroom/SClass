#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEBATCHCONVFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEBATCHCONVFORM
#include "IO/FileExporter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageBatchConvForm : public UI::GUIForm
		{
		private:
			struct ConvertSess
			{
				Sync::Mutex mut;
				IO::FileExporter *exporter;
				Int32 quality;
				Bool succ;
				Text::String *errMsg;
			};

			enum class ThreadStatus
			{
				NotStarted,
				Idle,
				Processing,
				Stopped
			};
			struct ThreadState
			{
				ThreadStatus status;
				AVIRImageBatchConvForm *me;
				Sync::Event *evt;
				ConvertSess *sess;
				Bool hasData;
				Bool toStop;
				Text::String *srcFile;
				Text::String *destFile;
			};
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblDir;
			UI::GUITextBox *txtDir;
			UI::GUIButton *btnBrowse;
			UI::GUILabel *lblOutFormat;
			UI::GUIRadioButton *radFormatJPG;
			UI::GUIRadioButton *radFormatWebP;
			UI::GUILabel *lblQuality;
			UI::GUITextBox *txtQuality;
			UI::GUICheckBox *chkSubdir;
			UI::GUITextBox *txtSubdir;
			UI::GUIButton *btnConvert;
			UOSInt nThreads;
			ThreadState *threadStates;
			Bool threadToStop;
			Sync::Event threadEvt;

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnConvertClicked(void *userObj);
			static UInt32 __stdcall ThreadFunc(void *userObj);

			void StartThreads();
			void StopThreads();
			void MTConvertFile(ConvertSess *sess, Text::CStringNN srcFile, Text::CStringNN destFile);
			void ConvertFile(ConvertSess *sess, Text::CStringNN srcFile, Text::CStringNN destFile);
		public:
			AVIRImageBatchConvForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRImageBatchConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
