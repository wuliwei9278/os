
//namespace rpvm {

#include <circle/sysconfig.h>
#include <circle/types.h>
#include <circle/util.h>
#include <circle/synchronize.h>

enum THREAD_STATE
{
	THREAD_INIT,
  THREAD_READY,
  THREAD_RUNNING,
  THREAD_WAITTING,
  THREAD_FINISHED
};

struct ARM_REGFILES
{
	//size_t is the word size of native architecture
	//The standard 32-bit ARM calling convention allocates the 16 ARM registers as:
  // r15 is the program counter.
  // r14 is the link register. 
  // (The BL instruction, used in a subroutine call, stores the return address in this register).
	// r13 is the stack pointer. (The Push/Pop instructions in "Thumb" operating mode use this register only).
	// r12 is the Intra-Procedure-call scratch register.
	// r4 to r11: used to hold local variables.
	// r0 to r3: used to hold argument values passed to a subroutine, and also hold results returned from a subroutine.
	size_t r0;
	size_t r1;
	size_t r2;
	size_t r3;
	size_t r4;
	size_t r5;
	size_t r6;
	size_t r7;
	size_t r8;
	size_t r9;
	size_t r10;
	size_t r11;
	size_t r12;
	size_t sp;
	size_t lr; 
};

#ifdef __cplusplus
extern "C" {
#endif

void TaskSwitch(ARM_REGFILES *pOldRegs, ARM_REGFILES *pNewRegs);

#ifdef __cplusplus
}
#endif

// TASK CONTROL BLOCK
// consist of stack informationa,
// saved registers(including sp, pc, or even return value)
struct TCB {
	// thread state may changed by other threads
	volatile THREAD_STATE state; 
	ARM_REGFILES regs;
	size_t       stack_size;
	u8           *stack;
};

struct task_t {
	size_t id;
	size_t priority;
	TCB *tcb;
};

typedef task_t thread_t;

typedef void (thread_handler)(int, void* kernel);

void
stub(thread_handler func,int arg,void *kernel){ //equivalent to TaskEntry
	(*func)(arg,kernel); //Execute the function func()
	//thread_exit(0);//If func() does not call exit,call it here.
}

#if 0

void thread_exit(int ret) {
	 thread_t *currentThread = thread_current_tcb_array[0];
	 TCB *runningTCB   = currentThread->tcb;
   runningTCB->state = THREAD_FINISHED;
   
   runningTCB->regs.r0 = ret;
}

void
stub(thread_handler func,int arg,void *kernel){ //equivalent to TaskEntry
	(*func)(arg,kernel); //Execute the function func()
	thread_exit(0);//If func() does not call exit,call it here.
}

void 
thread_create(thread_t *thread, thread_handler func, int arg,void *kernel){
	
	TCB *tcb = new TCB();
	thread->tcb = tcb;
	tcb->stack_size = TASK_STACK_SIZE;
	tcb->stack  =  new u8[TASK_STACK_SIZE]; //new Stack();
	memset (&(tcb->regs), 0, sizeof(tcb->regs));
	//stack grow downward
	tcb->regs.sp = (size_t) tcb->stack + TASK_STACK_SIZE;
	tcb->regs.lr = (size_t) stub;
	// //For x86 system to save/restore per-thread state
		// //follows calling convetion and push func and arg to create a stack_frame
		// *(tcb->regs.sp) = arg;
		// tcb->regs.sp -= 1;
		// *(tcb->regs.sp) = func;
		// tcb->regs.sp -= 1;
		// //Createanotherstackframesothatthread_switchworkscorrectly.
		// //even for new created thread, it will looks like "returned" from some context switch
		// //thread_dummy_switch_frame
		// *(tcb->regs.sp) = stub
		// tcb->regs.sp -= 1;
		// tcn->regs.sp -= SizeOfPopad; 
	/*
	  Use the bottom of stack to find current TCB
	  In the future, we can use hardware support to look up processor ID
	  with a global array of TCB* of size == the number of cores 
	 */
	// *((size_t *)tcb->regs.sp) = (size_t) tcb;
	// tcb->regs.sp -= sizeof(size_t);

	tcb->regs.r0 = (size_t) func;
	tcb->regs.r1 = (size_t) arg;
	tcb->regs.r2 = (size_t) kernel;
	tcb->state = THREAD_READY;
}


void thread_yield() {
	thread_t *currentThread = thread_current_tcb_array[0];
	thread_t *finishedThread = 0;
	TCB *runningTCB   = currentThread->tcb;	
	TCB *nextTCB = 0;
	
	EnterCritical(FIQ_LEVEL);

	if(nextTCB == 0){
		// do nothing 
	} else {
		runningTCB->state = THREAD_READY;
		ReadyList[currentThread->id] = currentThread;

		//switch to new thread, how to re-enable interrupt ???
		TaskSwitch(&(runningTCB->regs),&(nextTCB->regs));
		//we returned from other thread, maybe happened a long time later
		runningTCB->state = THREAD_RUNNING;
	}

	//TODO Scheduler Clean Finished Thread

	LeaveCritical();
}

int thread_join(thread_t *thread) {
	while (thread->tcb->state != THREAD_FINISHED) {
		thread_yield();
	}
	EnterCritical(FIQ_LEVEL);
	int ret  = thread->tcb->regs.r0;
	FinishedList[FinishedListCount++] = thread;
	LeaveCritical();
	return ret;
}
#endif
//}