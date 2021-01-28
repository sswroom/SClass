section .text

global _MemFillB
global MemFillB
global _MemFillW
global MemFillW
global _MemClearAC
global MemClearAC
global _MemClearANC
global MemClearANC

;void MemFillB(UInt8 *buff, OSInt byteCnt, UInt8 val)
;0 retAddr
;rcx buff
;rdx byteCnt
;r8 val
	align 16
_MemFillB:
MemFillB:
	push edi
	mov edi,dword [esp+4]
	mov eax,dword [esp+12]
	mov ecx,dword [esp+8]
	rep stosb
	pop edi
	ret

;void MemFillW(UInt8 *buff, OSInt wordCnt, UInt16 val);
;0 retAddr
;rcx buff
;rdx wordCnt
;r8 val
	align 16
_MemFillW:
MemFillW:
	push edi
	mov edi,dword [esp+4]
	mov eax,dword [esp+12]
	mov ecx,dword [esp+8]
	rep stosw
	pop edi
	ret

;void MemClearAC(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;4 buff
;8 buffSize
	align 16
_MemClearAC:
MemClearAC:
	mov ecx,dword [esp+4]
	mov edx,dword [esp+8]
	shr edx,4
	pxor xmm0,xmm0
	align 16
memclearaclop:
	movaps [ecx],xmm0
	lea ecx,[ecx+16]
	dec edx
	jnz memclearaclop
	ret

;void MemClearANC(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;4 buff
;8 buffSize
	align 16
_MemClearANC:
MemClearANC:
	mov ecx,dword [esp+4]
	mov edx,dword [esp+8]
	shr edx,4
	pxor xmm0,xmm0
	align 16
memclearanclop:
	movntps [ecx],xmm0
	lea ecx,[ecx+16]
	dec edx
	jnz memclearanclop
	ret
