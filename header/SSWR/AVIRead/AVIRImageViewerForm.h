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
			NN<UI::GUIPictureBoxDD> pbImage;
			NN<UI::GUIMainMenu> mnuMain;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			Bool allowEnlarge;
			Int64 imgTimeoutTick;
			Optional<Media::ImageList> imgList;
			UOSInt imgIndex;
			Optional<IO::PackageFile> pkgFile;
			UOSInt fileIndex;
			Bool hideCursor;
			UOSInt noMoveCount;
			
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnMoveToNext(AnyType userObj);
			static void __stdcall OnMoveToPrev(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnAniTimerTick(AnyType userObj);
			static UI::EventState __stdcall OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static Bool IsImageFileName(Text::CStringNN fileName);
		public:
			AVIRImageViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRImageViewerForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnFocus();
			virtual void OnMonitorChanged();

			void SetImage(Optional<Media::ImageList> imgList, Bool sameDir);
			Bool ParseFile(NN<IO::StreamData> fd);
		};
	}
}
#endif
