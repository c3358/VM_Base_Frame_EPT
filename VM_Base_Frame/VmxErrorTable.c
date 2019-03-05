#ifdef DBG
#include "stdafx.h"
#include "VmxErrorTable.h"





static wchar_t* szExit_Reason_Des[MAX_EXIT_REASONS_COUNT] = 
{
	L"Vm-Exit ���쳣��NMI����",
	L"Vm-Exit ���ⲿ�ж�����",
	L"Vm-Exit �� triple-fault ����",
	L"�� INT �źŵ��� Vm-Exit",
	L"�� SIPI ���� Vm-Exit",
	L"I/O SMI: ָ�ӵ� SMI ������ SMM Vm-Exit,SMI ������ I/O ָ���",
	L"Other SMI: ָ�ӵ� SMI ������ SMM Vm-Exit,SMI �������� I/O ָ���",
	L"Vm-Exit �� 'interupt-window exiting' Ϊ1ʱ����",
	L"Vm-Exit �� 'NMI-window exiting' Ϊ1ʱ����",
	L"���Խ��������л������� Vm-Exit",
	L"����ִ�� CPUID ָ��� Vm-Exit",
	L"����ִ�� GETSEC ָ��� Vm-Exit",
	L"����ִ�� HLT ָ��� Vm-Exit",
	L"����ִ�� INVD ָ��� Vm-Exit",
	L"����ִ�� INVLPG ָ��� Vm-Exit",
	L"����ִ�� RDPMC ָ��� Vm-Exit",
	L"����ִ�� RDTSC ָ��� Vm-Exit",
	L"����ִ�� RSM ָ��� Vm-Exit",
	L"����ִ�� VMCALL ָ��� Vm-Exit",
	L"����ִ�� VMCLEAR ָ��� Vm-Exit",
	L"����ִ�� VMLAUNCH ָ��� Vm-Exit",
	L"����ִ�� VMPTRLD ָ��� Vm-Exit",
	L"����ִ�� VMPTRST ָ��� Vm-Exit",
	L"����ִ�� VMREAD ָ��� Vm-Exit",
	L"����ִ�� VMRESUME ָ��� Vm-Exit",
	L"����ִ�� VMWRITE ָ��� Vm-Exit",
	L"����ִ�� VMXOFF ָ��� Vm-Exit",
	L"����ִ�� VMXON ָ��� Vm-Exit",
	L"���Է��ʿ��ƼĴ������� Vm-Exit",
	L"����ִ�� MOV-DR ָ������ Vm-Exit",
	L"����ִ�� I/O ָ������ Vm-Exit",
	L"����ִ�� RDMSR ָ��� Vm-Exit",
	L"����ִ�� WRMSR ָ��� Vm-Exit",
	L"�� Vm-Entry ʱ,������Ч�� guest-state ���� Vm-Exit",
	L"�� Vm-Entry ʱ,���� MSR ����ʧ�ܵ��� Vm-Exit",
	L"[---������ Vm-Exit REASON 35---]",
	L"����ִ�� MWAIT ָ������ Vm-Exit",
	L"�� Vm-Entry ��,�� MTF(monitor trap flag) ���� Vm-Exit",
	L"[---������ Vm-Exit REASON 38---]",
	L"����ִ�� MONITOR ָ������ Vm-Exit",
	L"����ִ�� PAUSE ָ������ Vm-Exit",
	L"�� Vm-Entry ʱ,���� machine-check ���� Vm-Exit",
	L"[---������ Vm-Exit REASON 42---]",
	L"���� VTPR ���� TPR threshold ֵ������ Vm-Exit",
	L"���� APIC-access page ҳ�浼�� Vm-Exit",
	L"ִ�� EOI���⻯ ���� Vm -Exit",
	L"���Է��� GDTR �� IDTR �Ĵ������� Vm-Exit",
	L"���Է��� LDTR �� TR �Ĵ������� Vm-Exit",
	L"���� EPT violation ������ Vm-Exit",
	L"���� EPT misconfiguration ������ Vm-Exit",
	L"����ִ�� INVEPT ָ������ Vm-Exit",
	L"����ִ�� RDTSCP ָ������ Vm-Exit",
	L"���� VMX-preemption timer ��ʱ������ Vm-Exit",
	L"����ִ�� INVVPID ָ������ Vm-Exit",
	L"����ִ�� WBINVD ָ������ Vm-Exit",
	L"����ִ�� XSETBV ָ������ Vm-Exit",
	L"���ڳ����� APIC-access pageд������ Vm-Exit",
	L"����ִ�� RDRAND ָ������ Vm-Exit",
	L"����ִ�� INVPCID ָ������ Vm-Exit",
	L"����ִ�� VMFUNC ָ������ Vm-Exit",
};


static wchar_t* szVmFailValid_Des[MAX_FAILVALID_COUNT] =
{
	L"VmFailValid ������",
	L"VMCALL ָ��ִ���� root ����",
	L"VMCLEAR ָ�����������Ч�������ַ",
	L"VMCLEAR ָ��������� VMXON ָ��",
	L"VMLAUNCH ִ��ʱ, current-VMCS ״̬�� �� 'clear'",
	L"VMRESULT ִ��ʱ, current-VMCS ״̬�� �� 'launched'",
	L"VMRESULT �� VMXOFF ָ���",
	L"Vm-entry ����ʱ, current-VMCS ������Ч�� VM-execution �����ֶ�",
	L"Vm-entry ����ʱ, current-VMCS ������Ч�� host-state �ֶ�",
	L"VMPTRLD ָ�����������Ч�������ַ",
	L"VMPTRLD ָ��������� VMXON ָ��",
	L"VMPTRLD ָ��ִ��ʱ, VMCS �ڵ� VMCS IDֵ����",
	L"VMREAD/VMWRITE ָ���д current-VMCS �ڲ����ڵ��ֶ�",
	L"VMWRITE ָ�� ��ͼд current-VMCS ��ֻ���ֶ�",
	L"VmFailValid ���� 14",
	L"VMXON ָ��ִ���� VMX root operation ����",
	L"Vm-entry ����ʱ, current-VMCS ������Ч�� executive-VMCS ָ��",
	L"Vm-entry ����ʱ, current-VMCS ʹ���˷� 'launched' ״̬�� executive-VMCS",
	L"Vm-entry ����ʱ, current-VMCS �ڵ� executive-VMCS ָ�벻�� VMXON ָ��",
	L"ִ�� VMCALL ָ������ SMM-transfer monitor ʱ,VMCS �� 'clear'",
	L"ִ�� VMCALL ָ������ SMM-transfer monitor ʱ,VMCS �ڵ� Vm-Exit control(����) �ֶ���Ч",
	L"VmFailValid ���� 21",
	L"ִ�� VMCALL ָ������ SMM-transfer monitor ʱ,MSEG �ڵ� MESG IDֵ��Ч",
	L"�� SMM ˫�ؼ�ش��������ִ�� VMXOFF ָ��",
	L"ִ�� VMCALL ָ������ SMM-transfer monitor ʱ,MSEG ��ʹ������Ч�� SMM monitor ����",
	L"���� 'Vm-Entry that return from SMM' ����ʱ, �� executive VMCS ����������Ч�� VM-execution control �ֶ�",
	L"VM-entry����ʱ,VMLAUNCH �� VMRESUME �� MOV-SS/POP-SS ����",
	L"VmFailValid ���� 27",
	L"INVEPT/INVVPID ָ��ʹ������Ч�Ĳ�����",
};



/*
�����������и��������һ���±��0��ʼһ���±�� 1 ��ʼ
*/
VOID _KdPrintExitReason(ULONG uExitReason)
{
	if (uExitReason < MAX_EXIT_REASONS_COUNT)
	{
		KdPrint(("-[VM-Base-Frame]- : 0x%08X -- %S\n", uExitReason, szExit_Reason_Des[uExitReason]));
	}
	else
	{
		KdPrint(("-[VM-Base-Frame]- : _KdPrintExitReason Invalid  :  0x%08X\n", uExitReason));
	}
}


VOID _KdPrintFailValid(ULONG uFailValid)
{
	if (uFailValid != 0 && uFailValid < MAX_FAILVALID_COUNT)
	{
		KdPrint(("-[VM-Base-Frame]- :  0x%08X --%S\n", uFailValid, szVmFailValid_Des[uFailValid]));
	}
	else
	{
		KdPrint(("-[VM-Base-Frame]- : _KdPrintFailValid Invalid  :   0x%08X\n", uFailValid));
	}
}


#endif