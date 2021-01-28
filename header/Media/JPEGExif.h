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
		Data::ArrayList<ExifValue*> *exifs;

		void FreeExif(ExifValue *exif);
		ExifValue *DupExif(ExifValue *exif);
		ExifValue *GetExif(ExifValue *grp, Int32 id);
		void CalExifSize(Data::ArrayList<ExifValue*> *exifArr, Int32 *size, Int32 *endOfst);
		void GenExifBuff(UInt8 *buff, Data::ArrayList<ExifValue*> *exifArr, Int32 *startOfst, Int32 *otherOfst);
	public:
		JPEGExif();
		~JPEGExif();

		ExifValue *AddExifGroup(Int32 id);
		void SetExif(ExifValue *grp, Int32 id, Int32 numerator, Int32 denominator);
		void SetExif(ExifValue *grp, Int32 id, const Char *s);
		void SetExif(ExifValue *grp, Int32 id, const UInt8 *s, Int32 size);
		void SetExifUnk(ExifValue *grp, Int32 id, const UInt8 *s, Int32 size);
		void SetExif(ExifValue *grp, Int32 id, UInt16 val);
		void SetExif(ExifValue *grp, Int32 id, UInt32 val);
		void SetExif(ExifValue *grp, Int32 id, const Int32 *val, Int32 cnt);
		void DelExif(ExifValue *grp, Int32 id);

		Bool WriteExif(IO::Stream *input, IO::Stream *output);
	};
};
