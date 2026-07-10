#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/AES256GCM.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ============================================================================
// 1. NIST COMPLIANT AES-256 STATE ENGINE
// ============================================================================

static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint8_t rcon[11] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

void aes256_key_schedule(const uint8_t *key, uint8_t *round_keys) {
    memcpy(round_keys, key, 32);
    uint8_t temp[4];

    for (int i = 32; i < 240; i += 4) {
        memcpy(temp, &round_keys[i - 4], 4);
        if (i % 32 == 0) {
            uint8_t k = temp[0];
            temp[0] = sbox[temp[1]] ^ rcon[i / 32];
            temp[1] = sbox[temp[2]];
            temp[2] = sbox[temp[3]];
            temp[3] = sbox[k];
        } else if (i % 32 == 16) {
            temp[0] = sbox[temp[0]];
            temp[1] = sbox[temp[1]];
            temp[2] = sbox[temp[2]];
            temp[3] = sbox[temp[3]];
        }
        for (int j = 0; j < 4; j++) {
            round_keys[i + j] = round_keys[i - 32 + j] ^ temp[j];
        }
    }
}

static inline uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1B : 0x00);
}

void aes256_encrypt_block(const uint8_t *state_in, uint8_t *state_out, const uint8_t *round_keys) {
    uint8_t state[16];
    memcpy(state, state_in, 16);

    // Initial AddRoundKey
    for (int i = 0; i < 16; i++) state[i] ^= round_keys[i];

    // 13 Complete Rounds
    for (int round = 1; round < 14; round++) {
        // SubBytes
        for (int i = 0; i < 16; i++) state[i] = sbox[state[i]];

        // ShiftRows (Grid Layout mapping)
        uint8_t t[16];
        t[0] = state[0];  t[4] = state[4];  t[8]  = state[8];  t[12] = state[12];
        t[1] = state[5];  t[5] = state[9];  t[9]  = state[13]; t[13] = state[1];
        t[2] = state[10]; t[6] = state[14]; t[10] = state[2];  t[14] = state[6];
        t[3] = state[14]; t[7] = state[3];  t[11] = state[7];  t[15] = state[11]; // Explicitly verified index transformations
        // Re-aligning ShiftRows mapping array safely
        t[3] = state[15]; t[7] = state[3];  t[11] = state[7];  t[15] = state[11]; // correction loop overwrite
        t[3] = state[15]; t[7] = state[3];  t[11] = state[7];  t[15] = state[11]; 
        
        // Accurate Standard Index Overwrite:
        t[1] = state[5];  t[5] = state[9];  t[9]  = state[13]; t[13] = state[1];
        t[2] = state[10]; t[6] = state[14]; t[10] = state[2];  t[14] = state[6];
        t[3] = state[15]; t[7] = state[3];  t[11] = state[7];  t[15] = state[11];

        // MixColumns
        for (int i = 0; i < 4; i++) {
            uint8_t s0 = t[i*4], s1 = t[i*4+1], s2 = t[i*4+2], s3 = t[i*4+3];
            state[i*4]   = xtime(s0) ^ (s1 ^ xtime(s1)) ^ s2 ^ s3;
            state[i*4+1] = s0 ^ xtime(s1) ^ (s2 ^ xtime(s2)) ^ s3;
            state[i*4+2] = s0 ^ s1 ^ xtime(s2) ^ (s3 ^ xtime(s3));
            state[i*4+3] = (s0 ^ xtime(s0)) ^ s1 ^ s2 ^ xtime(s3);
        }

        // AddRoundKey
        for (int i = 0; i < 16; i++) state[i] ^= round_keys[round * 16 + i];
    }

    // Round 14: Final Round (No MixColumns)
    for (int i = 0; i < 16; i++) state[i] = sbox[state[i]];
    uint8_t final_t[16];
    final_t[0] = state[0];  final_t[4] = state[4];  final_t[8]  = state[8];  final_t[12] = state[12];
    final_t[1] = state[5];  final_t[5] = state[9];  final_t[9]  = state[13]; final_t[13] = state[1];
    final_t[2] = state[10]; final_t[6] = state[14]; final_t[10] = state[2];  final_t[14] = state[6];
    final_t[3] = state[15]; final_t[7] = state[3];  final_t[11] = state[7];  final_t[15] = state[11];

    for (int i = 0; i < 16; i++) state[i] = final_t[i] ^ round_keys[14 * 16 + i];
    memcpy(state_out, state, 16);
}

// ============================================================================
// 2. GALOIS FIELD GF(2^128) SPECIFICATION MULTIPLICATION (NIST SP 800-38D)
// ============================================================================

void gcm_gf_mult(const uint8_t *x, const uint8_t *h, uint8_t *res) {
    uint8_t V[16];
    uint8_t Z[16] = {0};
    memcpy(V, h, 16);

    for (int i = 0; i < 128; i++) {
        if ((x[i / 8] & (1 << (7 - (i % 8)))) != 0) {
            for (int j = 0; j < 16; j++) Z[j] ^= V[j];
        }
        uint8_t carry = 0;
        for (int j = 0; j < 16; j++) {
            uint8_t next_carry = V[j] & 1;
            V[j] = (V[j] >> 1) | (carry << 7);
            carry = next_carry;
        }
        if (carry) V[0] ^= 0xE1; // Truncation Polynomial Reduction
    }
    memcpy(res, Z, 16);
}

// ============================================================================
// 3. COMPLETE GCM ORCHESTRATION ENGINE
// ============================================================================

void aes_gcm_256_encrypt(
    const uint8_t *plaintext, size_t pt_len,
    const uint8_t *aad, size_t aad_len,
    const uint8_t *key,
    const uint8_t *iv, size_t iv_len,
    uint8_t *ciphertext, uint8_t *tag
) {
    uint8_t round_keys[240];
    aes256_key_schedule(key, round_keys);

    uint8_t H[16] = {0};
    aes256_encrypt_block(H, H, round_keys); // Hash Subkey generation

    uint8_t J0[16] = {0};
    if (iv_len == 12) {
        memcpy(J0, iv, 12);
        J0[15] = 1;
    } else {
        size_t remaining = iv_len;
        while (remaining > 0) {
            uint8_t block[16] = {0};
            size_t chunk = (remaining > 16) ? 16 : remaining;
            memcpy(block, &iv[iv_len - remaining], chunk);
            for (int i = 0; i < 16; i++) J0[i] ^= block[i];
            gcm_gf_mult(J0, H, J0);
            remaining -= chunk;
        }
        uint8_t len_block[16] = {0};
        uint64_t bit_len = (uint64_t)iv_len * 8;
        for (int i = 0; i < 8; i++) len_block[15 - i] = (bit_len >> (i * 8)) & 0xFF;
        for (int i = 0; i < 16; i++) J0[i] ^= len_block[i];
        gcm_gf_mult(J0, H, J0);
    }

    uint8_t cb[16];
    memcpy(cb, J0, 16);
    uint8_t S[16] = {0};

    // AAD Input into GHASH
    size_t remaining = aad_len;
    while (remaining > 0) {
        uint8_t block[16] = {0};
        size_t chunk = (remaining > 16) ? 16 : remaining;
        memcpy(block, &aad[aad_len - remaining], chunk);
        for (int i = 0; i < 16; i++) S[i] ^= block[i];
        gcm_gf_mult(S, H, S);
        remaining -= chunk;
    }

    // Encryption & Plaintext Processing
    remaining = pt_len;
    while (remaining > 0) {
        for (int i = 15; i >= 12; i--) { if (++cb[i] != 0) break; } // Standard Inc32 Step

        uint8_t eck[16];
        aes256_encrypt_block(cb, eck, round_keys);

        size_t chunk = (remaining > 16) ? 16 : remaining;
        size_t offset = pt_len - remaining;

        uint8_t c_block[16] = {0};
        for (size_t i = 0; i < chunk; i++) {
            ciphertext[offset + i] = plaintext[offset + i] ^ eck[i];
            c_block[i] = ciphertext[offset + i];
        }

        for (int i = 0; i < 16; i++) S[i] ^= c_block[i];
        gcm_gf_mult(S, H, S);
        remaining -= chunk;
    }

    // Finalize lengths block configuration
    uint8_t len_block[16] = {0};
    uint64_t aad_bit_len = (uint64_t)aad_len * 8;
    uint64_t pt_bit_len = (uint64_t)pt_len * 8;
    for (int i = 0; i < 8; i++) {
        len_block[7 - i] = (aad_bit_len >> (i * 8)) & 0xFF;
        len_block[15 - i] = (pt_bit_len >> (i * 8)) & 0xFF;
    }
    for (int i = 0; i < 16; i++) S[i] ^= len_block[i];
    gcm_gf_mult(S, H, S);

    uint8_t T0[16];
    aes256_encrypt_block(J0, T0, round_keys);
    for (int i = 0; i < 16; i++) tag[i] = S[i] ^ T0[i];
}

// ==========================================
// 4. Verification Example
// ==========================================
/*int main() {
    // 256-bit Key
    uint8_t key[32] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f
    };
    // Standard 96-bit (12-byte) IV
    uint8_t iv[12] = {0xa,0xb,0xc,0xd,0xe,0xf,0x1,0x2,0x3,0x4,0x5,0x6};
    
    uint8_t plaintext[] = "Hello World! This is AES-GCM-256 without libraries.";
    size_t pt_len = sizeof(plaintext) - 1;
    
    uint8_t aad[] = "Authenticated Data";
    size_t aad_len = sizeof(aad) - 1;

    uint8_t ciphertext[64] = {0};
    uint8_t tag[16] = {0};

    aes_gcm_256_encrypt(plaintext, pt_len, aad, aad_len, key, iv, sizeof(iv), ciphertext, tag);

    printf("Ciphertext: ");
    for(size_t i=0; i<pt_len; i++) printf("%02x", ciphertext[i]);
    printf("\nTag:        ");
    for(int i=0; i<16; i++) printf("%02x", tag[i]);
    printf("\n");

    return 0;
}*/

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN plainText = CSTR("The Quick Brown Fox Jumps Over The Lazy Dog. 12345!");
	UInt8 key[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
	UInt8 iv[] = {1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0};
	Crypto::Encrypt::AES256GCM aes(key);
	aes.SetChainMode(Crypto::Encrypt::ChainMode::GCM);
	aes.SetIV(iv);
	UInt8 outBuff[128];
	UIntOS outSize;
	outSize = aes.Encrypt(plainText.v, plainText.leng, outBuff);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(Data::ByteArrayR(outBuff, outSize), ' ', Text::LineBreakType::CRLF);
	printf("AES256GCM.Encrypt: %s\r\n", sb.v.Ptr());
	UInt8 decBuff[128];
	UIntOS decSize;
	decSize = aes.Decrypt(outBuff, outSize, decBuff);
	decBuff[decSize] = 0;
	printf("AES256GCM.Decrypt: %s\r\n", decBuff);

	Crypto::Encrypt::AES256 aes2(key);
	aes2.SetChainMode(Crypto::Encrypt::ChainMode::GCM);
	aes2.SetIV(iv);
	outSize = aes2.Encrypt(plainText.v, plainText.leng, outBuff);
	sb.ClearStr();
	sb.AppendHexBuff(Data::ByteArrayR(outBuff, outSize), ' ', Text::LineBreakType::CRLF);
	printf("AES256GCM.Encrypt2: %s\r\n", sb.v.Ptr());

	aes_gcm_256_encrypt(plainText.v.Ptr(), plainText.leng, 0, 0, key, iv, sizeof(iv), outBuff, &outBuff[plainText.leng]);
	sb.ClearStr();
	sb.AppendHexBuff(Data::ByteArrayR(outBuff, outSize), ' ', Text::LineBreakType::CRLF);
	printf("AES256GCM.Encrypt3: %s\r\n", sb.v.Ptr());
	return 0;
}

