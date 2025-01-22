#ifndef _SM_DATA_COMPRESS_INFLATER
#define _SM_DATA_COMPRESS_INFLATER
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		enum class InflateStatus
		{
			FailedCannotMakeProgress = -4,
			BadParam = -3,
			Adler32Mismatch = -2,
			Failed = -1,
			Done = 0,
			NeedsMoreInput = 1,
			HasMoreOutput = 2
		};

		enum class InflateResult
		{
			Ok = 0,
			StreamEnd = 1,
			NeedDict = 2,
			Errno = -1,
			StreamError = -2,
			DataError = -3,
			MemError = -4,
			BufError = -5,
			VersionError = -6,
			ParamError = -10000
		};

		struct InflateState;
		struct InflateDecompressor;

		class Inflater : public IO::Stream
		{
		private:
			NN<IO::Stream> decStm;
			NN<InflateState> state;
			UnsafeArray<const UInt8> next_in;
			UOSInt avail_in;

    		static void ClearTree(NN<InflateDecompressor> r);
		    static InflateStatus Decompress(NN<InflateDecompressor> r, UnsafeArray<const UInt8> pIn_buf_next, InOutParam<UOSInt> pIn_buf_size, UnsafeArray<UInt8> pOut_buf_start, UnsafeArray<UInt8> pOut_buf_next, InOutParam<UOSInt> pOut_buf_size, const UInt32 decomp_flags);
			InflateResult Inflate(Bool isEnd);
		public:
			Inflater(NN<IO::Stream> decStm, Bool hasHeader);
			virtual ~Inflater();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			static Bool DecompressDirect(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff, Bool hasHeader);
		};
	}
}
#endif
