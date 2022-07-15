#ifndef _SM_SSWR_AVIREAD_AVIRGISTILEDOWNLOADFORM
#define _SM_SSWR_AVIREAD_AVIRGISTILEDOWNLOADFORM
#include "IO/SPackageFile.h"
#include "Map/TileMapLayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/IMapNavigator.h"
#include "Sync/Mutex.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISTileDownloadForm : public UI::GUIForm
		{
		private:
			struct ThreadStat
			{
				AVIRGISTileDownloadForm *me;
				Sync::Event *threadEvt;
				Int32 threadStat; //0 = not running, 1 = idle, 2 = has task, 3 = toStop
				UOSInt lyrId;
				Int64 imageId;
				Sync::Mutex *pkgMut;
				IO::SPackageFile *spkg;
				const UTF8Char *folderName;
				Map::TileMap *tileMap;
			};
		private:
			UI::GUIButton *btnArea;
			UI::GUIButton *btnSave;
			UI::GUIButton *btnStop;

			UI::GUILabel *lblLayer;
			UI::GUITextBox *txtLayer;
			UI::GUILabel *lblImages;
			UI::GUITextBox *txtImages;
			UI::GUILabel *lblError;
			UI::GUITextBox *txtError;

			SSWR::AVIRead::AVIRCore *core;
			Math::Coord2DDbl sel1;
			Math::Coord2DDbl sel2;
			Bool selecting;
			Bool isDown;
			Math::Coord2D<OSInt> downPos;

			Int32 errCnt;
			Sync::Event mainEvt;
			Bool stopDownload;
			UOSInt threadCnt;
			ThreadStat *threadStat;

			Map::TileMapLayer *lyr;
			IMapNavigator *navi;

			static Bool __stdcall OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos);
			static Bool __stdcall OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos);

			static void __stdcall OnAreaClicked(void *userObj);
			static void __stdcall OnSaveDirClicked(void *userObj);
			static void __stdcall OnSaveFileClicked(void *userObj);
			static void __stdcall OnStopClicked(void *userObj);

			void SaveTilesDir(const UTF8Char *folderName);
			void SaveTilesFile(Text::CString fileName);
			static UInt32 __stdcall ProcThread(void *userObj);
		public:
			AVIRGISTileDownloadForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::TileMapLayer *lyr, IMapNavigator *navi);
			virtual ~AVIRGISTileDownloadForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
