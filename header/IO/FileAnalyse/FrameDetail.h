#ifndef _SM_IO_FILEANALYSE_FRAMEDETAIL
#define _SM_IO_FILEANALYSE_FRAMEDETAIL
#include "Data/ArrayList.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF.h"

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
				FT_TEXT
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
			UInt32 size;

			Data::ArrayList<const UTF8Char*> *headers;
			Data::ArrayList<FieldInfo*> *fields;

			void FreeFieldInfo(FieldInfo *field);
			void AddFieldInfo(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value, FieldType fieldType);
		public:
			FrameDetail(UInt64 ofst, UInt32 size);
			virtual ~FrameDetail();

			UInt64 GetOffset();
			UInt32 GetSize();
			const FieldInfo *GetFieldInfo(UInt64 ofst);

			virtual void AddHeader(const UTF8Char *header);
			virtual void AddField(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddSubfield(UInt32 ofst, UInt32 size, const UTF8Char *name, const UTF8Char *value);
			virtual void AddFieldSeperstor(UInt32 ofst, const UTF8Char *name);
			virtual void AddText(UInt32 ofst, const UTF8Char *name);

			void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
