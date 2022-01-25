#ifndef _SM_IO_FILEANALYSE_FRAMEDETAIL
#define _SM_IO_FILEANALYSE_FRAMEDETAIL
#include "Data/ArrayList.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FrameDetail : public IO::FileAnalyse::FrameDetailHandler
		{
		public:
			enum FieldType
			{
				FT_FIELD,
				FT_SUBFIELD,
				FT_SEPERATOR,
				FT_TEXT,
				FT_SUBFRAME
			};

			struct FieldInfo
			{
				UInt32 ofst;
				UInt32 size;
				const UTF8Char *name;
				const UTF8Char *value;
				FieldType fieldType;
			};
		private:
			UInt64 ofst;
			UInt64 size;

			Data::ArrayList<const UTF8Char*> *headers;
			Data::ArrayList<FieldInfo*> *fields;

			void FreeFieldInfo(FieldInfo *field);
			void AddFieldInfo(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value, FieldType fieldType);
		public:
			FrameDetail(UInt64 ofst, UInt64 size);
			virtual ~FrameDetail();

			UInt64 GetOffset();
			UInt64 GetSize();
			UOSInt GetFieldInfos(UInt64 ofst, Data::ArrayList<const FieldInfo*> *fieldList);

			virtual void AddHeader(const UTF8Char *header);
			virtual void AddField(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddSubfield(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddFieldSeperstor(UOSInt ofst, const UTF8Char *name);
			virtual void AddText(UOSInt ofst, const UTF8Char *name);
			virtual void AddSubframe(UOSInt ofst, UOSInt size);

			void ToString(Text::StringBuilderUTF8 *sb);
		};
	}
}
#endif
