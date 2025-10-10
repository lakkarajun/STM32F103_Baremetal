/*
 * sys_tick.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 *
 *   Note: Calculate SysTick period
 *   The reload value determines how many clock ticks per interrupt cycle:
 *   𝑇SysTick = (SYST_RVR + 1) / 𝑓SysTick
 *   If SYST_RVR = 36000 - 1 = 35999, and f = 72 MHz:
 *   𝑇 = 36000 / 72,000,000 i.e.  0.0005 seconds = 0.5 ms
 */


#define SYSTICK_BASE     0xE000E010UL
#define SYST_CSR         (*(volatile unsigned long *)(SYSTICK_BASE + 0x0))
#define SYST_RVR         (*(volatile unsigned long *)(SYSTICK_BASE + 0x4))
#define SYST_CVR         (*(volatile unsigned long *)(SYSTICK_BASE + 0x8))

/* SysTick Control / Status bits */
#define SYST_CSR_ENABLE    (1 << 0)   // Counter enable
#define SYST_CSR_TICKINT   (1 << 1)   // Interrupt enable
#define SYST_CSR_CLKSOURCE (1 << 2)   // Clock source = processor clock (72MHz)

void SysTick_Init(void)
{
//	SYST_RVR = 144000 - 1;                      // Reload value for 2 ms
//	SYST_RVR = 72000 - 1;                       // Reload value for 1 ms
    SYST_RVR = 36000 - 1;                       // Reload value for 0.5 ms

    SYST_CVR = 0;                               // Clear current value
    SYST_CSR = SYST_CSR_CLKSOURCE |             // Use CPU clock
               SYST_CSR_TICKINT  |              // Enable interrupt
               SYST_CSR_ENABLE;                 // Enable counter
}
