#pragma once

#include <ntddk.h>
#include "Misc.h"


/*
15:0 Basic exit reason
27:16 Reserved (cleared to 0)
28 Pending MTF VM exit
29 VM exit from VMX root operation
30 Reserved (cleared to 0)
31 VM-entry failure (0 = true VM exit; 1 = VM-entry failure)
*/
#pragma pack(push)  //�������״̬
#pragma pack(1)		//�趨Ϊ4�ֽڶ���
typedef struct _STC_EXIT_REASON
{
	ULONG	Basic_Exit_Reason : 16;
	ULONG	Reserved : 12;
	ULONG	Pending_MTF_VM_Exit : 1;
	// ��λΪ 1 ��ʾ �ڸ�ģʽ���� Vm-Exit
	ULONG	Vm_Exit_From_VM_Root_Operation : 1;
	ULONG	Reserved1 : 1;
	ULONG	Failure_Reason : 1;
}STC_EXIT_REASON, *PSTC_EXIT_REASON;


/*
Table 24-15.  Format of the VM-Exit Interruption-Information Field
Bit Position(s) Content
7:0 Vector of interrupt or exception
10:8 Interruption type:
0: External interrupt
1: Not used
2: Non-maskable interrupt (NMI)
3: Hardware exception
4�C5: Not used
6: Software exception
7: Not used
11 Error code valid (0 = invalid; 1 = valid)
12 NMI unblocking due to IRET

30:13 Reserved (cleared to 0)

31 Valid
*/

/*
Table 24-13.  Format of the VM-Entry Interruption-Information Field
7:0 Vector of interrupt or exception
10:8 Interruption type:
0: External interrupt
1: Reserved
2: Non-maskable interrupt (NMI)
3: Hardware exception
4: Software interrupt
5: Privileged software exception
6: Software exception
7: Other event
11 Deliver error code (0 = do not deliver; 1 = deliver)
30:12 Reserved
31 Valid
Table 24-13.  Format of the VM-Entry Interruption-Information Field (Contd.)
Bit Position(s) Content
*/
// ����һ���ṹ��
typedef struct _INTERRUPTION_INFORMATION_STC
{
	ULONG	Vector_Of_Interrupt_Or_Exception : 8;
	ULONG	Interruption_Type : 3;
	ULONG	Error_Code_Valid_Or_DeliverErrorCode : 1;
	// ����ֶ� VM-Entry û��
	ULONG	NMI_Unblocking_Due_To_IRET_For_Exit : 1;
	ULONG   Reserved : 18;
	ULONG	Valid : 1;
}INTERRUPTION_INFORMATION_STC, *PINTERRUPTION_INFORMATION_STC;


#pragma pack(pop)

typedef VOID(*HANDLE_EXIT_ARRAY)(PREGISTERS pGuestRegs);

// Table C-1.  Basic Exit Reasons 
// ���� 65 ���˳�ԭ��  0~64
#define MAX_EXIT_REASONS_COUNT		(65)
#define MAX_FAILVALID_COUNT			(29)
HANDLE_EXIT_ARRAY  handle_exit_array[MAX_EXIT_REASONS_COUNT];


VOID RegisterExitHandle();
VOID Exit_Handle(PREGISTERS pGuestRegs);