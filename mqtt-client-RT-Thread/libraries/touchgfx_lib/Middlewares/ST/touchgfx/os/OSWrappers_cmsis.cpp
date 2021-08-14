
#include <touchgfx/hal/OSWrappers.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <touchgfx/hal/HAL.hpp>

#include <assert.h>
#include <cmsis_os.h>

using namespace touchgfx;

static osSemaphoreId frame_buffer_sem = 0;
static osMessageQId vsync_queue = 0;

// Just a dummy value to insert in the VSYNC queue.
static uint32_t dummy = 0x5a;

void OSWrappers::initialize()
{
    // Create a queue of length 1
    osSemaphoreDef(frame_buffer_sem);
    frame_buffer_sem = osSemaphoreCreate(osSemaphore(frame_buffer_sem), 1); // Binary semaphore
    osSemaphoreWait(frame_buffer_sem, osWaitForever); // take the lock

    // Create a queue of length 1
    osMessageQDef(vsync_queue, 1, uint32_t);
    vsync_queue = osMessageCreate(osMessageQ(vsync_queue),NULL);
}

void OSWrappers::takeFrameBufferSemaphore()
{
    assert(frame_buffer_sem);
    osSemaphoreWait(frame_buffer_sem, osWaitForever);
}

void OSWrappers::giveFrameBufferSemaphore()
{
    assert(frame_buffer_sem);
    osSemaphoreRelease(frame_buffer_sem);
}

void OSWrappers::tryTakeFrameBufferSemaphore()
{
    assert(frame_buffer_sem);
    osSemaphoreWait(frame_buffer_sem, 0);
}

void OSWrappers::giveFrameBufferSemaphoreFromISR()
{
    assert(frame_buffer_sem);
    osSemaphoreRelease(frame_buffer_sem);
}

void OSWrappers::signalVSync()
{
    if (vsync_queue)
    {
        osMessagePut(vsync_queue, dummy, 0);
    }
}

void OSWrappers::waitForVSync()
{
    if (vsync_queue)
    {
        // First make sure the queue is empty, by trying to remove an element with 0 timeout.
        osMessageGet(vsync_queue, 0);

        // Then, wait for next VSYNC to occur.
        osMessageGet(vsync_queue, osWaitForever);
    }
}

void OSWrappers::taskDelay(uint16_t ms)
{
    osDelay(static_cast<uint32_t>(ms));
}


// NOTE:
// The remainder of this file is FreeRTOS-specific. If using a different OS,
// you can just remove all the following code, as it is optional.
// However, if MCU load percentage readout is required, you need to find a way
// to inform TouchGFX of when the idle task is switched in/out and call the
// setMCUActive function accordingly (see below).

//FreeRTOS hook function being called when idle task is switched in or out.
static portBASE_TYPE IdleTaskHook(void* p)
{
    if ((int)p) //idle task sched out
    {
        touchgfx::HAL::getInstance()->setMCUActive(true);
    }
    else //idle task sched in
    {
        touchgfx::HAL::getInstance()->setMCUActive(false);
    }
    return pdTRUE;
}

extern "C"
{
    void vApplicationStackOverflowHook(xTaskHandle xTask,
                                       signed portCHAR* pcTaskName)
    {
        while (1);
    }

    void vApplicationMallocFailedHook(xTaskHandle xTask,
                                      signed portCHAR* pcTaskName)
    {
        while (1);
    }

    void vApplicationIdleHook(void)
    {
        // Set task tag in order to have the "IdleTaskHook" function called when the idle task is
        // switched in/out. Used solely for measuring MCU load, and can be removed if MCU load
        // readout is not needed.
        vTaskSetApplicationTaskTag(NULL, IdleTaskHook);
    }
}
