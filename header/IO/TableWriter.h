#ifndef _SM_IO_TABLEWRITER
#define _SM_IO_TABLEWRITER
#include "Data/Timestamp.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class TableWriter
	{
	protected:
		NN<IO::SeekableStream> stm;
	public:
		TableWriter(NN<IO::SeekableStream> stm);
		virtual ~TableWriter();
		
		virtual void NextRow() = 0;
		virtual void NextColNull() = 0;
		virtual void NextColStr(NN<Text::String> s) = 0;
		virtual void NextColStr(Text::CStringNN s) = 0;
		virtual void NextColI32(Int32 v) = 0;
		virtual void NextColU64(UInt64 v) = 0;
		virtual void NextColTS(Data::Timestamp ts) = 0;
		virtual void EndWrite() = 0;
	};
}

#endif
