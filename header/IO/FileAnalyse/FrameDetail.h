#ifndef _SM_IO_FILEANALYSE_FRAMEDETAIL
#define _SM_IO_FILEANALYSE_FRAMEDETAIL
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/ByteBuffer.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FileAnalyser;
		class FileAnalyserCreator;
		class FrameDetail : public IO::FileAnalyse::FrameDetailHandler
		{
		public:
			enum FieldType
			{
				FT_FIELD,
				FT_SUBFIELD,
				FT_SEPERATOR,
				FT_TEXT,
				FT_SUBFRAME,
				FT_AREA
			};

			struct FieldInfo
			{
				UInt64 ofst;
				UInt64 size;
				NN<Text::String> name;
				NN<Text::String> value;
				FieldType fieldType;
			};
		private:
			UInt64 ofst;
			UInt64 size;

			Data::ArrayListStringNN headers;
			Data::ArrayListNN<FieldInfo> fields;

			Optional<Data::ByteBuffer> devrivedBuff;
			Optional<IO::FileAnalyse::FileAnalyserCreator> devrivedAnalyse;
			
			static void __stdcall FreeFieldInfo(NN<FieldInfo> field);
			void AddFieldInfo(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value, FieldType fieldType);
		public:
			FrameDetail(UInt64 ofst, UInt64 size);
			virtual ~FrameDetail();

			UInt64 GetOffset() const;
			UInt64 GetSize() const;
			UOSInt GetFieldInfos(UInt64 ofst, NN<Data::ArrayListNN<const FieldInfo>> fieldList) const;
			UOSInt GetAreaInfos(UInt64 ofst, NN<Data::ArrayListNN<const FieldInfo>> areaList) const;
			Optional<Data::ByteBuffer> GetDevrivedBuff() const;
			Optional<IO::FileAnalyse::FileAnalyser> CreateDevrivedAnaylse() const;

			virtual void AddHeader(Text::CStringNN header);
			virtual void AddField(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value);
			virtual void AddSubfield(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value);
			virtual void AddFieldSeperstor(UInt64 ofst, Text::CStringNN name);
			virtual void AddText(UInt64 ofst, Text::CStringNN name);
			virtual void AddSubframe(UInt64 ofst, UInt64 size);
			void AddArea(UInt64 ofst, UOSInt size, Text::CStringNN name);
			void SetDevrivedBuff(Data::ByteArrayR buff);
			void SetDevrivedAnaylse(NN<IO::FileAnalyse::FileAnalyserCreator> analyseCreator);

			void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
