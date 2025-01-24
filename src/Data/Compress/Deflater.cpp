#include "Stdafx.h"
#include "Crypto/Hash/Adler32.h"
#include "Data/Compress/Deflater.h"
#include <assert.h>

#define DEFLATER_CLEAR_ARR(obj) memset((obj), 0, sizeof(obj))
#define DEFLATER_ADLER32_INIT (1)
#define DEFLATER_ASSERT(x) assert(x)

#ifdef _MSC_VER
#define DEFLATER_MACRO_END while (0, 0)
#else
#define DEFLATER_MACRO_END while (0)
#endif

#define DEFLATER_MAX_SUPPORTED_HUFF_CODESIZE 32

#define TDEFL_PUT_BITS(b, l)                                       \
    do                                                             \
    {                                                              \
        UInt32 bits = b;                                          \
        UInt32 len = l;                                           \
        DEFLATER_ASSERT(bits <= ((1U << len) - 1U));                     \
        d->m_bit_buffer |= (bits << d->m_bits_in);                 \
        d->m_bits_in += len;                                       \
        while (d->m_bits_in >= 8)                                  \
        {                                                          \
            if (d->m_pOutput_buf < d->m_pOutput_buf_end)           \
                *d->m_pOutput_buf++ = (UInt8)(d->m_bit_buffer); \
            d->m_bit_buffer >>= 8;                                 \
            d->m_bits_in -= 8;                                     \
        }                                                          \
    }                                                              \
    DEFLATER_MACRO_END

#define TDEFL_RLE_PREV_CODE_SIZE()                                                                                       \
    {                                                                                                                    \
        if (rle_repeat_count)                                                                                            \
        {                                                                                                                \
            if (rle_repeat_count < 3)                                                                                    \
            {                                                                                                            \
                d->m_huff_count[2][prev_code_size] = (UInt16)(d->m_huff_count[2][prev_code_size] + rle_repeat_count); \
                while (rle_repeat_count--)                                                                               \
                    packed_code_sizes[num_packed_code_sizes++] = prev_code_size;                                         \
            }                                                                                                            \
            else                                                                                                         \
            {                                                                                                            \
                d->m_huff_count[2][16] = (UInt16)(d->m_huff_count[2][16] + 1);                                        \
                packed_code_sizes[num_packed_code_sizes++] = 16;                                                         \
                packed_code_sizes[num_packed_code_sizes++] = (UInt8)(rle_repeat_count - 3);                           \
            }                                                                                                            \
            rle_repeat_count = 0;                                                                                        \
        }                                                                                                                \
    }

#define TDEFL_RLE_ZERO_CODE_SIZE()                                                         \
    {                                                                                      \
        if (rle_z_count)                                                                   \
        {                                                                                  \
            if (rle_z_count < 3)                                                           \
            {                                                                              \
                d->m_huff_count[2][0] = (UInt16)(d->m_huff_count[2][0] + rle_z_count);  \
                while (rle_z_count--)                                                      \
                    packed_code_sizes[num_packed_code_sizes++] = 0;                        \
            }                                                                              \
            else if (rle_z_count <= 10)                                                    \
            {                                                                              \
                d->m_huff_count[2][17] = (UInt16)(d->m_huff_count[2][17] + 1);          \
                packed_code_sizes[num_packed_code_sizes++] = 17;                           \
                packed_code_sizes[num_packed_code_sizes++] = (UInt8)(rle_z_count - 3);  \
            }                                                                              \
            else                                                                           \
            {                                                                              \
                d->m_huff_count[2][18] = (UInt16)(d->m_huff_count[2][18] + 1);          \
                packed_code_sizes[num_packed_code_sizes++] = 18;                           \
                packed_code_sizes[num_packed_code_sizes++] = (UInt8)(rle_z_count - 11); \
            }                                                                              \
            rle_z_count = 0;                                                               \
        }                                                                                  \
    }

enum
{
	TDEFL_MAX_HUFF_TABLES = 3,
	TDEFL_MAX_HUFF_SYMBOLS_0 = 288,
	TDEFL_MAX_HUFF_SYMBOLS_1 = 32,
	TDEFL_MAX_HUFF_SYMBOLS_2 = 19,
	TDEFL_LZ_DICT_SIZE = 32768,
	TDEFL_LZ_DICT_SIZE_MASK = TDEFL_LZ_DICT_SIZE - 1,
	TDEFL_MIN_MATCH_LEN = 3,
	TDEFL_MAX_MATCH_LEN = 258
};

/* TDEFL_OUT_BUF_SIZE MUST be large enough to hold a single entire compressed output block (using static/fixed Huffman codes). */
#if DEFLATER_LESS_MEMORY
    enum
    {
        DEFLATER_LZ_CODE_BUF_SIZE = 24 * 1024,
        DEFLATER_OUT_BUF_SIZE = (DEFLATER_LZ_CODE_BUF_SIZE * 13) / 10,
        DEFLATER_MAX_HUFF_SYMBOLS = 288,
        DEFLATER_LZ_HASH_BITS = 12,
        DEFLATER_LEVEL1_HASH_SIZE_MASK = 4095,
        DEFLATER_LZ_HASH_SHIFT = (DEFLATER_LZ_HASH_BITS + 2) / 3,
        DEFLATER_LZ_HASH_SIZE = 1 << DEFLATER_LZ_HASH_BITS
    };
#else
enum
{
    DEFLATER_LZ_CODE_BUF_SIZE = 64 * 1024,
    DEFLATER_OUT_BUF_SIZE = (UInt32)((DEFLATER_LZ_CODE_BUF_SIZE * 13) / 10),
    DEFLATER_MAX_HUFF_SYMBOLS = 288,
    DEFLATER_LZ_HASH_BITS = 15,
    DEFLATER_LEVEL1_HASH_SIZE_MASK = 4095,
    DEFLATER_LZ_HASH_SHIFT = (DEFLATER_LZ_HASH_BITS + 2) / 3,
    DEFLATER_LZ_HASH_SIZE = 1 << DEFLATER_LZ_HASH_BITS
};
#endif

enum
{
	DEFLATER_HUFFMAN_ONLY = 0,
	TDEFL_DEFAULT_MAX_PROBES = 128,
	TDEFL_MAX_PROBES_MASK = 0xFFF
};

enum
{
	TDEFL_WRITE_ZLIB_HEADER = 0x01000,
	TDEFL_COMPUTE_ADLER32 = 0x02000,
	TDEFL_GREEDY_PARSING_FLAG = 0x04000,
	TDEFL_NONDETERMINISTIC_PARSING_FLAG = 0x08000,
	TDEFL_RLE_MATCHES = 0x10000,
	TDEFL_FILTER_MATCHES = 0x20000,
	TDEFL_FORCE_ALL_STATIC_BLOCKS = 0x40000,
	TDEFL_FORCE_ALL_RAW_BLOCKS = 0x80000
};

struct Data::Compress::DeflateCompressor
{
	UInt32 m_flags, m_max_probes[2];
	Int32 m_greedy_parsing;
	UInt32 m_adler32, m_lookahead_pos, m_lookahead_size, m_dict_size;
	UInt8 *m_pLZ_code_buf, *m_pLZ_flags, *m_pOutput_buf, *m_pOutput_buf_end;
	UInt32 m_num_flags_left, m_total_lz_bytes, m_lz_code_buf_dict_pos, m_bits_in, m_bit_buffer;
	UInt32 m_saved_match_dist, m_saved_match_len, m_saved_lit, m_output_flush_ofs, m_output_flush_remaining, m_finished, m_block_index, m_wants_to_finish;
	DeflateStatus m_prev_return_status;
	const void *m_pIn_buf;
	void *m_pOut_buf;
	UOSInt *m_pIn_buf_size, *m_pOut_buf_size;
	DeflateFlush m_flush;
	const UInt8 *m_pSrc;
	UOSInt m_src_buf_left, m_out_buf_ofs;
	UInt8 m_dict[TDEFL_LZ_DICT_SIZE + TDEFL_MAX_MATCH_LEN - 1];
	UInt16 m_huff_count[TDEFL_MAX_HUFF_TABLES][DEFLATER_MAX_HUFF_SYMBOLS];
	UInt16 m_huff_codes[TDEFL_MAX_HUFF_TABLES][DEFLATER_MAX_HUFF_SYMBOLS];
	UInt8 m_huff_code_sizes[TDEFL_MAX_HUFF_TABLES][DEFLATER_MAX_HUFF_SYMBOLS];
	UInt8 m_lz_code_buf[DEFLATER_LZ_CODE_BUF_SIZE];
	UInt16 m_next[TDEFL_LZ_DICT_SIZE];
	UInt16 m_hash[DEFLATER_LZ_HASH_SIZE];
	UInt8 m_output_buf[DEFLATER_OUT_BUF_SIZE];
};

struct Data::Compress::Deflater::SymFreq
{
	UInt16 m_key, m_sym_index;
};

const UInt32 Data::Compress::Deflater::s_tdefl_num_probes[] = { 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 };
const UInt8 Data::Compress::Deflater::s_tdefl_packed_code_size_syms_swizzle[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
const UInt32 Data::Compress::Deflater::mz_bitmasks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };
const UInt16 Data::Compress::Deflater::s_tdefl_len_sym[] = {
	257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
	273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
	277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
	279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
	281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
	282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
	283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
	284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285
};
const UInt8 Data::Compress::Deflater::s_tdefl_len_extra[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0
};
const UInt8 Data::Compress::Deflater::s_tdefl_small_dist_sym[] = {
	0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
	11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
};
const UInt8 Data::Compress::Deflater::s_tdefl_small_dist_extra[] = {
	0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7
};
const UInt8 Data::Compress::Deflater::s_tdefl_large_dist_sym[] = {
	0, 0, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};
const UInt8 Data::Compress::Deflater::s_tdefl_large_dist_extra[] = {
	0, 0, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
};

UInt32 Data::Compress::Deflater::CreateCompFlagsFromParams(CompLevel level, Bool hasHeader, CompStrategy strategy)
{
	UInt32 comp_flags = s_tdefl_num_probes[((OSInt)level >= 0) ? Math_Min(10, (OSInt)level) : (OSInt)CompLevel::DefaultLevel] | (((OSInt)level <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);
	if (hasHeader)
		comp_flags |= TDEFL_WRITE_ZLIB_HEADER;

	if (level != CompLevel::NoCompression)
		comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
	else if (strategy == CompStrategy::Filtered)
		comp_flags |= TDEFL_FILTER_MATCHES;
	else if (strategy == CompStrategy::HuffmanOnly)
		comp_flags &= ~TDEFL_MAX_PROBES_MASK;
	else if (strategy == CompStrategy::Fixed)
		comp_flags |= TDEFL_FORCE_ALL_STATIC_BLOCKS;
	else if (strategy == CompStrategy::RLE)
		comp_flags |= TDEFL_RLE_MATCHES;
	return comp_flags;
}

void Data::Compress::Deflater::DeflateInit(NN<DeflateCompressor> d, UInt32 flags)
{
	d->m_flags = (UInt32)(flags);
	d->m_max_probes[0] = 1 + ((flags & 0xFFF) + 2) / 3;
	d->m_greedy_parsing = (flags & TDEFL_GREEDY_PARSING_FLAG) != 0;
	d->m_max_probes[1] = 1 + (((flags & 0xFFF) >> 2) + 2) / 3;
	if (!(flags & TDEFL_NONDETERMINISTIC_PARSING_FLAG))
		DEFLATER_CLEAR_ARR(d->m_hash);
	d->m_lookahead_pos = d->m_lookahead_size = d->m_dict_size = d->m_total_lz_bytes = d->m_lz_code_buf_dict_pos = d->m_bits_in = 0;
	d->m_output_flush_ofs = d->m_output_flush_remaining = d->m_finished = d->m_block_index = d->m_bit_buffer = d->m_wants_to_finish = 0;
	d->m_pLZ_code_buf = d->m_lz_code_buf + 1;
	d->m_pLZ_flags = d->m_lz_code_buf;
	*d->m_pLZ_flags = 0;
	d->m_num_flags_left = 8;
	d->m_pOutput_buf = d->m_output_buf;
	d->m_pOutput_buf_end = d->m_output_buf;
	d->m_prev_return_status = DeflateStatus::Okay;
	d->m_saved_match_dist = d->m_saved_match_len = d->m_saved_lit = 0;
	d->m_adler32 = 1;
	d->m_pIn_buf = 0;
	d->m_pOut_buf = 0;
	d->m_pIn_buf_size = 0;
	d->m_pOut_buf_size = 0;
	d->m_flush = DeflateFlush::NoFlush;
	d->m_pSrc = 0;
	d->m_src_buf_left = 0;
	d->m_out_buf_ofs = 0;
	if (!(flags & TDEFL_NONDETERMINISTIC_PARSING_FLAG))
		DEFLATER_CLEAR_ARR(d->m_dict);
	memset(&d->m_huff_count[0][0], 0, sizeof(d->m_huff_count[0][0]) * TDEFL_MAX_HUFF_SYMBOLS_0);
	memset(&d->m_huff_count[1][0], 0, sizeof(d->m_huff_count[1][0]) * TDEFL_MAX_HUFF_SYMBOLS_1);
}

void Data::Compress::Deflater::RecordLiteral(NN<DeflateCompressor> d, UInt8 lit)
{
	d->m_total_lz_bytes++;
	*d->m_pLZ_code_buf++ = lit;
	*d->m_pLZ_flags = (UInt8)(*d->m_pLZ_flags >> 1);
	if (--d->m_num_flags_left == 0)
	{
		d->m_num_flags_left = 8;
		d->m_pLZ_flags = d->m_pLZ_code_buf++;
	}
	d->m_huff_count[0][lit]++;
}

void Data::Compress::Deflater::FindMatch(NN<DeflateCompressor> d, UInt32 lookahead_pos, UInt32 max_dist, UInt32 max_match_len, UInt32 *pMatch_dist, UInt32 *pMatch_len)
{
	UInt32 dist, pos = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK, match_len = *pMatch_len, probe_pos = pos, next_probe_pos, probe_len;
	UInt32 num_probes_left = d->m_max_probes[match_len >= 32];
	const UInt16 *s = (const UInt16 *)(d->m_dict + pos), *p, *q;
	UInt16 c01 = ReadUInt16(&d->m_dict[pos + match_len - 1]), s01 = ReadUInt16((UInt8*)s);
	DEFLATER_ASSERT(max_match_len <= TDEFL_MAX_MATCH_LEN);
	if (max_match_len <= match_len)
		return;
	for (;;)
	{
		for (;;)
		{
			if (--num_probes_left == 0)
				return;
#define TDEFL_PROBE                                                                             \
	next_probe_pos = d->m_next[probe_pos];                                                      \
	if ((!next_probe_pos) || ((dist = (UInt16)(lookahead_pos - next_probe_pos)) > max_dist)) \
		return;                                                                                 \
	probe_pos = next_probe_pos & TDEFL_LZ_DICT_SIZE_MASK;                                       \
	if (ReadUInt16(&d->m_dict[probe_pos + match_len - 1]) == c01)                \
		break;
			TDEFL_PROBE;
			TDEFL_PROBE;
			TDEFL_PROBE;
		}
		if (!dist)
			break;
		q = (const UInt16 *)(d->m_dict + probe_pos);
		if (ReadUInt16((const UInt8*)q) != s01)
			continue;
		p = s;
		probe_len = 32;
		do
		{
		} while ((ReadUInt16(++p) == ReadUInt16(++q)) && (ReadUInt16(++p) == ReadUInt16(++q)) &&
					(ReadUInt16(++p) == ReadUInt16(++q)) && (ReadUInt16(++p) == ReadUInt16(++q)) && (--probe_len > 0));
		if (!probe_len)
		{
			*pMatch_dist = dist;
			*pMatch_len = Math_Min(max_match_len, (UInt32)TDEFL_MAX_MATCH_LEN);
			break;
		}
		else if ((probe_len = ((UInt32)(p - s) * 2) + (UInt32)(*(const UInt8 *)p == *(const UInt8 *)q)) > match_len)
		{
			*pMatch_dist = dist;
			if ((*pMatch_len = match_len = Math_Min(max_match_len, probe_len)) == max_match_len)
				break;
			c01 = ReadUInt16(&d->m_dict[pos + match_len - 1]);
		}
	}
}

void Data::Compress::Deflater::RecordMatch(NN<DeflateCompressor> d, UInt32 match_len, UInt32 match_dist)
{
	UInt32 s0, s1;

	DEFLATER_ASSERT((match_len >= TDEFL_MIN_MATCH_LEN) && (match_dist >= 1) && (match_dist <= TDEFL_LZ_DICT_SIZE));

	d->m_total_lz_bytes += match_len;

	d->m_pLZ_code_buf[0] = (UInt8)(match_len - TDEFL_MIN_MATCH_LEN);

	match_dist -= 1;
	d->m_pLZ_code_buf[1] = (UInt8)(match_dist & 0xFF);
	d->m_pLZ_code_buf[2] = (UInt8)(match_dist >> 8);
	d->m_pLZ_code_buf += 3;

	*d->m_pLZ_flags = (UInt8)((*d->m_pLZ_flags >> 1) | 0x80);
	if (--d->m_num_flags_left == 0)
	{
		d->m_num_flags_left = 8;
		d->m_pLZ_flags = d->m_pLZ_code_buf++;
	}

	s0 = s_tdefl_small_dist_sym[match_dist & 511];
	s1 = s_tdefl_large_dist_sym[(match_dist >> 8) & 127];
	d->m_huff_count[1][(match_dist < 512) ? s0 : s1]++;
	d->m_huff_count[0][s_tdefl_len_sym[match_len - TDEFL_MIN_MATCH_LEN]]++;
}

Data::Compress::Deflater::SymFreq *Data::Compress::Deflater::RadixSortSyms(UInt32 num_syms, SymFreq *pSyms0, SymFreq *pSyms1)
{
	UInt32 total_passes = 2, pass_shift, pass, i, hist[256 * 2];
	SymFreq *pCur_syms = pSyms0, *pNew_syms = pSyms1;
	DEFLATER_CLEAR_ARR(hist);
	for (i = 0; i < num_syms; i++)
	{
		UInt32 freq = pSyms0[i].m_key;
		hist[freq & 0xFF]++;
		hist[256 + ((freq >> 8) & 0xFF)]++;
	}
	while ((total_passes > 1) && (num_syms == hist[(total_passes - 1) * 256]))
		total_passes--;
	for (pass_shift = 0, pass = 0; pass < total_passes; pass++, pass_shift += 8)
	{
		const UInt32 *pHist = &hist[pass << 8];
		UInt32 offsets[256], cur_ofs = 0;
		for (i = 0; i < 256; i++)
		{
			offsets[i] = cur_ofs;
			cur_ofs += pHist[i];
		}
		for (i = 0; i < num_syms; i++)
			pNew_syms[offsets[(pCur_syms[i].m_key >> pass_shift) & 0xFF]++] = pCur_syms[i];
		{
			SymFreq *t = pCur_syms;
			pCur_syms = pNew_syms;
			pNew_syms = t;
		}
	}
	return pCur_syms;
}

void Data::Compress::Deflater::CalculateMinimumRedundancy(SymFreq *A, Int32 n)
{
	Int32 root, leaf, next, avbl, used, dpth;
	if (n == 0)
		return;
	else if (n == 1)
	{
		A[0].m_key = 1;
		return;
	}
	A[0].m_key += A[1].m_key;
	root = 0;
	leaf = 2;
	for (next = 1; next < n - 1; next++)
	{
		if (leaf >= n || A[root].m_key < A[leaf].m_key)
		{
			A[next].m_key = A[root].m_key;
			A[root++].m_key = (UInt16)next;
		}
		else
			A[next].m_key = A[leaf++].m_key;
		if (leaf >= n || (root < next && A[root].m_key < A[leaf].m_key))
		{
			A[next].m_key = (UInt16)(A[next].m_key + A[root].m_key);
			A[root++].m_key = (UInt16)next;
		}
		else
			A[next].m_key = (UInt16)(A[next].m_key + A[leaf++].m_key);
	}
	A[n - 2].m_key = 0;
	for (next = n - 3; next >= 0; next--)
		A[next].m_key = A[A[next].m_key].m_key + 1;
	avbl = 1;
	used = dpth = 0;
	root = n - 2;
	next = n - 1;
	while (avbl > 0)
	{
		while (root >= 0 && (int)A[root].m_key == dpth)
		{
			used++;
			root--;
		}
		while (avbl > used)
		{
			A[next--].m_key = (UInt16)(dpth);
			avbl--;
		}
		avbl = 2 * used;
		dpth++;
		used = 0;
	}
}

void Data::Compress::Deflater::HuffmanEnforceMaxCodeSize(Int32 *pNum_codes, Int32 code_list_len, Int32 max_code_size)
{
	Int32 i;
	UInt32 total = 0;
	if (code_list_len <= 1)
		return;
	for (i = max_code_size + 1; i <= DEFLATER_MAX_SUPPORTED_HUFF_CODESIZE; i++)
		pNum_codes[max_code_size] += pNum_codes[i];
	for (i = max_code_size; i > 0; i--)
		total += (((UInt32)pNum_codes[i]) << (max_code_size - i));
	while (total != (1UL << max_code_size))
	{
		pNum_codes[max_code_size]--;
		for (i = max_code_size - 1; i > 0; i--)
			if (pNum_codes[i])
			{
				pNum_codes[i]--;
				pNum_codes[i + 1] += 2;
				break;
			}
		total--;
	}
}

void Data::Compress::Deflater::OptimizeHuffmanTable(NN<DeflateCompressor> d, Int32 table_num, Int32 table_len, Int32 code_size_limit, Bool static_table)
{
	Int32 i, j, l, num_codes[1 + DEFLATER_MAX_SUPPORTED_HUFF_CODESIZE];
	UInt32 next_code[DEFLATER_MAX_SUPPORTED_HUFF_CODESIZE + 1];
	DEFLATER_CLEAR_ARR(num_codes);
	if (static_table)
	{
		for (i = 0; i < table_len; i++)
			num_codes[d->m_huff_code_sizes[table_num][i]]++;
	}
	else
	{
		SymFreq syms0[DEFLATER_MAX_HUFF_SYMBOLS], syms1[DEFLATER_MAX_HUFF_SYMBOLS], *pSyms;
		Int32 num_used_syms = 0;
		const UInt16 *pSym_count = &d->m_huff_count[table_num][0];
		for (i = 0; i < table_len; i++)
			if (pSym_count[i])
			{
				syms0[num_used_syms].m_key = (UInt16)pSym_count[i];
				syms0[num_used_syms++].m_sym_index = (UInt16)i;
			}

		pSyms = RadixSortSyms(num_used_syms, syms0, syms1);
		CalculateMinimumRedundancy(pSyms, num_used_syms);

		for (i = 0; i < num_used_syms; i++)
			num_codes[pSyms[i].m_key]++;

		HuffmanEnforceMaxCodeSize(num_codes, num_used_syms, code_size_limit);

		DEFLATER_CLEAR_ARR(d->m_huff_code_sizes[table_num]);
		DEFLATER_CLEAR_ARR(d->m_huff_codes[table_num]);
		for (i = 1, j = num_used_syms; i <= code_size_limit; i++)
			for (l = num_codes[i]; l > 0; l--)
				d->m_huff_code_sizes[table_num][pSyms[--j].m_sym_index] = (UInt8)(i);
	}

	next_code[1] = 0;
	for (j = 0, i = 2; i <= code_size_limit; i++)
		next_code[i] = j = ((j + num_codes[i - 1]) << 1);

	for (i = 0; i < table_len; i++)
	{
		UInt32 rev_code = 0, code, code_size;
		if ((code_size = d->m_huff_code_sizes[table_num][i]) == 0)
			continue;
		code = next_code[code_size]++;
		for (l = code_size; l > 0; l--, code >>= 1)
			rev_code = (rev_code << 1) | (code & 1);
		d->m_huff_codes[table_num][i] = (UInt16)rev_code;
	}
}

void Data::Compress::Deflater::StartDynamicBlock(NN<DeflateCompressor> d)
{
	Int32 num_lit_codes, num_dist_codes, num_bit_lengths;
	UInt32 i, total_code_sizes_to_pack, num_packed_code_sizes, rle_z_count, rle_repeat_count, packed_code_sizes_index;
	UInt8 code_sizes_to_pack[TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1], packed_code_sizes[TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1], prev_code_size = 0xFF;

	d->m_huff_count[0][256] = 1;

	OptimizeHuffmanTable(d, 0, TDEFL_MAX_HUFF_SYMBOLS_0, 15, false);
	OptimizeHuffmanTable(d, 1, TDEFL_MAX_HUFF_SYMBOLS_1, 15, false);

	for (num_lit_codes = 286; num_lit_codes > 257; num_lit_codes--)
		if (d->m_huff_code_sizes[0][num_lit_codes - 1])
			break;
	for (num_dist_codes = 30; num_dist_codes > 1; num_dist_codes--)
		if (d->m_huff_code_sizes[1][num_dist_codes - 1])
			break;

	memcpy(code_sizes_to_pack, &d->m_huff_code_sizes[0][0], num_lit_codes);
	memcpy(code_sizes_to_pack + num_lit_codes, &d->m_huff_code_sizes[1][0], num_dist_codes);
	total_code_sizes_to_pack = num_lit_codes + num_dist_codes;
	num_packed_code_sizes = 0;
	rle_z_count = 0;
	rle_repeat_count = 0;

	memset(&d->m_huff_count[2][0], 0, sizeof(d->m_huff_count[2][0]) * TDEFL_MAX_HUFF_SYMBOLS_2);
	for (i = 0; i < total_code_sizes_to_pack; i++)
	{
		UInt8 code_size = code_sizes_to_pack[i];
		if (!code_size)
		{
			TDEFL_RLE_PREV_CODE_SIZE();
			if (++rle_z_count == 138)
			{
				TDEFL_RLE_ZERO_CODE_SIZE();
			}
		}
		else
		{
			TDEFL_RLE_ZERO_CODE_SIZE();
			if (code_size != prev_code_size)
			{
				TDEFL_RLE_PREV_CODE_SIZE();
				d->m_huff_count[2][code_size] = (UInt16)(d->m_huff_count[2][code_size] + 1);
				packed_code_sizes[num_packed_code_sizes++] = code_size;
			}
			else if (++rle_repeat_count == 6)
			{
				TDEFL_RLE_PREV_CODE_SIZE();
			}
		}
		prev_code_size = code_size;
	}
	if (rle_repeat_count)
	{
		TDEFL_RLE_PREV_CODE_SIZE();
	}
	else
	{
		TDEFL_RLE_ZERO_CODE_SIZE();
	}

	OptimizeHuffmanTable(d, 2, TDEFL_MAX_HUFF_SYMBOLS_2, 7, false);

	TDEFL_PUT_BITS(2, 2);

	TDEFL_PUT_BITS(num_lit_codes - 257, 5);
	TDEFL_PUT_BITS(num_dist_codes - 1, 5);

	for (num_bit_lengths = 18; num_bit_lengths >= 0; num_bit_lengths--)
		if (d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[num_bit_lengths]])
			break;
	num_bit_lengths = Math_Max(4, (num_bit_lengths + 1));
	TDEFL_PUT_BITS(num_bit_lengths - 4, 4);
	for (i = 0; (int)i < num_bit_lengths; i++)
		TDEFL_PUT_BITS(d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[i]], 3);

	for (packed_code_sizes_index = 0; packed_code_sizes_index < num_packed_code_sizes;)
	{
		UInt32 code = packed_code_sizes[packed_code_sizes_index++];
		DEFLATER_ASSERT(code < TDEFL_MAX_HUFF_SYMBOLS_2);
		TDEFL_PUT_BITS(d->m_huff_codes[2][code], d->m_huff_code_sizes[2][code]);
		if (code >= 16)
			TDEFL_PUT_BITS(packed_code_sizes[packed_code_sizes_index++], "\02\03\07"[code - 16]);
	}
}

void Data::Compress::Deflater::StartStaticBlock(NN<DeflateCompressor> d)
{
	UInt32 i;
	UInt8 *p = &d->m_huff_code_sizes[0][0];

	for (i = 0; i <= 143; ++i)
		*p++ = 8;
	for (; i <= 255; ++i)
		*p++ = 9;
	for (; i <= 279; ++i)
		*p++ = 7;
	for (; i <= 287; ++i)
		*p++ = 8;

	memset(d->m_huff_code_sizes[1], 5, 32);

	OptimizeHuffmanTable(d, 0, 288, 15, true);
	OptimizeHuffmanTable(d, 1, 32, 15, true);

	TDEFL_PUT_BITS(1, 2);
}

Bool Data::Compress::Deflater::CompressLzCodes(NN<DeflateCompressor> d)
{
	UInt32 flags;
	UInt8 *pLZ_codes;
	UInt8 *pOutput_buf = d->m_pOutput_buf;
	UInt8 *pLZ_code_buf_end = d->m_pLZ_code_buf;
	UInt64 bit_buffer = d->m_bit_buffer;
	UInt32 bits_in = d->m_bits_in;

#define TDEFL_PUT_BITS_FAST(b, l)                    \
	{                                                \
		bit_buffer |= (((UInt64)(b)) << bits_in); \
		bits_in += (l);                              \
	}

	flags = 1;
	for (pLZ_codes = d->m_lz_code_buf; pLZ_codes < pLZ_code_buf_end; flags >>= 1)
	{
		if (flags == 1)
			flags = *pLZ_codes++ | 0x100;

		if (flags & 1)
		{
			UInt32 s0, s1, n0, n1, sym, num_extra_bits;
			UInt32 match_len = pLZ_codes[0];
			UInt32 match_dist = (pLZ_codes[1] | (pLZ_codes[2] << 8));
			pLZ_codes += 3;

			DEFLATER_ASSERT(d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
			TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][s_tdefl_len_sym[match_len]], d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
			TDEFL_PUT_BITS_FAST(match_len & mz_bitmasks[s_tdefl_len_extra[match_len]], s_tdefl_len_extra[match_len]);

			/* This sequence coaxes MSVC into using cmov's vs. jmp's. */
			s0 = s_tdefl_small_dist_sym[match_dist & 511];
			n0 = s_tdefl_small_dist_extra[match_dist & 511];
			s1 = s_tdefl_large_dist_sym[match_dist >> 8];
			n1 = s_tdefl_large_dist_extra[match_dist >> 8];
			sym = (match_dist < 512) ? s0 : s1;
			num_extra_bits = (match_dist < 512) ? n0 : n1;

			DEFLATER_ASSERT(d->m_huff_code_sizes[1][sym]);
			TDEFL_PUT_BITS_FAST(d->m_huff_codes[1][sym], d->m_huff_code_sizes[1][sym]);
			TDEFL_PUT_BITS_FAST(match_dist & mz_bitmasks[num_extra_bits], num_extra_bits);
		}
		else
		{
			UInt32 lit = *pLZ_codes++;
			DEFLATER_ASSERT(d->m_huff_code_sizes[0][lit]);
			TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);

			if (((flags & 2) == 0) && (pLZ_codes < pLZ_code_buf_end))
			{
				flags >>= 1;
				lit = *pLZ_codes++;
				DEFLATER_ASSERT(d->m_huff_code_sizes[0][lit]);
				TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);

				if (((flags & 2) == 0) && (pLZ_codes < pLZ_code_buf_end))
				{
					flags >>= 1;
					lit = *pLZ_codes++;
					DEFLATER_ASSERT(d->m_huff_code_sizes[0][lit]);
					TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);
				}
			}
		}

		if (pOutput_buf >= d->m_pOutput_buf_end)
			return false;

		memcpy(pOutput_buf, &bit_buffer, sizeof(UInt64));
		pOutput_buf += (bits_in >> 3);
		bit_buffer >>= (bits_in & ~7);
		bits_in &= 7;
	}

#undef TDEFL_PUT_BITS_FAST

	d->m_pOutput_buf = pOutput_buf;
	d->m_bits_in = 0;
	d->m_bit_buffer = 0;

	while (bits_in)
	{
		UInt32 n = Math_Min(bits_in, 16);
		TDEFL_PUT_BITS((UInt32)bit_buffer & mz_bitmasks[n], n);
		bit_buffer >>= n;
		bits_in -= n;
	}

	TDEFL_PUT_BITS(d->m_huff_codes[0][256], d->m_huff_code_sizes[0][256]);

	return (d->m_pOutput_buf < d->m_pOutput_buf_end);
}

Bool Data::Compress::Deflater::CompressBlock(NN<DeflateCompressor> d, Bool static_block)
{
	if (static_block)
		StartStaticBlock(d);
	else
		StartDynamicBlock(d);
	return CompressLzCodes(d);
}

Bool Data::Compress::Deflater::CompressNormal(NN<DeflateCompressor> d)
{
	const UInt8 *pSrc = d->m_pSrc;
	UOSInt src_buf_left = d->m_src_buf_left;
	DeflateFlush flush = d->m_flush;

	while ((src_buf_left) || ((flush != DeflateFlush::NoFlush) && (d->m_lookahead_size)))
	{
		UInt32 len_to_move, cur_match_dist, cur_match_len, cur_pos;
		/* Update dictionary and hash chains. Keeps the lookahead size equal to TDEFL_MAX_MATCH_LEN. */
		if ((d->m_lookahead_size + d->m_dict_size) >= (TDEFL_MIN_MATCH_LEN - 1))
		{
			UInt32 dst_pos = (d->m_lookahead_pos + d->m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK, ins_pos = d->m_lookahead_pos + d->m_lookahead_size - 2;
			UInt32 hash = (d->m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << DEFLATER_LZ_HASH_SHIFT) ^ d->m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK];
			UInt32 num_bytes_to_process = (UInt32)Math_Min(src_buf_left, TDEFL_MAX_MATCH_LEN - d->m_lookahead_size);
			const UInt8 *pSrc_end = pSrc ? pSrc + num_bytes_to_process : 0;
			src_buf_left -= num_bytes_to_process;
			d->m_lookahead_size += num_bytes_to_process;
			while (pSrc != pSrc_end)
			{
				UInt8 c = *pSrc++;
				d->m_dict[dst_pos] = c;
				if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
					d->m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c;
				hash = ((hash << DEFLATER_LZ_HASH_SHIFT) ^ c) & (DEFLATER_LZ_HASH_SIZE - 1);
				d->m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
				d->m_hash[hash] = (UInt16)(ins_pos);
				dst_pos = (dst_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK;
				ins_pos++;
			}
		}
		else
		{
			while ((src_buf_left) && (d->m_lookahead_size < TDEFL_MAX_MATCH_LEN))
			{
				UInt8 c = *pSrc++;
				UInt32 dst_pos = (d->m_lookahead_pos + d->m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK;
				src_buf_left--;
				d->m_dict[dst_pos] = c;
				if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
					d->m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c;
				if ((++d->m_lookahead_size + d->m_dict_size) >= TDEFL_MIN_MATCH_LEN)
				{
					UInt32 ins_pos = d->m_lookahead_pos + (d->m_lookahead_size - 1) - 2;
					UInt32 hash = ((d->m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << (DEFLATER_LZ_HASH_SHIFT * 2)) ^ (d->m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK] << DEFLATER_LZ_HASH_SHIFT) ^ c) & (DEFLATER_LZ_HASH_SIZE - 1);
					d->m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
					d->m_hash[hash] = (UInt16)(ins_pos);
				}
			}
		}
		d->m_dict_size = Math_Min(TDEFL_LZ_DICT_SIZE - d->m_lookahead_size, d->m_dict_size);
		if ((flush == DeflateFlush::NoFlush) && (d->m_lookahead_size < TDEFL_MAX_MATCH_LEN))
			break;

		/* Simple lazy/greedy parsing state machine. */
		len_to_move = 1;
		cur_match_dist = 0;
		cur_match_len = d->m_saved_match_len ? d->m_saved_match_len : (TDEFL_MIN_MATCH_LEN - 1);
		cur_pos = d->m_lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK;
		if (d->m_flags & (TDEFL_RLE_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS))
		{
			if ((d->m_dict_size) && (!(d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS)))
			{
				UInt8 c = d->m_dict[(cur_pos - 1) & TDEFL_LZ_DICT_SIZE_MASK];
				cur_match_len = 0;
				while (cur_match_len < d->m_lookahead_size)
				{
					if (d->m_dict[cur_pos + cur_match_len] != c)
						break;
					cur_match_len++;
				}
				if (cur_match_len < TDEFL_MIN_MATCH_LEN)
					cur_match_len = 0;
				else
					cur_match_dist = 1;
			}
		}
		else
		{
			FindMatch(d, d->m_lookahead_pos, d->m_dict_size, d->m_lookahead_size, &cur_match_dist, &cur_match_len);
		}
		if (((cur_match_len == TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 8U * 1024U)) || (cur_pos == cur_match_dist) || ((d->m_flags & TDEFL_FILTER_MATCHES) && (cur_match_len <= 5)))
		{
			cur_match_dist = cur_match_len = 0;
		}
		if (d->m_saved_match_len)
		{
			if (cur_match_len > d->m_saved_match_len)
			{
				RecordLiteral(d, (UInt8)d->m_saved_lit);
				if (cur_match_len >= 128)
				{
					RecordMatch(d, cur_match_len, cur_match_dist);
					d->m_saved_match_len = 0;
					len_to_move = cur_match_len;
				}
				else
				{
					d->m_saved_lit = d->m_dict[cur_pos];
					d->m_saved_match_dist = cur_match_dist;
					d->m_saved_match_len = cur_match_len;
				}
			}
			else
			{
				RecordMatch(d, d->m_saved_match_len, d->m_saved_match_dist);
				len_to_move = d->m_saved_match_len - 1;
				d->m_saved_match_len = 0;
			}
		}
		else if (!cur_match_dist)
			RecordLiteral(d, d->m_dict[Math_Min(cur_pos, sizeof(d->m_dict) - 1)]);
		else if ((d->m_greedy_parsing) || (d->m_flags & TDEFL_RLE_MATCHES) || (cur_match_len >= 128))
		{
			RecordMatch(d, cur_match_len, cur_match_dist);
			len_to_move = cur_match_len;
		}
		else
		{
			d->m_saved_lit = d->m_dict[Math_Min(cur_pos, sizeof(d->m_dict) - 1)];
			d->m_saved_match_dist = cur_match_dist;
			d->m_saved_match_len = cur_match_len;
		}
		/* Move the lookahead forward by len_to_move bytes. */
		d->m_lookahead_pos += len_to_move;
		DEFLATER_ASSERT(d->m_lookahead_size >= len_to_move);
		d->m_lookahead_size -= len_to_move;
		d->m_dict_size = Math_Min(d->m_dict_size + len_to_move, (UInt32)TDEFL_LZ_DICT_SIZE);
		/* Check if it's time to flush the current LZ codes to the internal output buffer. */
		if ((d->m_pLZ_code_buf > &d->m_lz_code_buf[DEFLATER_LZ_CODE_BUF_SIZE - 8]) ||
			((d->m_total_lz_bytes > 31 * 1024) && (((((UInt32)(d->m_pLZ_code_buf - d->m_lz_code_buf) * 115) >> 7) >= d->m_total_lz_bytes) || (d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS))))
		{
			Int32 n;
			d->m_pSrc = pSrc;
			d->m_src_buf_left = src_buf_left;
			if ((n = FlushBlock(d, DeflateFlush::NoFlush)) != 0)
				return (n < 0) ? false : true;
		}
	}

	d->m_pSrc = pSrc;
	d->m_src_buf_left = src_buf_left;
	return true;
}
			
Data::Compress::DeflateStatus Data::Compress::Deflater::FlushOutputBuffer(NN<DeflateCompressor> d)
{
	if (d->m_pIn_buf_size)
	{
		*d->m_pIn_buf_size = d->m_pSrc - (const UInt8 *)d->m_pIn_buf;
	}

	if (d->m_pOut_buf_size)
	{
		UOSInt n = Math_Min(*d->m_pOut_buf_size - d->m_out_buf_ofs, d->m_output_flush_remaining);
		memcpy((UInt8 *)d->m_pOut_buf + d->m_out_buf_ofs, d->m_output_buf + d->m_output_flush_ofs, n);
		d->m_output_flush_ofs += (UInt32)n;
		d->m_output_flush_remaining -= (UInt32)n;
		d->m_out_buf_ofs += n;

		*d->m_pOut_buf_size = d->m_out_buf_ofs;
	}

	return (d->m_finished && !d->m_output_flush_remaining) ? DeflateStatus::Done : DeflateStatus::Okay;
}

Int32 Data::Compress::Deflater::FlushBlock(NN<DeflateCompressor> d, DeflateFlush flush)
{
	UInt32 saved_bit_buf, saved_bits_in;
	UInt8 *pSaved_output_buf;
	Bool comp_block_succeeded = false;
	Int32 n, use_raw_block = ((d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS) != 0) && (d->m_lookahead_pos - d->m_lz_code_buf_dict_pos) <= d->m_dict_size;
	UInt8 *pOutput_buf_start = ((*d->m_pOut_buf_size - d->m_out_buf_ofs) >= DEFLATER_OUT_BUF_SIZE) ? ((UInt8 *)d->m_pOut_buf + d->m_out_buf_ofs) : d->m_output_buf;

	d->m_pOutput_buf = pOutput_buf_start;
	d->m_pOutput_buf_end = d->m_pOutput_buf + DEFLATER_OUT_BUF_SIZE - 16;

	DEFLATER_ASSERT(!d->m_output_flush_remaining);
	d->m_output_flush_ofs = 0;
	d->m_output_flush_remaining = 0;

	*d->m_pLZ_flags = (UInt8)(*d->m_pLZ_flags >> d->m_num_flags_left);
	d->m_pLZ_code_buf -= (d->m_num_flags_left == 8);

	if ((d->m_flags & TDEFL_WRITE_ZLIB_HEADER) && (!d->m_block_index))
	{
		const UInt8 cmf = 0x78;
		UInt8 flg, flevel = 3;
		UInt32 header, i, mz_un = sizeof(s_tdefl_num_probes) / sizeof(UInt32);

		/* Determine compression level by reversing the process in tdefl_create_comp_flags_from_zip_params() */
		for (i = 0; i < mz_un; i++)
			if (s_tdefl_num_probes[i] == (d->m_flags & 0xFFF))
				break;

		if (i < 2)
			flevel = 0;
		else if (i < 6)
			flevel = 1;
		else if (i == 6)
			flevel = 2;

		header = cmf << 8 | (flevel << 6);
		header += 31 - (header % 31);
		flg = header & 0xFF;

		TDEFL_PUT_BITS(cmf, 8);
		TDEFL_PUT_BITS(flg, 8);
	}

	TDEFL_PUT_BITS(flush == DeflateFlush::Finish, 1);

	pSaved_output_buf = d->m_pOutput_buf;
	saved_bit_buf = d->m_bit_buffer;
	saved_bits_in = d->m_bits_in;

	if (!use_raw_block)
		comp_block_succeeded = CompressBlock(d, (d->m_flags & TDEFL_FORCE_ALL_STATIC_BLOCKS) || (d->m_total_lz_bytes < 48));

	/* If the block gets expanded, forget the current contents of the output buffer and send a raw block instead. */
	if (((use_raw_block) || ((d->m_total_lz_bytes) && ((d->m_pOutput_buf - pSaved_output_buf + 1U) >= d->m_total_lz_bytes))) &&
		((d->m_lookahead_pos - d->m_lz_code_buf_dict_pos) <= d->m_dict_size))
	{
		UInt32 i;
		d->m_pOutput_buf = pSaved_output_buf;
		d->m_bit_buffer = saved_bit_buf, d->m_bits_in = saved_bits_in;
		TDEFL_PUT_BITS(0, 2);
		if (d->m_bits_in)
		{
			TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
		}
		for (i = 2; i; --i, d->m_total_lz_bytes ^= 0xFFFF)
		{
			TDEFL_PUT_BITS(d->m_total_lz_bytes & 0xFFFF, 16);
		}
		for (i = 0; i < d->m_total_lz_bytes; ++i)
		{
			TDEFL_PUT_BITS(d->m_dict[(d->m_lz_code_buf_dict_pos + i) & TDEFL_LZ_DICT_SIZE_MASK], 8);
		}
	}
	/* Check for the extremely unlikely (if not impossible) case of the compressed block not fitting into the output buffer when using dynamic codes. */
	else if (!comp_block_succeeded)
	{
		d->m_pOutput_buf = pSaved_output_buf;
		d->m_bit_buffer = saved_bit_buf, d->m_bits_in = saved_bits_in;
		CompressBlock(d, true);
	}

	if (flush != DeflateFlush::NoFlush)
	{
		if (flush == DeflateFlush::Finish)
		{
			if (d->m_bits_in)
			{
				TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
			}
			if (d->m_flags & TDEFL_WRITE_ZLIB_HEADER)
			{
				UInt32 i, a = d->m_adler32;
				for (i = 0; i < 4; i++)
				{
					TDEFL_PUT_BITS((a >> 24) & 0xFF, 8);
					a <<= 8;
				}
			}
		}
		else
		{
			UInt32 i, z = 0;
			TDEFL_PUT_BITS(0, 3);
			if (d->m_bits_in)
			{
				TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
			}
			for (i = 2; i; --i, z ^= 0xFFFF)
			{
				TDEFL_PUT_BITS(z & 0xFFFF, 16);
			}
		}
	}

	DEFLATER_ASSERT(d->m_pOutput_buf < d->m_pOutput_buf_end);

	memset(&d->m_huff_count[0][0], 0, sizeof(d->m_huff_count[0][0]) * TDEFL_MAX_HUFF_SYMBOLS_0);
	memset(&d->m_huff_count[1][0], 0, sizeof(d->m_huff_count[1][0]) * TDEFL_MAX_HUFF_SYMBOLS_1);

	d->m_pLZ_code_buf = d->m_lz_code_buf + 1;
	d->m_pLZ_flags = d->m_lz_code_buf;
	d->m_num_flags_left = 8;
	d->m_lz_code_buf_dict_pos += d->m_total_lz_bytes;
	d->m_total_lz_bytes = 0;
	d->m_block_index++;

	if ((n = (Int32)(d->m_pOutput_buf - pOutput_buf_start)) != 0)
	{
		if (pOutput_buf_start == d->m_output_buf)
		{
			Int32 bytes_to_copy = (Int32)Math_Min((UOSInt)n, (UOSInt)(*d->m_pOut_buf_size - d->m_out_buf_ofs));
			memcpy((UInt8 *)d->m_pOut_buf + d->m_out_buf_ofs, d->m_output_buf, bytes_to_copy);
			d->m_out_buf_ofs += bytes_to_copy;
			if ((n -= bytes_to_copy) != 0)
			{
				d->m_output_flush_ofs = bytes_to_copy;
				d->m_output_flush_remaining = n;
			}
		}
		else
		{
			d->m_out_buf_ofs += n;
		}
	}

	return d->m_output_flush_remaining;
}

Data::Compress::DeflateStatus Data::Compress::Deflater::Compress(NN<DeflateCompressor> d, const void *pIn_buf, UOSInt *pIn_buf_size, void *pOut_buf, UOSInt *pOut_buf_size, DeflateFlush flush)
{
	d->m_pIn_buf = pIn_buf;
	d->m_pIn_buf_size = pIn_buf_size;
	d->m_pOut_buf = pOut_buf;
	d->m_pOut_buf_size = pOut_buf_size;
	d->m_pSrc = (const UInt8 *)(pIn_buf);
	d->m_src_buf_left = pIn_buf_size ? *pIn_buf_size : 0;
	d->m_out_buf_ofs = 0;
	d->m_flush = flush;

	if ((d->m_prev_return_status != DeflateStatus::Okay) ||
		(d->m_wants_to_finish && (flush != DeflateFlush::Finish)) || (pIn_buf_size && *pIn_buf_size && !pIn_buf) || (pOut_buf_size && *pOut_buf_size && !pOut_buf))
	{
		if (pIn_buf_size)
			*pIn_buf_size = 0;
		if (pOut_buf_size)
			*pOut_buf_size = 0;
		return (d->m_prev_return_status = DeflateStatus::BadParam);
	}
	d->m_wants_to_finish |= (flush == DeflateFlush::Finish);

	if ((d->m_output_flush_remaining) || (d->m_finished))
		return (d->m_prev_return_status = FlushOutputBuffer(d));

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
	if (((d->m_flags & TDEFL_MAX_PROBES_MASK) == 1) &&
		((d->m_flags & TDEFL_GREEDY_PARSING_FLAG) != 0) &&
		((d->m_flags & (TDEFL_FILTER_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS | TDEFL_RLE_MATCHES)) == 0))
	{
		if (!tdefl_compress_fast(d))
			return d->m_prev_return_status;
	}
	else
#endif /* #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN */
	{
		if (!CompressNormal(d))
			return d->m_prev_return_status;
	}

	if ((d->m_flags & (TDEFL_WRITE_ZLIB_HEADER | TDEFL_COMPUTE_ADLER32)) && (pIn_buf))
		d->m_adler32 = Adler32_Calc((const UInt8 *)pIn_buf, d->m_pSrc - (const UInt8 *)pIn_buf, d->m_adler32);

	if ((flush != DeflateFlush::NoFlush) && (!d->m_lookahead_size) && (!d->m_src_buf_left) && (!d->m_output_flush_remaining))
	{
		if ((OSInt)FlushBlock(d, flush) < 0)
			return d->m_prev_return_status;
		d->m_finished = (flush == DeflateFlush::Finish);
		if (flush == DeflateFlush::FullFlush)
		{
			DEFLATER_CLEAR_ARR(d->m_hash);
			DEFLATER_CLEAR_ARR(d->m_next);
			d->m_dict_size = 0;
		}
	}

	return (d->m_prev_return_status = FlushOutputBuffer(d));
}

Data::Compress::DeflateResult Data::Compress::Deflater::Deflate(DeflateFlush flush)
{
	UOSInt in_bytes, out_bytes;
	UInt32 orig_total_in, orig_total_out;
	DeflateResult mz_status = DeflateResult::Ok;

	if (((OSInt)flush < 0) || ((OSInt)flush > (OSInt)DeflateFlush::Finish) || (!this->next_out))
		return DeflateResult::StreamError;
	if (!this->avail_out)
		return DeflateResult::BufError;

	if (this->state->m_prev_return_status == DeflateStatus::Done)
		return (flush == DeflateFlush::Finish) ? DeflateResult::StreamEnd : DeflateResult::BufError;

	orig_total_in = this->total_in;
	orig_total_out = this->total_out;
	for (;;)
	{
		DeflateStatus defl_status;
		in_bytes = this->avail_in;
		out_bytes = this->avail_out;

		defl_status = Compress(this->state, this->next_in, &in_bytes, this->next_out, &out_bytes, flush);
		this->next_in += (UInt32)in_bytes;
		this->avail_in -= (UInt32)in_bytes;
		this->total_in += (UInt32)in_bytes;
		this->adler = this->state->m_adler32;

		this->next_out += (UInt32)out_bytes;
		this->avail_out -= (UInt32)out_bytes;
		this->total_out += (UInt32)out_bytes;

		if ((OSInt)defl_status < 0)
		{
			mz_status = DeflateResult::StreamError;
			break;
		}
		else if (defl_status == DeflateStatus::Done)
		{
			mz_status = DeflateResult::StreamEnd;
			break;
		}
		else if (!this->avail_out)
			break;
		else if ((!this->avail_in) && (flush != DeflateFlush::Finish))
		{
			if ((flush != DeflateFlush::NoFlush) || (this->total_in != orig_total_in) || (this->total_out != orig_total_out))
				break;
			return DeflateResult::BufError;
		}
	}
	return mz_status;
}

Data::Compress::Deflater::Deflater(NN<IO::Stream> srcStm, CompLevel level, Bool hasHeader) : IO::Stream(CSTR("Deflater"))
{
	UInt32 comp_flags = TDEFL_COMPUTE_ADLER32 | CreateCompFlagsFromParams(level, hasHeader, CompStrategy::DefaultStrategy);

	this->adler = DEFLATER_ADLER32_INIT;
	this->msg = 0;
	this->reserved = 0;
	this->total_in = 0;
	this->total_out = 0;
	this->state = MemAllocNN(DeflateCompressor);
	DeflateInit(this->state, comp_flags);
}

Data::Compress::Deflater::~Deflater()
{
	MemFreeNN(this->state);
}

Bool Data::Compress::Deflater::IsDown() const
{
	return false;
}

UOSInt Data::Compress::Deflater::Read(const Data::ByteArray &buff)
{
	return 0;
/*	UOSInt initSize = buff.GetSize();
	int ret;
	this->next_out = buff.Arr().Ptr();
	this->avail_out = (unsigned int)buff.GetSize();
	while (this->avail_out == initSize)
	{
		if (this->avail_in == 0)
		{
			UOSInt readSize = BUFFSIZE;
			if (this->clsData->srcLeng > 0)
			{
				this->clsData->stm.next_in = this->clsData->buff;
				if (this->clsData->srcLeng < BUFFSIZE)
				{
					readSize = (UOSInt)this->clsData->srcLeng;
				}
				readSize = this->clsData->srcStm->Read(Data::ByteArray(this->clsData->buff, readSize));
				if (readSize > 0 && this->clsData->hash.SetTo(hash))
				{
					hash->Calc(this->clsData->buff, readSize);
				}
				this->clsData->srcLeng -= readSize;
				this->clsData->stm.avail_in = (unsigned int)readSize;
			}

			if (this->clsData->srcLeng == 0 || readSize == 0)
			{
				ret = this->Deflate(DeflateFlush::Finish);
				if (this->clsData->stm.avail_out == initSize)
				{
					return 0;
				}
				else
				{
					return initSize - this->clsData->stm.avail_out;
				}
			}
		}
		ret = this->Deflate(DeflateFlush::NoFlush);
		if (ret != 0)
		{
			break;
		}
	}
	return initSize - this->clsData->stm.avail_out;*/
}

UOSInt Data::Compress::Deflater::Write(Data::ByteArrayR buff)
{
	return 0;
}

Int32 Data::Compress::Deflater::Flush()
{
	return 0;
}

void Data::Compress::Deflater::Close()
{

}

Bool Data::Compress::Deflater::Recover()
{
	return this->srcStm->Recover();
}

IO::StreamType Data::Compress::Deflater::GetStreamType() const
{
	return IO::StreamType::Deflater;
}

Bool Data::Compress::Deflater::CompressDirect(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff, CompLevel level, Bool hasHeader)
{
	UInt32 comp_flags = TDEFL_COMPUTE_ADLER32 | CreateCompFlagsFromParams(level, hasHeader, CompStrategy::DefaultStrategy);
	DeflateCompressor state;
	DeflateInit(state, comp_flags);

	UOSInt in_bytes, out_bytes;
	DeflateStatus defl_status;
	in_bytes = srcBuff.GetCount();
	out_bytes = destBuff.GetCount();
	UOSInt totalSize = 0;

	while (true)
	{
		defl_status = Compress(state, srcBuff.Arr().Ptr(), &in_bytes, destBuff.Arr().Ptr(), &out_bytes, (in_bytes != 0)?DeflateFlush::NoFlush:DeflateFlush::Finish);
		if (defl_status == DeflateStatus::Done)
		{
			outDestBuffSize.Set(out_bytes);
			return true;
		}
		if (defl_status != DeflateStatus::Okay)
		{
			printf("Status: %d, %d, %d\r\n", defl_status, in_bytes, out_bytes);
			printf("%d, %d, %d\r\n", state.m_out_buf_ofs, state.m_src_buf_left, state.m_prev_return_status);
			return false;
		}
		if (in_bytes == 0 && out_bytes == 0 && srcBuff.GetCount() == 0)
		{
			printf("Size 0: %d, %d, %d\r\n", defl_status, in_bytes, out_bytes);
			return false;
		}
		printf("Status: %d, %d, %d\r\n", defl_status, in_bytes, out_bytes);
		srcBuff += in_bytes;
		destBuff += out_bytes;
		in_bytes = srcBuff.GetCount();
		out_bytes = destBuff.GetCount();
	}
	return false;
}
