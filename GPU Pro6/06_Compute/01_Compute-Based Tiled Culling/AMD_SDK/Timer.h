//
// Copyright 2014 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
//
// AMD is granting you permission to use this software and documentation (if
// any) (collectively, the “Materials”) pursuant to the terms and conditions
// of the Software License Agreement included with the Materials.  If you do
// not have a copy of the Software License Agreement, contact your AMD
// representative for a copy.
// You agree that you will not reverse engineer or decompile the Materials,
// in whole or in part, except as allowed by applicable law.
//
// WARRANTY DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE SOFTWARE
// WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM CUSTOM OF
// TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE OF THE
// SOFTWARE IS ASSUMED BY YOU.
// Some jurisdictions do not allow the exclusion of implied warranties, so
// the above exclusion may not apply to You. 
// 
// LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
// NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
// INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
// THE SOFTWARE OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  
// In no event shall AMD's total liability to You for all damages, losses,
// and causes of action (whether in contract, tort (including negligence) or
// otherwise) exceed the amount of $100 USD.  You agree to defend, indemnify
// and hold harmless AMD and its licensors, and any of their directors,
// officers, employees, affiliates or agents from and against any and all
// loss, damage, liability and other expenses (including reasonable attorneys'
// fees), resulting from Your use of the Software or violation of the terms and
// conditions of this Agreement.  
//
// U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
// RIGHTS." Use, duplication, or disclosure by the Government is subject to the
// restrictions as set forth in FAR 52.227-14 and DFAR252.227-7013, et seq., or
// its successor.  Use of the Materials by the Government constitutes
// acknowledgement of AMD's proprietary rights in them.
// 
// EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
// stated in the Software License Agreement.
//

/***************************************************************************************
 * Purpose of Timer.cpp/Timer.h
 * 
 * These source files provide high accuracy timer functionality to easily add profiling to
 * your code. Special care was taken to ensure non-blocking behaviour of the GPU-timer and 
 * to make it easy to instrument your applications for both accurate CPU- and GPU- timing 
 * information.
 *
 * One side effect is that GPU timing information will only be available several frames 
 * later to avoid stalling the CPU waiting for GPU counter data to return. 
 * When profiling events that don't happen regularly (i.e. every frame when activated) you 
 * should not use the TimerEx class. Examples for this include texture baking, computation 
 * of shadow or reflection maps that get updated at low frequency. (See usage examples for details)
 *
 * The TimerEx class and macros have been written to enable instrumentation in your code 
 * similar to what is done with PIX performance counters, and to provide timing information in 
 * real-time within the program.
 * The macros already include DXUT_BeginPerEvent/DXUT_EndPerfEvent, so if some code is already 
 * instrumented for DXUT adding this timer should only be search and replace operation. (See  
 * description and examples for more details)
 *
 * The additional features of TimerEx cost a bit of extra performance when compared to the basic 
 * Timer class. If you are using TimerEx or its macros you may want to disable it in release builds.
 *
 *
 * Components of this code:
 * ========================
 *
 * Macros
 * ------
 *
 * TIMER_Init( device )
 *   This has to be called once at initialization time to give the ID3D11Device* to the timer.
 *   If NULL is passed to the timer GPU timing will not be available.
 *
 * TIMER_Destroy( )
 *   This has to be called when the ID3D11Device* gets destroyed to uninitialize the timer.
 *
 * TIMER_Reset( )
 *   This has to be called once at the start of each frame to reset the timers
 *
 * TIMER_Begin( col, name )
 *   This starts a new Timer (CPU and GPU if available, see TIMER_Init). 
 *   For each TIMER_Begin a corresponding TIMER_End has to be called.
 *   Note that for the GPU timer to work properly the name used for a code block should be the
 *   same every frame. Several timers may have the same name, provided they have different parents 
 *   (similar to a file system). If a timer with the same name is started in the same context the 
 *   existing timer with that name will be restarted. See examples for details.
 *
 * TIMER_End( )
 *	This ends a timer which was previously started with TIMER_Begin. 
 *
 * TIMER_ProfileCodeBlock( col, name )
 *   Convenience macro. Add this inside a code block to add profiling to it. See examples for details.
 *
 * TIMER_GetTime( Cpu_Gpu, name )
 *   Retrieve the timing value of a timer in microseconds.
 *   The parameter Cpu_Gpu can either be Cpu or Gpu depending what time you want to retrieve.
 *   Name can be the name of a "local" timer or a name including a path to the timer in the tree 
 *   structure. Valid path seperators are \, / or |
 *   See Examples for more details.
 *
 * TIMER_WaitForGpuAndGetTime( name )
 *   This macro stalls the CPU until the result of a GPU timer is available.
 *   Since it forces the CPU to idle, this macro should not be used in time critical parts of your app.
 *
 *
 * Classes
 * -------
 *
 * TimerExHelper
 *   Create an Instance of this class to automatically add profiling to a code block. 
 *   This class starts a timer in the constructor and ends the timer in its destructor.
 *
 * TimerEx
 *   Singleton that manages the timer tree generated by TIMER_Begin and TIMER_End.
 *   Functions:
 *     - Instance        : retrieve the instance of TimerEx
 *     - GetDevice       : get the device passed to TimerEx at Init
 *     - Init            : initialize TimerEx, pass ID3D11Device* if GPU profiling should be used
 *     - Destroy         : uninitialize TimerEx and release resources so the ID3D11Device* can be destroyed
 *     - Reset           : notify all timers that a new frame starts, remove unused timer events
 *     - Start           : start a timer
 *     - Stop            : stop a timer
 *     - GetTime         : retrieve the timing result of a timer
 *     - GetTimer        : retrieve a TimerEvent*. This ptr should not be kept past a reset.
 *                         it can be used to manually iterate through the timer tree
 *
 * TimerEvent
 *   Manages one CpuTimer and one GpuTimer (if ID3D11Device is specified) plus the name of 
 *   the timed event and the pointers of the tree structure.
 *   Functions:
 *     - GetTime       : retrieve the timing result for either gpu or cpu. 
 *                       Specify if the cpu should wait to the latest gpu time to be available
 *     - GetTimer      : retrieve a nested TimerEvent* by name or relative path
 *     - GetParent     : retrieve the parental TimerEvent*
 *     - GetFirstChild : retrieve the first child-TimerEvent*
 *     - GetNextTimer  : retrieve the next TimerEvent* with the same parent
 *   
 *
 * Timer
 *   Lightweight interface to instrument your code without the overhead introduced by the TimerEx class.
 *   The times measured by Timer will add up when starting/stopping the timer multiple times without
 *   resetting the timer.
 *   Create an instance of either of the derived classes for each event you want to profile:
 *     - CpuTimer    : measures the time taken on the CPU to execute from Start to Stop
 *     - GpuTimer    : measures the time taken on the GPU to execute from Start to Stop
 *                     When using GpuTimer please note that the timing results may only be available 
 *                     several frames later, so numTimeStamps should specify enough space for at least
 *                     numTimesTimerStartedPerFrame*maxNumFramesLag timing values. Please also keep in mind
 *                     that Crossfire configurations are likely to increase the number of frames until a result 
 *                     is ready. 
 *                     If numTimeStamps is chosen too small this will result in the CPU stalling.
 *     - GpuCpuTimer : Measure the time the GPU takes to execute the commands beeing issued between Start and Stop
 *                     by measuring the time on the CPU.
 *                     This will stall the CPU twice, once at Start and once at Stop.
 *
 *
 * Usage examples:
 * ===============
 
 * Macros
 * ------
 *
 * ***************************
 * ** EXAMPLE 1: Simple use **
 * ***************************
 *
 *  // TIMER_Init( device ) was previously called in device creation function
 *  // TIMER_Destroy( ) was added to the OnD3D11DestroyDevice function
 *
 *  For every frame:
 *  
 *  // Start of frame: reset the timer
 *  TIMER_Reset( );
 *
 *  TIMER_Begin( 0, L"Z prepass" );
 *
 *    TIMER_Begin( 0, L"solid" );
 *    // Render solid objects in Z pre-pass
 *    TIMER_End( ); // "solid"

 *    TIMER_Begin( 0, L"transparent" );
 *    // Render transparent (alpha-tested) objects in Z pre-pass
 *    TIMER_End( ); // "transparent"
 * 
 *  TIMER_End( ); // "Z prepass"
 *
 *  // Other render commands go here...
 *
 *  // End of frame: retrieval of timing results:
 *  double ZPrepass_Solid_Gpu = TIMER_GetTime( Gpu, L"Z prepass|solid" );
 *  double ZPrepass_Transparent_Gpu = TIMER_GetTime( Gpu, L"Z prepass|transparent" );
 *  double ZPrepass_Total_Gpu = TIMER_GetTime( Gpu, L"Z prepass" );
 *
 *
 * *****************************
 * ** EXAMPLE 2: Advanced use **
 * *****************************
 *
 *  // TIMER_Init( device ) was previously called in device creation function
 *  // TIMER_Destroy( ) was added to the OnD3D11DestroyDevice function
 *
 *  For every frame:
 *   
 *  // Start of frame: reset the timer
 *  TIMER_Reset( );
 *
 *  TIMER_Begin( 0, L"Render" );
 *
 *  TIMER_Begin( 0, L"Z prepass" );
 *
 *  {
 *    // This will automatically profile all GPU commands in the current code block
 *    TIMER_ProfileCodeBlock( col, L"solid" );
 *    // Render solid objects in Z pre-pass
 *  }
 *
 *  {
 *    TIMER_ProfileCodeBlock( col, L"transparent" );
 *    // Render transparent trees
 *  }
 *
 *  {
 *    TIMER_ProfileCodeBlock( col, L"transparent" );
 *    // Render transparent bushes
 *  }
 *
 *  TIMER_End( ); // "Z prepass"
 *
 *  TIMER_End( ); // "Render"
 *
 *  // Other render commands go here...
 *
 *  // End of frame: retrieval of timing results:
 *  // ZPrepass_Total_Transparent_Gpu will return the total GPU time spent in "transparent" (trees and bushes GPU rendering time)
 *  double ZPrepass_Total_Transparent_Gpu = TIMER_GetTime( Gpu, L"Render|Z prepass|transparent" );
 *  // ZPrepass_Total_Transparent_Cpu will return the total CPU time spent in "transparent" (trees and bushes CPU time)
 *  double ZPrepass_Total_Transparent_Cpu = TIMER_GetTime( Cpu, L"Render|Z prepass|transparent" );
 *  double Render_Total_Gpu = TIMER_GetTime( Gpu, L"Render" );
 *
 *
 * ********************
 * ** OTHER EXAMPLES **
 * ********************
 *
 *  // TIMER_Init( device ) was previously called in device creation function
 *  // TIMER_Destroy( ) was added to the OnD3D11DestroyDevice function
 *
 *  For every frame:
 *  
 *  // Start of frame: reset the timer
 *  TIMER_Reset( );
 *
 *  TIMER_Begin( 0, L"Render" );
 *
 *    TIMER_Begin( 0, L"Z prepass" );
 *
 *      TIMER_Begin( 0, L"solid" );
 *
 *      // Render solid objects in Z pre-pass
 *      
 *      // !! transparent_cpu will return 0 because "solid" has not ended yet !!
 *      double transparent_cpu = TIMER_GetTime( Cpu, L"Render|Z prepass|solid" );
 *
 *      // transparent_gpu will return the last Gpu timing available
 *      double transparent_gpu = TIMER_GetTime( Gpu, L"Render|Z prepass|solid" );
 *
 *      TIMER_End( ); // "solid"
 * 
 *    TIMER_End( ); // "Z prepass"
 *
 *    TIMER_Begin( col, L"main" );
 *    {
 *       TIMER_ProfileCodeBlock( col, L"solid" );
 *    }
 *    TIMER_End( ); // "main"
 *
 *  TIMER_End( ); // "Render"
 *
 *  // Other render commands go here...
 *
 *  // a little stroll through the timer tree
 *  TimerEvent* renderTimer      = TIMER_GetTimer( L"Render" );
 *  TimerEvent* zprepassTimer    = renderTimer->GetFirstChild();
 *  TimerEvent* mainTimer        = zprepassTimer->GetNextTimer();
 *  TimerEvent* solidTimer       = mainTimer->GetFirstChild();
 *
 *  // Will retrieve the time the CPU spends during "Render|main|solid"
 *  double solid_cpu = solidTimer->GetTime( ttCpu );
 *
 *  // Will return NULL since there is no other timer at the same level as "Render|main"
 *  TimerEvent* willreturnnull   = mainTimer->GetNextTimer(); 
 *  
 *
 * ****************************
 * ** IMPLEMENTATION DETAILS **
 * ****************************  
 *
 * TimerExHelper
 * -------------
 *
 *   // start of frame, reset timers
 *   TIMER_Reset( );
 *   { // <-timer 0 starts here
 *
 *     // in debug mode this would create an assert since a timer is already running
 *     // in release this would reset the timers but the already running timer would keep it's start time
 *     // TIMER_Reset( );
 *  
 *     //some code
 *     TimerExHelper timer0( 0, L"timer0" );
 *     // some more code
 *
 *     for(int i = 0; i<10; ++i) 
 *     { // <-timer 1 starts here
 *       TimerExHelper timer1( 0, L"timer1" );
 *
 *       //even more code
 *
 *       // start a timer with a seperate name for each iteration of the loop
 *       // these will all be childnodes to "timer1"
 *       LPWSTR timerName[32];
 *       swprintf( timerName,32, L"timer(i=%d)", i);
 *       TimerExHelper timer_i( 0, timerName );
 *     }// <-timer 1 ends here
 *
 *     // retrieve the runtime of the loop
 *     double timer1 = TIMER_GetTime( Cpu, L"timer1" );
 *   } // <-timer 0 ends here
 *
 *   // retrieve all timers from the immer loop
 *   TimerEvent* timer = TIMER_GetTimer( L"timer0|timer1" )->GetFirstChild();
 *   double timer_i[10];
 *   int i = 0;
 *   while( NULL != timer )
 *   {
 *     timer_i[i] = timer->GetTime( ttCpu );
 *     timer->GetNextTimer();
 *     ++i;
 *   }
 *
 *
 * TimerEx
 * -------
 * Using TimerEx is just like using the macros, except the macros also include DXUT_PerfEvent-marker
 * Macro:                       corresponding TimerEx function call:
 *  TIMER_Init( device );        TimerEx::Instance( ).Init( device );
 *  TIMER_Destroy( )             TimerEx::Instance( ).Destroy( );
 *  TIMER_Reset( );              TimerEx::Instance( ).Reset( );
 *  TIMER_Begin( col, name );    TimerEx::Instance( ).Start( name );
 *  TIMER_End( );                TimerEx::Instance( ).Stop( );
 *  TIMER_GetTime( Gpu, name );  TimerEx::Instance( ).GetTime( ttGpu, name [optional param bool stall CPU?] );
 *  TIMER_GetTime( Cpu, name );  TimerEx::Instance( ).GetTime( ttCpu, name [optional param is ignored] );
 *  TIMER_GetTimer( name );      TimerEx::Instance( ).GetTimer( name );
 *
 * Timer
 * -----
 * The Timer class is a lightweight timing interface. It's designed to interfere with the program timing as little
 * as possible. TimerEx on the other hand is written with performance in mind, but will consume some time and memory
 * when used excessively and thus should not be used inside heavily used parts of time critical code.
 * Especially when using GpuTimer some care must be taken, since using it is not as straight forward as the CpuTimer.
 * This is due to its non blocking behaviour and the difference of up to several frames between commands beeing issued
 * to the GPU und the commands having been executed and the timing result beeing available.
 *
 * {
 *   CpuTimer cpuTimer;
 *   Timer* gpuTimer1 = new GpuTimer( pDev );
 *   GpuTimer gpuTimer2( pDev );
 *   GpuTimer gpuTimer3( pDev);
 *   GpuTimer gpuTimer4( pDev, 0, 1 ); // <- we only need space for one event because we'll retrieve this data stalling the CPU
 *                                     // choosing the numTimeStamps too low will result in the CPU stalling
 *
 *   gpuTimer1->Start();
 *   gpuTimer3.Start();
 *   cpuTimer.Start();
 *   // so some stuff here
 *   cpuTimer.Stop();
 *   gpuTimer3.Stop();
 *   gpuTimer1->Stop();
 *
 *   // get timing results
 *   double timeCpu1 = cpuTimer.GetTime();   // get the CPU time required to do "some stuff"
 *   double timeGpu1 = gpuTimer1->GetTime(); // get the GPU time required to do "some stuff". This will be 0.0 for the first few frames
 *
 *   cpuTimer.Reset();  // reset CPU timer
 *   gpuTimer3.Reset(); // try to reset GPU timer. WARNING: THIS WILL NOT WORK. We'll do it anyway and explain what happens later.
 *
 *   gpuTimer3.Start();
 *   gpuTimer2.Start();
 *   cpuTimer.Start();
 *   // so more stuff here
 *   cpuTimer.Stop();
 *   gpuTimer3.Stop();
 *   gpuTimer2.Stop();
 *
 *   // get timing results
 *   double timeCpu2 = cpuTimer.GetTime();  // get the CPU time required to do "more stuff"
 *   double timeGpu2 = gpuTimer2.GetTime(); // get the GPU time required to do "more stuff". This will be 0.0 for the first few frames
 *   double timeGpu3 = gpuTimer3.GetTime(); // this will get inconsistant timing info
 *                                          // the reason for this is that the timer assumes Reset() is only called once a frame and 
 *                                          // a GpuTimer is only used to profile a specific part of the code taking roughly the same
 *                                          // time each frame. GetTime will return a timing value several frames old, where, in this
 *                                          // context a frame is what happened between two calls of Reset().
 *                                          // So, if the timer is reset several times a frame this will result in GetTime randomly 
 *                                          // reporting the time beeing used by "some stuff" and "more stuff"
 *
 *   gpuTimer4.Start();
 *   // so other stuff here
 *   gpuTimer4.Stop();
 *
 *   WaitIdle();                            // This will work just fine, but it stalls the CPU,
 *   double timeGpu4 = gpuTimer4->GetTime();// so you wouldn't want to use it in heavily used
 *   gpuTimer4.Reset();						// or time critical sections of your code

 *   gpuTimer4.Start();
 *   // so weird stuff here
 *   cpuTimer4.Stop();
 *   WaitIdle();
 *   double timeGpu4 = gpuTimer4->GetTime();
 *
 *   delete gpuTimer1;
 * }
 *
 ***********************************************************/


#ifndef _TIMER_H
#define _TIMER_H

//namespace AMD
//{

#define USE_RDTSC 0
#define WATCH_BAD_TS_VAL 0
#define CHECK_DISJOINT   0

#define ENABLE_AMD_TIMER 1

enum TimerType
{
    ttCpu		= 1,
    ttGpu		= 2,
    ttGpuCpu	= 3,
};

//-----------------------------------------------------------------------------

class Timer
{
public:
    Timer();
    virtual ~Timer();

    virtual void Reset( bool bResetSum ) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;

    double GetTime();
	double GetSumTime();
	double GetTimeNumFrames();

protected:
    double			m_LastTime;
	double			m_SumTime;
	unsigned int	m_NumFrames;

    virtual void FinishCollection() {}
};

//-----------------------------------------------------------------------------

class CpuTimer : public Timer
{
public:
    CpuTimer();
    virtual ~CpuTimer();

	virtual void Reset( bool bResetSum );
    virtual void Start();
    virtual void Stop();

    void Delay(double sec);

private:
    LARGE_INTEGER m_startTime;
    double m_freq;

#if USE_RDTSC
    double m_freqRdtsc;
#endif
};

//-----------------------------------------------------------------------------

class GpuTimer : public Timer
{
private:
    struct TsRecord
    {
		union
		{
			struct
			{
				unsigned frameID		:30;
				unsigned startIssued	: 1;
				unsigned stopIssued		: 1;
			} data;
			UINT state;
		};
        ID3D11Query* pStart;
        ID3D11Query* pStop;
		ID3D11Query* pDisjointTS;
    };

public:
    GpuTimer(ID3D11Device* pDev, UINT64 freq = 27000000, UINT numTimeStamps = 8);
    virtual ~GpuTimer();

    virtual void Reset( bool bResetSum );
    virtual void Start();
    virtual void Stop();

	void WaitIdle();

private:

    ID3D11DeviceContext*	m_pDevCtx;
    
	UINT					m_numTimeStamps;
	TsRecord*				m_ts;

    UINT					m_curIssueTs;
	UINT					m_nextRetrTs;
	UINT					m_FrameID;

	struct
	{
		UINT		id		:30;
		UINT		invalid	: 1;
	} m_CurTimeFrame;
	double					m_CurTime;


    virtual void FinishCollection();
    bool CollectData(UINT idx, BOOL stall = FALSE);
};

//-----------------------------------------------------------------------------

class GpuCpuTimer : public CpuTimer
{
public:
    GpuCpuTimer(ID3D11Device* pDev);
    virtual ~GpuCpuTimer();

    virtual void Start();
    virtual void Stop();

private:
    ID3D11DeviceContext* m_pDevCtx;
    ID3D11Query*         m_pEvent;

    void WaitIdle();
};



// TimingEvent:		one timing event managed by TimerEx
// TimerEx:			extended timer singleton to provide instrumentalization similar to PIX
// TimerExHelper:	convenience class to provide easy profiling of function calls
// some MAKROS:		to ease instrumenting your code
class TimingEvent
{
public:
	double			GetTime			( TimerType type, bool stall = false );
	double			GetAvgTime		( TimerType type, bool stall = false );

	TimingEvent*	GetTimer		( LPCWSTR timerId );	// get a child-timer by name
	TimingEvent*	GetParent		( );					// walk through timer tree
	TimingEvent*	GetFirstChild	( );					// walk through timer tree
	TimingEvent*	GetNextTimer	( );					// walk through timer tree

	LPCWSTR			GetName			( );

private:
	// functions only to be used by TimerEx
	friend class TimerEx;
	
	TimingEvent( );
	virtual ~TimingEvent();

	TimingEvent*	GetTimerRec			( LPWSTR timerId );
	void			Reset				( );
	void			Start				( );
	void			Stop				( );

	TimingEvent*	FindLastChildUsed	( );
	void			SetName				( LPCWSTR timerId );

private:
	LPWSTR			m_name;
	unsigned int	m_nameLen;

	CpuTimer		m_cpu;
	GpuTimer*		m_gpu;
	bool			m_used;

	TimingEvent*	m_parent;
	TimingEvent*	m_firstChild;
	TimingEvent*	m_next;
};

class TimerEx
{
public:
	static TimerEx&	Instance()
	{
		static TimerEx inst;
		return inst;
	}

	ID3D11Device*	GetDevice()
	{
		return m_pDev;
	}

	void			Init			( ID3D11Device* pDev );		// to be called before any timing is done
	void			Destroy         ( );						// to be called when the ID3D11Device* gets destroyed
	void			Reset			( bool bResetSum );			// to be called one a frame, preferably on frame switch (flip)
	void			Start			( LPCWSTR timerId );		// looks for the child in the tree structure, if not found adds another child
	void			Stop			( );
	double			GetTime			( TimerType type, LPCWSTR timerId, bool stall = false );
	double			GetAvgTime		( TimerType type, LPCWSTR timerId, bool stall = false );
	TimingEvent*	GetTimer		( LPCWSTR timerId = NULL );	// returns the first child of root if NULL, else searches childnodes for timer with that name
	
private:
	TimerEx				( );
	virtual ~TimerEx	( );

	void Reset			( TimingEvent* te, bool bResetSum );
	void DeleteTimerTree( TimingEvent* te );

protected:
	ID3D11Device*	m_pDev;
	TimingEvent*	m_Root;		// timer tree
	TimingEvent*	m_Current;	// current position in timer tree
	TimingEvent*	m_Unused;	// unused timers (for faster reuse)
};

#if ENABLE_AMD_TIMER
#define TIMER_Init( device )						\
	TimerEx::Instance( ).Init( device );

#define TIMER_Destroy( )							\
	TimerEx::Instance( ).Destroy();

#define TIMER_Reset( )								\
	TimerEx::Instance( ).Reset( false );

#define TIMER_FullReset( )							\
	TimerEx::Instance( ).Reset( true );

#define TIMER_GetTime( Cpu_Gpu, name )				\
	TimerEx::Instance( ).GetTime( tt##Cpu_Gpu, name )

#define TIMER_WaitForGpuAndGetTime( name )			\
	TimerEx::Instance( ).GetTime( ttGpu, name, true )

#define TIMER_GetAvgTime( Cpu_Gpu, name )				\
	TimerEx::Instance( ).GetAvgTime( tt##Cpu_Gpu, name )

// makros, analogue to PIX
#define TIMER_Begin( col, name )					\
	TimerEx::Instance( ).Start( name );				
//	DXUT_BeginPerfEvent( col, name );
//	D3DPERF_BeginEvent( col, name );

#define TIMER_End( )								\
		TimerEx::Instance( ).Stop( );				
//		DXUT_EndPerfEvent( );
//		D3DPERF_EndEvent( );
#else
#define TIMER_Init( device )
#define TIMER_Destroy( )
#define TIMER_Reset( )
#define TIMER_GetTime( Cpu_Gpu, name )          0
#define TIMER_WaitForGpuAndGetTime( name )      0
#define TIMER_GetAvgTime( Cpu_Gpu, name )       0
#define TIMER_Begin( col, name )
#define TIMER_End( )
#endif

class TimerExHelper
{
public:
	TimerExHelper( unsigned int col, LPCWSTR name )
	{
		(void)&col;
		TIMER_Begin( col, name );
	}
	virtual ~TimerExHelper( )
	{
		TIMER_End( );
	}
};

#if ENABLE_AMD_TIMER
#define TIMER_ProfileCodeBlock( col, name )			\
	TimerExHelper __codeblock_timer( col, name );
#else
#define TIMER_ProfileCodeBlock( col, name )
#endif
//}; // namespace AMD

#endif // _TIMER_H
