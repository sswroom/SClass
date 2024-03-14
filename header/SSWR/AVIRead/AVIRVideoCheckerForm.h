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
				NotNullPtr<Text::String> fileName;
				UOSInt index;
			} FileQueue;

			typedef struct
			{
				UOSInt index;
				Int32 status;
				Double t;
			} UpdateQueue;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnCancel;
			NotNullPtr<UI::GUICheckBox> chkAllowTimeSkip;
			NotNullPtr<UI::GUIListView> lvFiles;

			Data::ArrayList<FileQueue*> fileList;
			Sync::Mutex fileMut;
			Data::ArrayList<UpdateQueue*> updateList;
			Sync::Mutex updateMut;
			Media::VideoChecker checker;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;

			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> filesj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnAllowTimeSkipChange(AnyType userObj, Bool newVal);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			void CancelQueues();
		public:
			AVIRVideoCheckerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRVideoCheckerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
