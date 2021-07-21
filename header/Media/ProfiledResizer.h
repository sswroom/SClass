#ifndef _SM_MEDIA_PROFILEDRESIZER
#define _SM_MEDIA_PROFILEDRESIZER
#include "Parser/ParserList.h"
#include "Media/Batch/BatchLimiter.h"
#include "Media/Batch/BatchLoader.h"
#include "Media/Batch/BatchResizer.h"
#include "Media/Batch/BatchToLRGB.h"
#include "Media/Batch/BatchWatermarker.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"

namespace Media
{
	class ProfiledResizer
	{
	public:
		typedef enum
		{
			OT_TIFF,
			OT_JPEGSIZE,
			OT_JPEGQUALITY,
			OT_PNG
		} OutputType;

		typedef enum
		{
			ST_MAXSIZE,
			ST_DPI
		} SizeType;

		typedef struct
		{
			const UTF8Char *profileName;
			const UTF8Char *suffix;
			UInt32 targetSizeX;
			UInt32 targetSizeY;
			SizeType sizeType;
			OutputType outType;
			UInt32 outParam;
			const UTF8Char *watermark;
		} ResizeProfile;

	private:
		Data::ArrayList<ResizeProfile *> *profiles;
		UOSInt currProfile;
		Parser::ParserList *parsers;

		Media::ColorManager *colorMgr;
		Media::Resizer::LanczosResizerLR_C32 *resizer;
		Media::Batch::BatchLoader *loader;
		Media::Batch::BatchToLRGB *conv;
		Media::Batch::BatchLimiter *limiter;
		Media::Batch::BatchResizer *bresizer;
		Media::Batch::BatchWatermarker *watermarker;
		Media::Batch::BatchSaver *saver;

		void ReleaseProfile(ResizeProfile *profile);
	public:
		ProfiledResizer(Parser::ParserList *parsers, Media::ColorManagerSess *colorSess, Media::DrawEngine *deng);
		~ProfiledResizer();

		UOSInt GetProfileCount();
		UOSInt GetCurrProfileIndex();
		const ResizeProfile *GetCurrProfile();
		void SetCurrentProfile(UOSInt index);
		const ResizeProfile *GetProfile(UOSInt index);
		Bool AddProfile(const UTF8Char *profileName, const UTF8Char *suffix, UInt32 targetWidth, UInt32 targetHeight, OutputType outType, UInt32 outParam, const UTF8Char *watermark, SizeType sizeType);
		Bool RemoveProfile(UOSInt index);

		void ProcessFile(const UTF8Char *fileName);
		Bool SaveProfile(const UTF8Char *fileName);
		Bool LoadProfile(const UTF8Char *fileName);
	};
}
#endif
