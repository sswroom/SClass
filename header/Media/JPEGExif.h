#ifndef _SM_MEDIA_JPEGEXIF
#define _SM_MEDIA_JPEGEXIF
#include "Data/ArrayListNN.h"
namespace Media
{
	class JPEGExif
	{
	private:
		typedef enum
		{
			EXIF_TYPE_DEL,
			EXIF_TYPE_NUM,
			EXIF_TYPE_STR,
			EXIF_TYPE_BYTE,
			EXIF_TYPE_SHORT,
			EXIF_TYPE_LONG,
			EXIF_TYPE_NUMARR,
			EXIF_TYPE_GROUP,
			EXIF_TYPE_UNK
		} ExifType;

	public:
		typedef struct
		{
			Int32 id;
			ExifType t;
			Int32 numerator;
			Int32 denominator;
			Char *s;
		} ExifValue;

	private:
		Data::ArrayListNN<ExifValue> exifs;

		static void FreeExif(NN<ExifValue> exif);
		NN<ExifValue> DupExif(NN<ExifValue> exif);
		NN<ExifValue> GetExif(Optional<ExifValue> grp, Int32 id);
		void CalExifSize(NN<Data::ArrayListNN<ExifValue>> exifArr, OutParam<UOSInt> size, OutParam<UOSInt> endOfst);
		void GenExifBuff(UInt8 *buff, NN<Data::ArrayListNN<ExifValue>> exifArr, InOutParam<UOSInt> startOfst, InOutParam<UOSInt> otherOfst);
	public:
		JPEGExif();
		~JPEGExif();

		NN<ExifValue> AddExifGroup(Int32 id);
		void SetExif(Optional<ExifValue> grp, Int32 id, Int32 numerator, Int32 denominator);
		void SetExif(Optional<ExifValue> grp, Int32 id, const Char *s);
		void SetExif(Optional<ExifValue> grp, Int32 id, const UInt8 *s, Int32 size);
		void SetExifUnk(Optional<ExifValue> grp, Int32 id, const UInt8 *s, Int32 size);
		void SetExif(Optional<ExifValue> grp, Int32 id, UInt16 val);
		void SetExif(Optional<ExifValue> grp, Int32 id, UInt32 val);
		void SetExif(Optional<ExifValue> grp, Int32 id, const Int32 *val, Int32 cnt);
		void DelExif(Optional<ExifValue> grp, Int32 id);

		Bool WriteExif(NN<IO::Stream> input, NN<IO::Stream> output);
	};
}
#endif