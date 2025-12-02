#ifndef _SM_IO_CSVTABLEWRITER
#define _SM_IO_CSVTABLEWRITER
#include "IO/BufferedOutputStream.h"
#include "IO/TableWriter.h"

namespace IO
{
	class CSVTableWriter : public TableWriter
	{
	private:
		IO::BufferedOutputStream bos;
		Bool lineBegin;
	public:
		CSVTableWriter(NN<IO::SeekableStream> stm);
		virtual ~CSVTableWriter();
		
		virtual void NextRow();
		virtual void NextColNull();
		virtual void NextColStr(NN<Text::String> s);
		virtual void NextColStr(Text::CStringNN s);
		virtual void NextColI32(Int32 v);
		virtual void NextColU64(UInt64 v);
		virtual void NextColTS(Data::Timestamp ts);
		virtual void EndWrite();
	};
}

#endif
