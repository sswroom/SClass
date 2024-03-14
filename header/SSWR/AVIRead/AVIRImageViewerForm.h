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
			NotNullPtr<UI::GUIPictureBoxDD> pbImage;
			NotNullPtr<UI::GUIMainMenu> mnuMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Bool allowEnlarge;
			Int64 imgTimeoutTick;
			Media::ImageList *imgList;
			UOSInt imgIndex;
			IO::PackageFile *pkgFile;
			UOSInt fileIndex;
			Bool hideCursor;
			UOSInt noMoveCount;
			
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnMoveToNext(void *userObj);
			static void __stdcall OnMoveToPrev(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAniTimerTick(void *userObj);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool IsImageFileName(Text::CString fileName);
		public:
			AVIRImageViewerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImageViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnFocus();
			virtual void OnMonitorChanged();

			void SetImage(Media::ImageList *imgList, Bool sameDir);
			Bool ParseFile(NotNullPtr<IO::StreamData> fd);
		};
	}
}
#endif
