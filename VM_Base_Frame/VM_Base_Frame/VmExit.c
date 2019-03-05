#include "stdafx.h"
#include "VmExit.h"




/************************************************************************/
/*                                                                      */
/************************************************************************/

VOID DefaultHandle(PREGISTERS pGuestRegs)
{
	ULONG uGuestRip = _vmread(GUEST_RIP);
	ULONG uInstructionLen = _vmread(VM_EXIT_INSTRUCTION_LENGTH);
	uGuestRip += uInstructionLen;
	_vmwrite(GUEST_RIP, uGuestRip);
}

VOID HandleCpuid(PREGISTERS pGuestRegs)
{

	if (pGuestRegs->uEAX == 0x41414141 && \
		pGuestRegs->uECX == 0x42424242)
	{
		ULONG uGuestRip = _vmread(GUEST_RIP);
		ULONG uInstructionLen = _vmread(VM_EXIT_INSTRUCTION_LENGTH);
		uGuestRip += uInstructionLen;
		ULONG uGuestRsp = _vmread(GUEST_RSP);

		// ����������᷵��
		_vmxoff_no_root(pGuestRegs, (PVOID)uGuestRip, (PVOID)uGuestRsp);
	}



	// Ĭ�� CPUID ����
	REGISTERS  regs = { 0 };
	ULONG uFlag = pGuestRegs->uECX;
	_getcpuid(pGuestRegs->uECX, &regs.uEAX, &regs.uEBX, &regs.uECX, &regs.uEDX);
	pGuestRegs->uEAX = regs.uEAX;
	pGuestRegs->uEBX = regs.uEBX;
	pGuestRegs->uECX = regs.uECX;
	pGuestRegs->uEDX = regs.uEDX;
	//_vmwrite(GUEST_RSP, pGuestRegs->uESP);
	//if (uFlag == 1)
	//{
	//	// ��ֹ���˿���
	//	pGuestRegs->uECX = SetBit(pGuestRegs->uECX, 5, FALSE);
	//}

	DefaultHandle(pGuestRegs);
	return;
}




VOID HandleExcepitonOrNMI(PREGISTERS pGuestRegs)
{
	// �� Vm_Entry �д�
	ULONG uExitIntInformation = _vmread(VM_EXIT_INTERRUPTION_INFORMATION);
	ULONG uExitIntErrCode	  = _vmread(VM_EXIT_INTERRUPTION_ERROR_CODE);

	//KdPrint(("-[VM-Base-Frame]- : VM_ENTRY_INTERRUPTION_INFORMATION  Is :    0x%08X\n", uExitIntInformation));
	//KdPrint(("-[VM-Base-Frame]- : VM_EXIT_INTERRUPTION_ERROR_CODE  Is :    0x%08X\n", uExitIntErrCode));

	PINTERRUPTION_INFORMATION_STC pExitInter = (PINTERRUPTION_INFORMATION_STC)&uExitIntInformation;

	if (pExitInter->Error_Code_Valid_Or_DeliverErrorCode)
	{
		_vmwrite(VM_ENTRY_EXCEPTION_ERROR_CODE, uExitIntErrCode);
	}
	/************************************************************************/
	/*		������д�ıȽϼ򵥣�������˵ ��Ҫ���� �����ж� ֱ���쳣�Լ�����쳣��        */
	/************************************************************************/
	/*
	  �����Ҳ²� int 1 int 3 û�в�������쳣�����
	*/

	// ���� INT 1 �� #DB ��ԭ������� Dr�Ĵ�����ɵģ�Ҳ������ TF ��־λ����λ 
	// ��Ҫ Эͬ exit qualification �ֶ�
	// ����� int 1
	//if (pExitInter->Vector_Of_Interrupt_Or_Exception == DEBUG_EXCEPTION)
	//{
	//	// ������λ
	//	pExitInter->NMI_Unblocking_Due_To_IRET_For_Exit = 0;
	//	_vmwrite(VM_ENTRY_INTERRUPTION_INFORMATION, uExitIntInformation);
	//	_vmwrite(VM_ENTRY_INSTRUCTION_LENGTH, _vmread(VM_EXIT_INSTRUCTION_LENGTH));
	//	KdPrint(("-[VM-Base-Frame]- : INT 1 �ϵ㵽��  Addr Is :    0x%08X\n", _vmread(GUEST_RIP)));
	//	KdPrint(("-[VM-Base-Frame]- : INT 1 �ϵ㵽��  INSTRUCTION_LENGTH Is :    0x%08X\n", _vmread(VM_EXIT_INSTRUCTION_LENGTH)));
	//	return;
	//}

	// ����� int 3
	if (pExitInter->Vector_Of_Interrupt_Or_Exception == BREAKPOINT_EXCEPTION)
	{
		// ������λ
		pExitInter->NMI_Unblocking_Due_To_IRET_For_Exit = 0;
		_vmwrite(VM_ENTRY_INTERRUPTION_INFORMATION, uExitIntInformation);

		// д��ָ���  ������ VM_ENTRY_INSTRUCTION_LENGTH д�� VM_EXIT_INSTRUCTION_LENGTH ����һ�� �Դ�Ϊ��
		// �˳� �Ǹ�ģʽ���ǽ�����������Ǻͳ���˼ά���˸�Ȧ��
		_vmwrite(VM_ENTRY_INSTRUCTION_LENGTH, 1/*_vmread(VM_EXIT_INSTRUCTION_LENGTH)*/);
		KdPrint(("-[VM-Base-Frame]- : INT 3 �ϵ㵽��  Addr Is :    0x%08X\n", _vmread(GUEST_RIP)));
		KdPrint(("-[VM-Base-Frame]- : INT 3 �ϵ㵽��  INSTRUCTION_LENGTH Is :    0x%08X\n", _vmread(VM_EXIT_INSTRUCTION_LENGTH)));
	}
	else
	{
		DefaultHandle(pGuestRegs);
	}

	return;
}



typedef union _MovDrQualification {
	ULONG_PTR all;
	struct {
		ULONG_PTR debugl_register : 3;  //!< [0:2]
		ULONG_PTR reserved1 : 1;        //!< [3]
		ULONG_PTR direction : 1;        //!< [4]
		ULONG_PTR reserved2 : 3;        //!< [5:7]
		ULONG_PTR gp_register : 4;      //!< [8:11]
		ULONG_PTR reserved3 : 20;       //!<
		ULONG_PTR reserved4 : 32;       //!< [12:63]
	} fields;
}MovDrQualification;

#include <intrin.h>

VOID HandleDrxAccess(PREGISTERS pGuestRegs)
{
	KdPrint(("Drx Access!\r\n"));
	MovDrQualification exit_qualification;
	exit_qualification.all = _vmread(kExitQualification);

	ULONG* register_used = 0;
	switch (exit_qualification.fields.gp_register) {
	case 0: register_used = &pGuestRegs->uEAX; break;
	case 1: register_used = &pGuestRegs->uECX; break;
	case 2: register_used = &pGuestRegs->uEDX; break;
	case 3: register_used = &pGuestRegs->uEBX; break;
	case 4: register_used = &pGuestRegs->uESP; break;
	case 5: register_used = &pGuestRegs->uEBP; break;
	case 6: register_used = &pGuestRegs->uESI; break;
	case 7: register_used = &pGuestRegs->uEDI; break;
	}

	switch (exit_qualification.fields.direction) {
	
	case  0:
	{
		switch (exit_qualification.fields.debugl_register) {
		case 0: __writedr(0, *register_used); break;
		case 1: __writedr(1, *register_used); break;
		case 2: __writedr(2, *register_used); break;
		case 3: __writedr(3, *register_used); break;
		case 4: __writedr(4, *register_used); break;
		case 5: __writedr(5, *register_used); break;
		case 6: __writedr(6, *register_used); break;
		case 7: _setdr7(*register_used); break;
		default: break;
		}
		break;
	}
	case   1:

		switch (exit_qualification.fields.debugl_register) {
		case 0: *register_used = __readdr(0); break;
		case 1: *register_used = __readdr(1); break;
		case 2: *register_used = __readdr(2); break;
		case 3: *register_used = __readdr(3); break;
		case 4: *register_used = __readdr(4); break;
		case 5: *register_used = __readdr(5); break;
		case 6: *register_used = __readdr(6); break;
		case 7: *register_used = _getdr7(); break;
		default: break;
		}
		break;
	}

	DefaultHandle(pGuestRegs);
}


//************************************
// Method:    RegisterExitHandle
// FullName:  RegisterExitHandle
// Access:    public 
// Returns:   VOID
// Qualifier:
// Remark:    ע���˳�����
//************************************
VOID RegisterExitHandle()
{
	// ֱ�ӳ�ʼ��
	for (int i = 0; i < MAX_EXIT_REASONS_COUNT; i++)
	{
		handle_exit_array[i] = DefaultHandle;
	}

	// ��Ҫ���д�����б�

	// Exception or non-maskable interrupt (NMI).
	handle_exit_array[0] = HandleExcepitonOrNMI;
	// CPUID
	handle_exit_array[10] = HandleCpuid;
	// DrxAccess
	handle_exit_array[29] = HandleDrxAccess;
}




//************************************
// Method:    Exit_Handle
// FullName:  Exit_Handle
// Access:    public 
// Returns:   VOID
// Qualifier:
// Parameter: PREGISTERS pGuestRegs
// Remark:    �����˳��¼�
//************************************
VOID Exit_Handle(PREGISTERS pGuestRegs)
{
	KIRQL OldIrql;
	ULONG uExit_Reason = _vmread(EXIT_REASON);
	PSTC_EXIT_REASON pExitReason = (PSTC_EXIT_REASON)&uExit_Reason;
	ULONG uVmFailValid = _vmread(VM_INSTRUCTION_ERROR);
	OldIrql = KeRaiseIrqlToDpcLevel();
	
#ifdef _DBG
	KdPrintExitReason(pExitReason->Basic_Exit_Reason);
	if (pExitReason->Failure_Reason)
	{
		// ����׶�ʧ����
		KdPrintFailValid(uVmFailValid);
		KdPrint(("-[VM-Base-Frame]- : VM_EXIT_INSTRUCTION_INFORMATION Is :    0x%X\n", _vmread(VM_EXIT_INSTRUCTION_INFORMATION)));
		KdBreakPoint();
	}
	
	if (uVmFailValid !=  0)
	{
		KdPrintFailValid(uVmFailValid);
		KdBreakPoint();
	}
#endif	
	// ����
	handle_exit_array[pExitReason->Basic_Exit_Reason](pGuestRegs);
	
	KeLowerIrql(OldIrql);


	return;
}
