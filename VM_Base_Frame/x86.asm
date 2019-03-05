	.686p
	.model flat, stdcall
	option casemap:none
	
	.Data
dwEip	dword	0h
dwEsp	dword	0h
dwTemp  dword	0h
;===========================================================================	
	.Code
	;16λ�ι�ģ������£���ȱʡ������������WORD����32λ�ι�ģ������£���ȱʡ������������DWORD��


	; ��C�����У������ķ���ֵ����ڼĴ���EAX��EDX�У�EAX��ŵ�32λ��EDX��Ÿ�32λ���������ֵ����32λ����ֻ�õ�EAX��
 
_readmsr  PROC dwExc:dword
		mov ecx, dwExc
		rdmsr
		ret
_readmsr ENDP


_writemsr	Proc	Index:dword,LowPart,HighPart
	mov	ecx, Index
	mov	eax, LowPart
	mov	edx, HighPart
	wrmsr
	ret
_writemsr 	Endp


_setcr0 Proc  dwCr0:dword

		mov eax, dwCr0
		mov cr0, eax
		ret
_setcr0 	Endp


_getcr0 Proc
	mov eax, cr0
	ret
_getcr0 Endp

_setcr3 Proc  dwCr3:dword

		mov eax, dwCr3
		mov cr3, eax
		ret
_setcr3 	Endp


_getcr3 Proc
	mov eax, cr3
	ret
_getcr3 Endp

_setcr4 Proc  dwCr4:dword

		mov eax, dwCr4
		mov cr4, eax
		ret
_setcr4 	Endp


_getcr4 Proc
	mov eax, cr4
	ret
_getcr4 Endp


_setdr7 Proc  dwDr7:dword
		mov eax, dwDr7
		mov dr7, eax
		ret
_setdr7 	Endp


_getdr7 Proc
	mov eax, dr7
	ret
_getdr7 Endp


_vmxon Proc  dwLow:dword,dwHigh:dword
push dwHigh
push dwLow
vmxon qword ptr [esp]
add	esp,8
pushfd
pop eax
ret
_vmxon Endp


_vmclear Proc  dwLow:dword,dwHigh:dword
push dwHigh
push dwLow
vmclear qword ptr [esp]
add	esp,8
ret
_vmclear Endp


_vmptrld Proc  dwLow:dword,dwHigh:dword
push dwHigh
push dwLow
vmptrld qword ptr [esp]
add	esp,8
pushfd
pop eax
ret
_vmptrld Endp

_vmwrite Proc  dwEncode:dword,dwValue:dword
mov eax,dwEncode
vmwrite eax,dwValue
ret
_vmwrite Endp

_vmread Proc  dwEncode:dword
mov ebx,dwEncode
vmread eax,ebx
ret
_vmread Endp

_getGDTR Proc  pgdt:dword
mov eax,[pgdt]
sgdt [eax]
ret
_getGDTR Endp

_getIDTR Proc  pidt:dword
mov eax,[pidt]
sidt [eax]
ret
_getIDTR Endp

_getCS Proc
Xor eax,eax
mov ax,CS
ret
_getCS Endp

_getSS Proc
Xor eax,eax
mov ax,SS
ret
_getSS Endp

_getDS Proc
Xor eax,eax
mov ax,DS
ret
_getDS Endp

_getES Proc
Xor eax,eax
mov ax,ES
ret
_getES Endp

_getFS Proc
Xor eax,eax
mov ax,FS
ret
_getFS Endp

_getGS Proc
Xor eax,eax
mov ax,GS
ret
_getGS Endp

_getLDTR	Proc
	xor	eax, eax
	sldt	ax
	ret
_getLDTR	Endp

_getTR	Proc
	xor	eax, eax
	str	ax
	ret
_getTR	Endp




_init_vmcs Proc pvcpu:dword,InitVmcs:dword
push eax
push ebx
push ecx
push edx
push esi
push edi
push ebp
pushfd
push esp
pushfd
push pvcpu
call	InitVmcs

popfd
pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx
pop eax

ret

_init_vmcs Endp

; guest ����
_guest_ret Proc
popfd
pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx
pop eax
leave
ret 8
_guest_ret Endp




; exit ���
extern uFuncExitHandle:dword
_exit_handle Proc
CLI
push esp
push eax
push ebx
push ecx
push edx
push esi
push edi
push ebp
pushfd


push esp	; ���滷����Ӱ�ӼĴ�������ʵ�����ǲ����ڵģ�������ֱ�Ӳ���ջ�ڴ� �䵱Ӱ�ӼĴ���
call uFuncExitHandle




STI			; �ָ�Ӱ�ӼĴ������� Guest �Ĵ���
popfd
pop ebp
pop edi

pop esi
pop edx
pop ecx
pop ebx
pop eax
pop esp
vmresume
; ���ʧ���� �������Լ� ���� uFuncExitHandle �м�������Ϣ
; ͬʱ���� KdBugCheckDebug,���ڵ���ģʽ����Ч
call _exit_handle
_exit_handle Endp


_vmlaunch Proc
vmlaunch
ret
_vmlaunch Endp


;cpuidʹ��eax��Ϊ���������eax��ebx��ecx��edx��Ϊ�������
_getcpuid Proc uEncodeEax:dword,uEax:dword,uEbx:dword,uEcx:dword,uEdx:dword

; ������Ҫ�� ebx Ҳ������
push ebx
push esi
mov eax,[uEncodeEax]
cpuid
mov esi,uEax
mov [esi],eax
mov esi,uEbx
mov [esi],ebx
mov esi,uEcx
mov [esi],ecx
mov esi,uEdx
mov [esi],edx
pop esi
pop ebx

ret
_getcpuid Endp



_vmxstop Proc
push eax
push ecx
mov eax,41414141h
mov ecx,42424242h
cpuid
pop ecx
pop eax
ret
_vmxstop Endp



_vmxoff Proc
vmxoff
ret
_vmxoff Endp

; �������������
_vmxoff_no_root Proc pEsp:dword,pEip:dword,pGuestEsp:dword

vmxoff

mov eax,pEip
mov dwEip,eax
mov eax,pGuestEsp
mov dwEsp,eax

mov esp,pEsp
STI			; �ָ�Ӱ�ӼĴ������� Guest �Ĵ���
popfd
pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx
pop eax
pop esp
mov esp , dwEsp
jmp dwEip

ret
_vmxoff_no_root Endp


End	