#ifndef _SM_DATA_COMPRESS_DEFLATER
#define _SM_DATA_COMPRESS_DEFLATER
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		struct DeflateCompressor;

		enum class DeflateStatus
		{
			BadParam = -2,
			PutBufFailed = -1,
			Okay = 0,
			Done = 1
		};

		enum class DeflateFlush
		{
			NoFlush = 0,
			SyncFlush = 2,
			FullFlush = 3,
			Finish = 4
		};

		enum class DeflateResult
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

		class Deflater : public IO::Stream
		{
		public:
			enum class CompLevel
			{
				NoCompression = 0,
				BestSpeed = 1,
				BestCompression = 9,
				UberCompression = 10,
				DefaultLevel = 6,
				DefaultCompression = -1
			};

			enum class CompStrategy
			{
        		DefaultStrategy = 0,
        		Filtered = 1,
        		HuffmanOnly = 2,
        		RLE = 3,
		        Fixed = 4
			};

		private:
			struct SymFreq;

			NN<IO::Stream> srcStm;

			const UInt8 *next_in; /* pointer to next byte to read */
			UInt32 avail_in;        /* number of bytes available at next_in */
			UInt32 total_in;            /* total number of bytes consumed so far */

			UInt8 *next_out; /* pointer to next byte to write */
			UInt32 avail_out;  /* number of bytes that can be written to next_out */
			UInt32 total_out;      /* total number of bytes produced so far */

			Char *msg;                       /* error msg (unused) */
			NN<DeflateCompressor> state; /* internal state, allocated by zalloc/zfree */

			UInt32 adler;    /* adler32 of the source or uncompressed data */
			UInt32 reserved; /* not used */

			static const UInt32 s_tdefl_num_probes[];
			static const UInt8 s_tdefl_packed_code_size_syms_swizzle[];
			static const UInt32 mz_bitmasks[];
			static const UInt16 s_tdefl_len_sym[];
			static const UInt8 s_tdefl_len_extra[];
			static const UInt8 s_tdefl_small_dist_sym[];
			static const UInt8 s_tdefl_small_dist_extra[];
			static const UInt8 s_tdefl_large_dist_sym[];
			static const UInt8 s_tdefl_large_dist_extra[];

			static UInt32 CreateCompFlagsFromParams(CompLevel level, Bool hasHeader, CompStrategy strategy);
			static void DeflateInit(NN<DeflateCompressor> d, UInt32 flags);
		    static void RecordLiteral(NN<DeflateCompressor> d, UInt8 lit);
		    static void FindMatch(NN<DeflateCompressor> d, UInt32 lookahead_pos, UInt32 max_dist, UInt32 max_match_len, UInt32 *pMatch_dist, UInt32 *pMatch_len);
		    static void RecordMatch(NN<DeflateCompressor> d, UInt32 match_len, UInt32 match_dist);
			static SymFreq *RadixSortSyms(UInt32 num_syms, SymFreq *pSyms0, SymFreq *pSyms1);
			static void CalculateMinimumRedundancy(SymFreq *A, Int32 n);
			static void HuffmanEnforceMaxCodeSize(Int32 *pNum_codes, Int32 code_list_len, Int32 max_code_size);
		    static void OptimizeHuffmanTable(NN<DeflateCompressor> d, Int32 table_num, Int32 table_len, Int32 code_size_limit, Bool static_table);
		    static void StartDynamicBlock(NN<DeflateCompressor> d);
		    static void StartStaticBlock(NN<DeflateCompressor> d);
			static Bool CompressLzCodes(NN<DeflateCompressor> d);
		    static Bool CompressBlock(NN<DeflateCompressor> d, Bool static_block);
			static Bool CompressNormal(NN<DeflateCompressor> d);
			static DeflateStatus FlushOutputBuffer(NN<DeflateCompressor> d);
		    static Int32 FlushBlock(NN<DeflateCompressor> d, DeflateFlush flush);
			static DeflateStatus Compress(NN<DeflateCompressor> d, const void *pIn_buf, UOSInt *pIn_buf_size, void *pOut_buf, UOSInt *pOut_buf_size, DeflateFlush flush);
			DeflateResult Deflate(DeflateFlush flush);
		public:
			Deflater(NN<IO::Stream> srcStm, CompLevel level, Bool hasHeader);
			virtual ~Deflater();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;

			static Bool CompressDirect(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff, CompLevel level, Bool hasHeader);
		};
	}
}
#endif
