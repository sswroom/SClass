#ifndef _SM_UI_GUIPICTUREVIEW
#define _SM_UI_GUIPICTUREVIEW
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Media/DrawEngine.h"
#include "Media/StaticImage.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "UI/GUIDDrawControl.h"
#include "Parser/ParserList.h"

namespace UI
{
	class GUIPictureView : public GUIDDrawControl
	{
	private:
		typedef struct
		{
			Media::StaticImage *previewImg;
			const UTF8Char *fullPath;
			const UTF8Char *fileName;
			Bool isDir;
		} ScreenItem;
	private:
		GUICore *ui;
		Parser::ParserList *parsers;
		Media::ColorManagerSess *colorSess;
		Media::Resizer::LanczosResizer8_C8 *previewResizer;
		Media::DrawEngine *eng;
		Media::ImageAlphaBlend *alphaBlend;

		Data::ArrayList<ScreenItem*> *previewItems;
		Sync::Mutex *previewMut;

		Bool dirChanged;
		const UTF8Char *currDir;
		Sync::Mutex *threadMut;
		Sync::Event *threadEvt;
		Bool threadToStop;
		Bool threadRunning;

	private:
		static UInt32 __stdcall ThreadProc(void *userObj);
		Media::StaticImage *GenFolderImage();
		void UpdatePreview();
		void ClearPreviews();
	public:
		GUIPictureView(GUICore *ui, UI::GUIClientControl *parent, Parser::ParserList *parsers, Media::ColorManagerSess *colorSess, Media::DrawEngine *eng);
		virtual ~GUIPictureView();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);

		void SetDirectory(const UTF8Char *currDir);
		virtual void OnSurfaceCreated();
	};
};
#endif
