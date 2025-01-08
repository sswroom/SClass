#ifndef _SM_MEDIA_PROFILEDRESIZER
#define _SM_MEDIA_PROFILEDRESIZER
#include "Data/ArrayListNN.h"
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
			OT_PNG,
			OT_WEBPQUALITY
		} OutputType;

		typedef enum
		{
			ST_MAXSIZE,
			ST_DPI
		} SizeType;

		typedef struct
		{
			NN<Text::String> profileName;
			NN<Text::String> suffix;
			UInt32 targetSizeX;
			UInt32 targetSizeY;
			SizeType sizeType;
			OutputType outType;
			UInt32 outParam;
			Optional<Text::String> watermark;
		} ResizeProfile;

	private:
		Data::ArrayListNN<ResizeProfile> profiles;
		UOSInt currProfile;

		Media::Resizer::LanczosResizerLR_C32 *resizer;
		Media::Batch::BatchLoader *loader;
		Media::Batch::BatchToLRGB *conv;
		Media::Batch::BatchLimiter *limiter;
		Media::Batch::BatchResizer *bresizer;
		Media::Batch::BatchWatermarker *watermarker;
		Media::Batch::BatchSaver *saver;

		static void __stdcall ReleaseProfile(NN<ResizeProfile> profile);
	public:
		ProfiledResizer(NN<Parser::ParserList> parsers, Media::ColorManagerSess *colorSess, NN<Media::DrawEngine> deng);
		~ProfiledResizer();

		UOSInt GetProfileCount() const;
		UOSInt GetCurrProfileIndex();
		Optional<const ResizeProfile> GetCurrProfile();
		void SetCurrentProfile(UOSInt index);
		Optional<const ResizeProfile> GetProfile(UOSInt index) const;
		NN<const ResizeProfile> GetProfileNoCheck(UOSInt index) const;
		Bool AddProfile(Text::CStringNN profileName, Text::CStringNN suffix, UInt32 targetWidth, UInt32 targetHeight, OutputType outType, UInt32 outParam, Text::CString watermark, SizeType sizeType);
		Bool RemoveProfile(UOSInt index);

		void ProcessFile(Text::CStringNN fileName);
		Bool SaveProfile(Text::CStringNN fileName);
		Bool LoadProfile(Text::CStringNN fileName);
	};
}
#endif
