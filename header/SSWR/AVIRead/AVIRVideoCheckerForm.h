#ifndef _SM_SSWR_AVIREAD_AVIRVIDEOCHECKERFORM
#define _SM_SSWR_AVIREAD_AVIRVIDEOCHECKERFORM
#include "Media/VideoChecker.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVideoCheckerForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				const UTF8Char *fileName;
				UOSInt index;
			} FileQueue;

			typedef struct
			{
				UOSInt index;
				Int32 status;
				Double t;
			} UpdateQueue;
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnCancel;
			UI::GUICheckBox *chkAllowTimeSkip;
			UI::GUIListView *lvFiles;

			Data::ArrayList<FileQueue*> *fileList;
			Sync::Mutex *fileMut;
			Data::ArrayList<UpdateQueue*> *updateList;
			Sync::Mutex *updateMut;
			Media::VideoChecker *checker;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event *threadEvt;

			static void __stdcall OnFileHandler(void *userObj, const UTF8Char **files, OSInt nFilesj);
			static void __stdcall OnCancelClicked(void *userObj);
			static void __stdcall OnAllowTimeSkipChange(void *userObj, Bool newVal);
			static void __stdcall OnTimerTick(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			void CancelQueues();
		public:
			AVIRVideoCheckerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRVideoCheckerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
