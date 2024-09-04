export class ICrypto
{
	/**
	 * @param {ArrayBuffer} inBuff
	 * @returns {Promise<ArrayBuffer|null>}
	 */
	async encrypt(inBuff)
	{
		throw new Error("ICrypto.encrypt must be inherited");
	}

	/**
	 * @param {ArrayBuffer} inBuff
	 * @returns {Promise<ArrayBuffer|null>}
	 */
	async decrypt(inBuff)
	{
		throw new Error("ICrypto.decrypt must be inherited");
	}
	
	/**
	 * @returns {number}
	 */
	getEncBlockSize()	/**
	* @returns {number}
	*/

	{
		throw new Error("ICrypto.getEncBlockSize must be inherited");
	}

	/**
	 * @returns {number}
	 */
	getDecBlockSize()
	{
		throw new Error("ICrypto.getDecBlockSize must be inherited");
	}

	
	/**
	 * @param {string} key
	 * @param {number} nBytes
	 * @returns {Uint8Array}
	 */
	static keyToBuffer(key, nBytes)
	{
		let enc = new TextEncoder();
		let arr = enc.encode(key);
		if (arr.length >= nBytes)
		{
			return arr.subarray(0, nBytes);
		}
		else
		{
			let ret = new Uint8Array(nBytes);
			let i = 0;
			let j = arr.length;
			let k = 0;
			while (i < nBytes)
			{
				if (k >= j)
				{
					ret[i] = 0x7f;
					k = 0;
				}
				else
				{
					ret[i] = arr[k];
					k++;
				}
				i++;
			}
			return ret;
		}
	}
}

export class AES256GCM extends ICrypto
{
	/**
	 * @param {ArrayBuffer} key
	 * @param {ArrayBuffer} iv
	 */
	constructor(key, iv)
	{
		super();
		if (key.byteLength != 32)
		{
			throw new Error("key must be 32 bytes long");
		}
		this.key = key;
		this.setIV(iv);
	}

	/**
	 * @param {ArrayBuffer} inBuff
	 */
	async encrypt(inBuff)
	{
		if (window.crypto.subtle == null)
		{
			return null;
		}
		let key = await window.crypto.subtle.importKey("raw", this.key, {name: "AES-GCM"}, false, ["encrypt", "decrypt"]);
		return await window.crypto.subtle.encrypt( {
			name: "AES-GCM",
			iv: this.iv
		  }, key, inBuff);
	}

	/**
	 * @param {ArrayBuffer} inBuff
	 */
	async decrypt(inBuff)
	{
		if (window.crypto.subtle == null)
		{
			return null;
		}
		let key = await window.crypto.subtle.importKey("raw", this.key, {name: "AES-GCM"}, false, ["encrypt", "decrypt"]);
		return await window.crypto.subtle.decrypt( {
			name: "AES-GCM",
			iv: this.iv
		  }, key, inBuff);
	}

	getEncBlockSize()
	{
		return 16;
	}

	getDecBlockSize()
	{
		return 16;
	}

	/**
	 * @param {ArrayBuffer} iv
	 */
	setIV(iv)
	{
		if (iv.byteLength != 12)
		{
			throw new Error("iv must be 12 bytes long");
		}
		this.iv = iv;
	}
}