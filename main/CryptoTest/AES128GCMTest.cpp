#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Encrypt/AES128GCM.h"
#include "IO/FileStream.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define AES_BLOCK_SIZE 16

// --- 1. MINIMAL AES-128 ENCRYPTION CORE ---
// S-box look-up table
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

// Key expansion schedule for AES-128
void aes128_key_expansion(const uint8_t *key, uint8_t *round_keys) {
    uint8_t rcon = 0x01;
    memcpy(round_keys, key, 16);
    for (int i = 16; i < 176; i += 4) {
        uint8_t temp[4];
        memcpy(temp, &round_keys[i - 4], 4);
        if (i % 16 == 0) {
            uint8_t k = temp[0];
            temp[0] = sbox[temp[1]] ^ rcon;
            temp[1] = sbox[temp[2]];
            temp[2] = sbox[temp[3]];
            temp[3] = sbox[k];
            rcon = (rcon << 1) ^ (rcon & 0x80 ? 0x1B : 0x00);
        }
        for (int j = 0; j < 4; j++) {
            round_keys[i + j] = round_keys[i - 16 + j] ^ temp[j];
        }
    }
}

// Single-block AES-128 encryption
void aes128_encrypt_block(const uint8_t *state_in, const uint8_t *round_keys, uint8_t *state_out) {
    uint8_t s[16];
    memcpy(s, state_in, 16);
    for (int i = 0; i < 16; i++) s[i] ^= round_keys[i];

    for (int round = 1; round <= 9; round++) {
        for (int i = 0; i < 16; i++) s[i] = sbox[s[i]];
        uint8_t t[16];
        t[0]=s[0]; t[4]=s[4]; t[8]=s[8];  t[12]=s[12];
        t[1]=s[5]; t[5]=s[9]; t[9]=s[13]; t[13]=s[1];
        t[2]=s[10];t[6]=s[14];t[10]=s[2]; t[14]=s[6];
        t[3]=s[15];t[7]=s[3]; t[11]=s[7]; t[15]=s[11];
        for (int i = 0; i < 4; i++) {
            int idx = i * 4;
            uint8_t a = t[idx], b = t[idx+1], c = t[idx+2], d = t[idx+3];
            #define g(x) ((x << 1) ^ ((x & 0x80) ? 0x1B : 0))
            s[idx]   = g(a) ^ (g(b)^b) ^ c ^ d;
            s[idx+1] = a ^ g(b) ^ (g(c)^c) ^ d;
            s[idx+2] = a ^ b ^ g(c) ^ (g(d)^d);
            s[idx+3] = (g(a)^a) ^ b ^ c ^ g(d);
        }
        for (int i = 0; i < 16; i++) s[i] ^= round_keys[round * 16 + i];
    }
    for (int i = 0; i < 16; i++) s[i] = sbox[s[i]];
    uint8_t t[16];
    t[0]=s[0]; t[4]=s[4]; t[8]=s[8];  t[12]=s[12];
    t[1]=s[5]; t[5]=s[9]; t[9]=s[13]; t[13]=s[1];
    t[2]=s[10];t[6]=s[14];t[10]=s[2]; t[14]=s[6];
    t[3]=s[15];t[7]=s[3]; t[11]=s[7]; t[15]=s[11];
    for (int i = 0; i < 16; i++) state_out[i] = t[i] ^ round_keys[160 + i];
}

// --- 2. GCM ARITHMETIC CORE (GF(2^128)) ---
// Carryless bit-multiplication for GCM over GF(2^128)
void gf_mult(const uint8_t *x, const uint8_t *y, uint8_t *z) {
    uint8_t V[16];
    memset(z, 0, 16);
    memcpy(V, y, 16);
    for (int i = 0; i < 128; i++) {
        if (x[i / 8] & (0x80 >> (i % 8))) {
            for (int j = 0; j < 16; j++) z[j] ^= V[j];
        }
        uint8_t carry = V[15] & 0x01;
        for (int j = 15; j > 0; j--) {
            V[j] = (V[j] >> 1) | (V[j - 1] << 7);
        }
        V[0] >>= 1;
        if (carry) V[0] ^= 0xE1; // GCM polynomial reduction
    }
}

// Incorporates a 16-byte buffer chunk into the running GHASH total
void ghash_update(uint8_t *current_hash, const uint8_t *h_key, const uint8_t *buffer, size_t len) {
    size_t blocks = len / 16;
    for (size_t b = 0; b < blocks; b++) {
        for (size_t i = 0; i < 16; i++) {
            current_hash[i] ^= buffer[b * 16 + i];
        }
        uint8_t next_hash[16];
        gf_mult(current_hash, h_key, next_hash);
        memcpy(current_hash, next_hash, 16);
    }
}

// Helper to handle partial trailing blocks by padding them out with zeros
void ghash_update_padded(uint8_t *current_hash, const uint8_t *h_key, const uint8_t *data, size_t len) {
    size_t full_blocks = len / 16;
    ghash_update(current_hash, h_key, data, full_blocks * 16);
    
    size_t remainder = len % 16;
    if (remainder > 0) {
        uint8_t pad[16] = {0};
        memcpy(pad, data + (full_blocks * 16), remainder);
        ghash_update(current_hash, h_key, pad, 16);
    }
}

// --- 3. CORE AES-GCM ENCRYPTION WRAPPER ---
void aes_gcm_128_encrypt(const uint8_t *key, const uint8_t *iv,
                         const uint8_t *aad, size_t aad_len,
                         const uint8_t *plaintext, size_t pt_len,
                         uint8_t *ciphertext, uint8_t *tag) {
    uint8_t round_keys[176];
    aes128_key_expansion(key, round_keys);

    // Step A: Generate the Hash Subkey H by encrypting an all-zero block
    uint8_t h_key[16] = {0};
    aes128_encrypt_block(h_key, round_keys, h_key);

    // Step B: Set up Counter 0 (J0) from the 96-bit (12-byte) IV
    uint8_t j0[16] = {0};
    memcpy(j0, iv, 12);
    j0[15] = 1; // Base counter initialized to 1

    // Step C: Generate Counter 1 for encrypting the payload blocks
    uint8_t counter[16];
    memcpy(counter, j0, 16);

    // Step D: CTR payload encryption stream
    size_t bytes_processed = 0;
    while (bytes_processed < pt_len) {
        // Increment lower 32-bits of counter
        for (int i = 15; i >= 12; i--) {
            if (++counter[i] != 0) break;
        }
        
        uint8_t keystream[16];
        aes128_encrypt_block(counter, round_keys, keystream);
        
        size_t chunk = (pt_len - bytes_processed < 16) ? (pt_len - bytes_processed) : 16;
        for (size_t i = 0; i < chunk; i++) {
            ciphertext[bytes_processed + i] = plaintext[bytes_processed + i] ^ keystream[i];
        }
        bytes_processed += chunk;
    }

    // Step E: GHASH processing pipeline (Binds AAD and Ciphertext together)
    uint8_t ghash_total[16] = {0};
    
    // 1. Process Auth Data (AAD) first
    ghash_update_padded(ghash_total, h_key, aad, aad_len);
    
    // 2. Process Ciphertext next
    ghash_update_padded(ghash_total, h_key, ciphertext, pt_len);
    
    // 3. Append explicit bit-length block [Len(AAD)_64-bit || Len(Ciphertext)_64-bit]
    uint8_t length_block[16];
    uint64_t aad_bits = (uint64_t)aad_len * 8;
    uint64_t ct_bits = (uint64_t)pt_len * 8;
    
    // Move to big-endian block layout
    for (int i = 0; i < 8; i++) {
        length_block[i]     = (aad_bits >> (56 - i * 8)) & 0xFF;
        length_block[8 + i] = (ct_bits  >> (56 - i * 8)) & 0xFF;
    }
    ghash_update(ghash_total, h_key, length_block, 16);

    // Step F: Encrypt Counter 0 and XOR with GHASH total to yield final tag
    uint8_t encrypted_j0[16];
    aes128_encrypt_block(j0, round_keys, encrypted_j0);
    
    for (int i = 0; i < 16; i++) {
        tag[i] = ghash_total[i] ^ encrypted_j0[i];
    }
}

// --- 4. EXECUTION FLOW TESTING ---
/*int main() {
    uint8_t key[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    uint8_t iv[12]  = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB};
    
    const uint8_t aad[]       = "AuthDataHeader123"; // Unencrypted authenticated metadata
    const uint8_t plaintext[] = "Hello World! This is a pure C AES-GCM string.";
    
    size_t aad_len = strlen((char*)aad);
    size_t pt_len  = strlen((char*)plaintext);
    
    uint8_t ciphertext[64] = {0};
    uint8_t tag[16]        = {0};

    aes_gcm_128_encrypt(key, iv, aad, aad_len, plaintext, pt_len, ciphertext, tag);

    printf("--- PURE C AES-GCM OUTPUT ---\n");
    printf("Ciphertext (Hex): ");
    for(size_t i=0; i<pt_len; i++) printf("%02X", ciphertext[i]);
    
    printf("\nAuth Tag (Hex):   ");
    for(int i=0; i<16; i++) printf("%02X", tag[i]);
    printf("\n");

    return 0;
}*/


Int32 MyMain2(NN<Core::ProgControl> progCtrl)
{
	UInt8 key[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	UInt8 iv[] = {1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0};
	Crypto::Encrypt::AES128GCM aes(key);
	aes.SetIV(iv);
	UnsafeArray<UInt8> inBuff;
	UIntOS sourceSize;
	UIntOS inSize;
	UnsafeArray<UInt8> outBuff;
	UIntOS outSize;

	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sourceSize = (UIntOS)fs.GetLength();
		if (sourceSize == 0)
		{
			return 0;
		}
		inBuff = MemAllocArr(UInt8, sourceSize + 32);
		inSize = fs.Read(Data::ByteArray(inBuff, sourceSize));
	}
	outBuff = MemAllocArr(UInt8, inSize + 16);
	outSize = aes.Encrypt(inBuff, inSize, outBuff);
	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js.aes128gcm.enc"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.WriteCont(outBuff, outSize);
	}

	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js.aes128gcm.enc"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		inSize = fs.Read(Data::ByteArray(inBuff, outSize));
	}
	outSize = aes.Decrypt(inBuff, inSize, outBuff);
	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js.aes128gcm"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.WriteCont(outBuff, outSize);
	}
	MemFreeArr(inBuff);
	MemFreeArr(outBuff);
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN plainText = CSTR("The Quick Brown Fox Jumps Over The Lazy Dog. 12345!");
	UInt8 key[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	UInt8 iv[] = {1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0};
	Crypto::Encrypt::AES128 aes(key);
	aes.SetChainMode(Crypto::Encrypt::ChainMode::GCM);
	aes.SetIV(iv);
	UInt8 outBuff[128];
	UIntOS outSize;
	outSize = aes.Encrypt(plainText.v, plainText.leng, outBuff);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(Data::ByteArrayR(outBuff, outSize), ' ', Text::LineBreakType::CRLF);
	printf("AES128.Encrypt: %s\r\n", sb.v.Ptr());
	UInt8 decBuff[128];
	UIntOS decSize;
	decSize = aes.Decrypt(outBuff, outSize, decBuff);
	decBuff[decSize] = 0;
	printf("AES128.Decrypt: %s\r\n", decBuff);

	aes_gcm_128_encrypt(key, iv, iv, 0, plainText.v.Ptr(), plainText.leng, outBuff, &outBuff[plainText.leng]);
	sb.ClearStr();
	sb.AppendHexBuff(Data::ByteArrayR(outBuff, outSize), ' ', Text::LineBreakType::CRLF);
	printf("AES128.Encrypt2: %s\r\n", sb.v.Ptr());
	return 0;
}





