//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"

static const char FromKernel[] = "kernel";



class KMultiCoreSched : public CMultiCoreSupport
{
public:
    KMultiCoreSched (CLogger *m_Logger, CMemorySystem *pMemorySystem){

    }
    ~KMultiCoreSched (void){

    }

    boolean Initialize (void)   { return TRUE; }

    void Run (unsigned nCore) {
        //scheduler stuff
        //start scheduling
        
    }

private:

private:
    CLogger *m_Logger;
};


CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer)
{
	m_ActLED.Blink (5);	// show we are alive
    
    for (int i = 0; i < CORES; ++i)
        m_MultiCoreSched[i] = new KMultiCoreSched(&m_Logger, &m_Memory);
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (&m_Serial);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}
    if (bOK)
    {
        for (int i = 0; i < CORES && bOK; ++i)
          bOK = m_MultiCoreSched[i].Initialize ();   // must be initialized at last
    }
	return bOK;
}

#if 1

#include "thread.cpp"

#endif

static thread_t s_thread;
static thread_t main_thread;

//maybe let scheduler setup the first thread(main)'s context
//this is used for finding the TCB/Thread during scheduling
static thread_t *RUNNING_THREAD_ON_CORE[1] = {&main_thread};

static TCB main_tcb = TCB();


TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);
        //thread_create(&(threads[0]),&ThreadWorker,5,this);
        thread_t * thread = &s_thread;

        TCB *tcb = new TCB();
        thread->tcb = tcb;
        tcb->stack_size = TASK_STACK_SIZE;
        tcb->stack  =  new u8[TASK_STACK_SIZE]; //new Stack();
        memset (&(tcb->regs), 0, sizeof(tcb->regs));
        //stack grow downward
        tcb->regs.sp = (size_t) tcb->stack + TASK_STACK_SIZE;
        tcb->regs.lr = (size_t) stub;

        tcb->regs.r0 = (size_t) ThreadWorker;
        tcb->regs.r1 = (size_t) 5;
        tcb->regs.r2 = (size_t) this;
        tcb->state = THREAD_READY;

        //thread_yield part 
        //thread_t *curThread = RUNNING_THREAD_ON_CORE[CMultiCoreSupport::ThisCore()];
        //&(curThread->tcb->regs)
        TaskSwitch(&(main_tcb.regs),&(tcb->regs)); //SaveLoadSwitch

        long exitValue = 1;//thread_join(&(threads[0]));
        m_Logger.Write (FromKernel, LogNotice,"Thread %d returned with %ld\n",0,exitValue);
	return ShutdownHalt;
}

void 
CKernel::ThreadWorker(int i, void *ker){
    CKernel *kernel = (CKernel *) ker;
    kernel->m_Logger.Write (FromKernel, LogNotice,"Hello from thread %d \n",i);    
    TaskSwitch(&(s_thread.tcb->regs),&(main_tcb.regs));
    //thread_exit(100+i);
}




void CKernel::TimerHandler (unsigned hTimer, void *pParam, void *pContext)
{
    #if 1
    	// jump to an invalid address (execution is only allowed below _etext, see circle.ld)
    	void (*pInvalid) (void) = (void (*) (void)) 0x500000;

    	(*pInvalid) ();
    #else
    	// alternatively execute an undefined instruction
    	asm volatile (".word 0xFF000000");
    #endif
}

