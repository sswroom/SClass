#ifndef _SM_IO_FILEANALYSE_OSGBFILEANALYSE
#define _SM_IO_FILEANALYSE_OSGBFILEANALYSE
#include "Data/ArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class OSGBFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UInt64 fileOfst;
				UInt64 dataSize;
				UInt32 nameSize;
				UInt32 objectId;
				UTF8Char className[129];
			} SceneObject;

			Optional<IO::StreamData> fd;
			Data::ArrayListNN<SceneObject> sceneObjects;
			UInt64 fileSize;
			UInt32 signatureHigh;
			UInt32 signatureLow;
			UInt32 contentType;
			UInt32 version;
			UInt32 attributes;
			UInt32 compressionSize;
			UInt8 compressionFlag;
			UIntOS headerSize;
			Bool isWrappedHeader;
			Bool isCompressed;
			Bool isBigEndian;

			static Text::CStringNN ContentTypeGetName(UInt32 contentType, Bool isWrappedHeader);
			static Bool IsClassName(UnsafeArray<const UInt8> className, UInt32 nameSize);
			static void __stdcall FreeSceneObject(NN<SceneObject> obj);
			void ParseSceneObjects(NN<IO::StreamData> fd);
		public:
			OSGBFileAnalyse(NN<IO::StreamData> fd);
			virtual ~OSGBFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
