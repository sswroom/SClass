#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEVIEWERFORM
#include "IO/PackageFile.h"
#include "UI/GUIForm.h"
#include "UI/GUIPictureBoxDD.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageViewerForm : public UI::GUIForm
		{
		private:
			UI::GUIPictureBoxDD *pbImage;
			UI::GUIMainMenu *mnuMain;

			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Bool allowEnlarge;
			Int64 imgTimeoutTick;
			Media::ImageList *imgList;
			UOSInt imgIndex;
			IO::PackageFile *pkgFile;
			UOSInt fileIndex;
			Bool hideCursor;
			UOSInt noMoveCount;
			
			static void __stdcall OnFileDrop(void *userObj, Text::String **files, UOSInt fileCnt);
			static void __stdcall OnMoveToNext(void *userObj);
			static void __stdcall OnMoveToPrev(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAniTimerTick(void *userObj);
			static Bool __stdcall OnMouseMove(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn);
			static Bool IsImageFileName(const UTF8Char *fileName);
		public:
			AVIRImageViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRImageViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnFocus();
			virtual void OnMonitorChanged();

			void SetImage(Media::ImageList *imgList, Bool sameDir);
			Bool ParseFile(IO::IStreamData *fd);
		};
	}
}
#endif
