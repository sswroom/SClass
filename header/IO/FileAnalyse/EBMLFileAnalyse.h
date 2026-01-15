#ifndef _SM_IO_FILEANALYSE_EBMLFILEANALYSE
#define _SM_IO_FILEANALYSE_EBMLFILEANALYSE
#include "Data/SyncArrayListNN.hpp"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace FileAnalyse
	{
		class EBMLFileAnalyse : public IO::FileAnalyse::FileAnalyser
		{
		private:
			typedef struct
			{
				UIntOS lev;
				UInt64 fileOfst;
				UIntOS packSize;
				UIntOS hdrSize;
				UInt8 packType[4];
			} PackInfo;

			typedef enum
			{
				ET_UNKNOWN,
				ET_MASTER,
				ET_SINT,
				ET_UINT,
				ET_FLOAT,
				ET_STRING,
				ET_UTF8,
				ET_DATE,
				ET_BINARY
			} ElementType;

			typedef struct
			{
				UInt32 elementId;
				ElementType type;
				Text::CStringNN elementName;
			} ElementInfo;
		private:
			Optional<IO::StreamData> fd;
			Data::SyncArrayListNN<PackInfo> packs;

			Bool pauseParsing;
			Sync::Thread thread;
			UIntOS maxLev;

			static ElementInfo elements[];

			static UnsafeArray<const UInt8> ReadInt(UnsafeArray<const UInt8> buffPtr, OutParam<UInt64> val, OptOut<UInt32> intSize);
			static Optional<const ElementInfo> GetElementInfo(UInt32 elementId);
			void ParseRange(UIntOS lev, UInt64 ofst, UInt64 size);
			static void __stdcall ParseThread(NN<Sync::Thread> thread);
			UIntOS GetFrameIndex(UIntOS lev, UInt64 ofst);
		public:
			EBMLFileAnalyse(NN<IO::StreamData> fd);
			virtual ~EBMLFileAnalyse();

			virtual Text::CStringNN GetFormatName();
			virtual UIntOS GetFrameCount();
			virtual Bool GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual Bool GetFrameDetail(UIntOS index, NN<Text::StringBuilderUTF8> sb);
			virtual UIntOS GetFrameIndex(UInt64 ofst);
			virtual Optional<FrameDetail> GetFrameDetail(UIntOS index);

			virtual Bool IsError();
			virtual Bool IsParsing();
			virtual Bool TrimPadding(Text::CStringNN outputFile);
		};
	}
}
#endif
