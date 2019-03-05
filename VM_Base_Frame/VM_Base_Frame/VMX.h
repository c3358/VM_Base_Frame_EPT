#pragma once
#include <ntddk.h>

#pragma disable(warning:4214) 

#define VMX_PAGE_SIZE		(0x1000)
#define VMX_PAGE_SIZE_MORE	(0x1000 * 4)
#define VMX_HOST_STACK_SIZE (0x3F00)

/*
VMX_FLAG_0			δ����
VMX_FLAG_1			VMX_ON		�׶����
VMX_FLAG_2			VMX_Launch  �׶����  �״� Vm-Entry ���
*/
#define VMX_STEP_0		(0)
#define VMX_STEP_1		(1)
#define VMX_STEP_2		(2)


#define VMX_HOST_SELECTOR_MASK		(0xFFF8)


#pragma pack(push)  //�������״̬
#pragma pack(1)		//�趨Ϊ4�ֽڶ���
typedef struct _VCPU_STC
{
	// ��¼��ǰ CPU �ı��
	ULONG uCPUID;

	// ��¼ ���� ����ָ��
	PVOID pVirtualAddr_VMXON_region;
	PVOID pVirtualAddr_VMXCS_region;
	PVOID pVirtualAddr_MSR_BitMap;

	PVOID pVirtualAddr_Host_Stack;

	// ��¼ �����ַ
	PHYSICAL_ADDRESS PhysicalAddr_VMXON_region;
	PHYSICAL_ADDRESS PhysicalAddr_VMXCS_region;
	PHYSICAL_ADDRESS PhysicalAddr_MSR_BitMap;



	// ��¼ VMM ״̬
	ULONG uStatus;
}VCPU_STC, *PVCPU_STC;
#pragma pack(pop)



PVCPU_STC g_pAllCpuStc;
NTSTATUS  VMXSTART();
NTSTATUS  VMXSTOP();