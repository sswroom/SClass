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
				NN<Text::String> fileName;
				UIntOS index;
			} FileQueue;

			typedef struct
			{
				UIntOS index;
				Int32 status;
				Double t;
			} UpdateQueue;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUICheckBox> chkAllowTimeSkip;
			NN<UI::GUIListView> lvFiles;

			Data::ArrayListNN<FileQueue> fileList;
			Sync::Mutex fileMut;
			Data::ArrayListNN<UpdateQueue> updateList;
			Sync::Mutex updateMut;
			Media::VideoChecker checker;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;

			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> filesj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnAllowTimeSkipChange(AnyType userObj, Bool newVal);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall ProcessThread(AnyType userObj);
			void CancelQueues();
		public:
			AVIRVideoCheckerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRVideoCheckerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
