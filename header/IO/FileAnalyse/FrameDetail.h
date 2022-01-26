#ifndef _SM_IO_FILEANALYSE_FRAMEDETAIL
#define _SM_IO_FILEANALYSE_FRAMEDETAIL
#include "Data/ArrayList.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/String.h"
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
				Text::String *name;
				Text::String *value;
				FieldType fieldType;
			};
		private:
			UInt64 ofst;
			UInt64 size;

			Data::ArrayList<Text::String*> *headers;
			Data::ArrayList<FieldInfo*> *fields;

			void FreeFieldInfo(FieldInfo *field);
			void AddFieldInfo(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value, FieldType fieldType);
		public:
			FrameDetail(UInt64 ofst, UInt64 size);
			virtual ~FrameDetail();

			UInt64 GetOffset();
			UInt64 GetSize();
			UOSInt GetFieldInfos(UInt64 ofst, Data::ArrayList<const FieldInfo*> *fieldList);

			virtual void AddHeader(Text::CString header);
			virtual void AddField(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value);
			virtual void AddSubfield(UOSInt ofst, UOSInt size, Text::CString name, Text::CString value);
			virtual void AddFieldSeperstor(UOSInt ofst, Text::CString name);
			virtual void AddText(UOSInt ofst, Text::CString name);
			virtual void AddSubframe(UOSInt ofst, UOSInt size);

			void ToString(Text::StringBuilderUTF8 *sb);
		};
	}
}
#endif
