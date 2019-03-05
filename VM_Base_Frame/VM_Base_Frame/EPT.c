#include "EPT.h"
#include "stdafx.h"


ULONG GetMaxPhyaddr()
{
	REGISTERS  regs = { 0 };
	ULONG uEax = 0;
	_getcpuid(0x80000008, &regs.uEAX, &regs.uEBX, &regs.uECX, &regs.uEDX);
	uEax = regs.uEAX;
	// ȡ Eax �� 7:0 ��Ϊ MaxPhyaddr
	uEax &= 0xF;
	return uEax;
}


// 416 �·�����Ƿ�֧��4��ҳ����� IA32_VMX_EPT_VPID_CAP �� bit 6 Ϊ1ʱ Ϊ 4��ҳ�� ÿ�� EPT ��СΪ 4K,,ͬʱ��� bit 16 bit 17 �Ƿ�Ϊ1 ��ΪЧ��  bit 8 �� bit 14 ��ѯ EPTP ֧�ֵ�����
// EPTP �ṹ�� bit 5:3 ��������Ϊ3 ��ʾ��Ҫ����4��ҳ��� walk

// bit 6 �� dirty ��ҳ�� �� Ȩ��λ��Ϊ1ʱ����������ʱ  EPT ҳ����� bit 8 bit 9 ��Ϊ��־λ

/*
PML4T(Page Map Level4 Table)�����ڵ�PML4E�ṹ��ÿ����Ϊ4K���ں�512�� PML4E �ṹ��ÿ��8�ֽ�

PDPT (Page Directory Pointer Table)�����ڵ�PDPTE �ṹ��ÿ���� 4K���ں�512�� PDPTE �ṹ��ÿ��8�ֽ�

PDT (Page Directory Table)  �����ڵ�PDE�ṹ��ÿ����4K���ں�512 �� PDE �ṹ��ÿ�� 8 �ֽ�

PT(Page Table)�����ڶ� PTE �ṹ��ÿ����4K���ں�512�� PTE �ṹ��ÿ��8�ֽ�
*/

/*
static_assert(sizeof(EptPointer) == 8, "Size check");


*/

ULONG GetPagingSizeMode()
{
	// ���� IA32_VMX_EPT_VPID_CAP ���鿴 ҳ���С֧��

	MSR_REGISTER msrEPTVPIDCAP = { 0 };
	msrEPTVPIDCAP = _readmsr(IA32_VMX_EPT_VPID_CAP);

	// �� bit 6 == 1 ֧�� 4��ҳ��
	// bit 16 == 1 ֧�� 2M ҳ��
	// bit 17 == 1 ֧�� 1G ҳ��

	// ���ȼ��� ��������
	if (GetBit(msrEPTVPIDCAP.uLow,6) == 1)
	{
		return s4KMode;
	}

	if (GetBit(msrEPTVPIDCAP.uLow, 16) == 1)
	{
		return s2MMode;
	}

	if (GetBit(msrEPTVPIDCAP.uLow, 17) == 1)
	{
		return s1GMode;
	}
	return -1;
}




ULONG GetPagingModeType()
{
	// ��� EPT ����

	// CPUID.01H:ECX[5] = 1
	// IA32_VMX_PROCBASED_CTLS[63] = 1
	// IA32_VMX_PROCBASED_CTLS2[33] = 1
	
	// ECX[5] ��ʼ�ͼ���� ���ﲻ�ظ����

	KdBreakPoint();

	MSR_REGISTER msrPROCBASEDCTLS = { 0 };
	msrPROCBASEDCTLS = _readmsr(IA32_VMX_PROCBASED_CTLS);

	MSR_REGISTER msrPROCBASEDCTLS2 = { 0 };
	msrPROCBASEDCTLS2 = _readmsr(IA32_VMX_PROCBASED_CTLS2);

	if (GetBit(msrPROCBASEDCTLS.uHigh, 63 - 32) != 1 || GetBit(msrPROCBASEDCTLS2.uHigh, 33 - 32) != 1)
	{
		KdPrint(("-[VM-Base-Frame]- : IA32_VMX_PROCBASED_CTLS OR  IA32_VMX_PROCBASED_CTLS2 Is't Support EPT!\n"));
		return NonSupport;
	}


	// ��ȡѰַģʽ

	// ����Ԥ�� 3 ��ģʽ


	ULONG uCr0 = _getcr0();

	// ��� PG NE PE
	if (GetBit(uCr0, 31) == 0)
	{
		KdPrint(("-[VM-Base-Frame]- : PG Is't Support EPT!\n"));
		return NonSupport;
	}

	// 32 λ��ҳ���ﲻ���� �Ǹ�Ӧ����ʵģʽ


	// CR4.PAE[bit 5].
	ULONG uCr4 = _getcr4();

	if (GetBit(uCr0, 5) == 0)
	{
		KdPrint(("-[VM-Base-Frame]- : Cr4.PAE Is't Support EPT!\n"));
		return NonSupport;
	}


	MSR_REGISTER msrIA32EFER = { 0 };
	msrIA32EFER = _readmsr(IA32_EFER_MSR);

	// 	IA32_EFER.LMA[bit 10] == 0
	if (GetBit(msrIA32EFER.uLow, 10))
	{
		KdPrint(("-[VM-Base-Frame]- : Use IA32e Mode!\n"));
		return IA32eMode;
	}
	else
	{
		KdPrint(("-[VM-Base-Frame]- : Use PAEMode!\n"));
		return PAEMode;
	}
	return NonSupport;
}