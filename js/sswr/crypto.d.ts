export abstract class ICrypto
{
	abstract encrypt(inBuff: ArrayBuffer): Promise<ArrayBuffer | null>;
	abstract decrypt(inBuff: ArrayBuffer): Promise<ArrayBuffer | null>;
	
	abstract getEncBlockSize(): number;
	abstract getDecBlockSize(): number;

	static keyToBuffer(key: string, nBytes: number): ArrayBuffer;
}

export class AES256GCM extends ICrypto
{
	constructor(key: ArrayBuffer, iv: ArrayBuffer);

	encrypt(inBuff: ArrayBuffer): Promise<ArrayBuffer | null>;
	decrypt(inBuff: ArrayBuffer): Promise<ArrayBuffer | null>;

	getEncBlockSize(): number;
	getDecBlockSize(): number;

	/**
	 * 12 byte length
	 * @param iv Initial Vector
	 */
	setIV(iv: ArrayBuffer): void;
}