#ifndef _SM_IO_FILEANALYSE_FRAMEDETAIL
#define _SM_IO_FILEANALYSE_FRAMEDETAIL
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
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
				UInt64 ofst;
				UInt64 size;
				NotNullPtr<Text::String> name;
				NotNullPtr<Text::String> value;
				FieldType fieldType;
			};
		private:
			UInt64 ofst;
			UInt64 size;

			Data::ArrayListNN<Text::String> headers;
			Data::ArrayList<FieldInfo*> fields;

			void FreeFieldInfo(FieldInfo *field);
			void AddFieldInfo(UInt64 ofst, UInt64 size, Text::CString name, Text::CString value, FieldType fieldType);
		public:
			FrameDetail(UInt64 ofst, UInt64 size);
			virtual ~FrameDetail();

			UInt64 GetOffset() const;
			UInt64 GetSize() const;
			UOSInt GetFieldInfos(UInt64 ofst, Data::ArrayList<const FieldInfo*> *fieldList) const;

			virtual void AddHeader(Text::CStringNN header);
			virtual void AddField(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value);
			virtual void AddSubfield(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value);
			virtual void AddFieldSeperstor(UInt64 ofst, Text::CStringNN name);
			virtual void AddText(UInt64 ofst, Text::CStringNN name);
			virtual void AddSubframe(UInt64 ofst, UInt64 size);
			void AddArea(UInt64 ofst, UOSInt size, Text::CStringNN name);

			void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
