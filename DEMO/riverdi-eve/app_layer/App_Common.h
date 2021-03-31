/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE,
DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON
BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION
IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS
THE RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND
SUBSEQUENT WHQL RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : BRIDGETEK

Revision History:
0.1 - date 2017.02.23 - Initial version
*/

#ifndef _APP_COMMON_H_
#define _APP_COMMON_H_

#include "Gpu_Hal.h"
#include <stdint.h>

/******************************************************************************/

extern Gpu_Hal_Context_t host;
extern Gpu_Hal_Context_t * phost;

/******************************************************************************/

void
App_WrCoCmd_Buffer( Gpu_Hal_Context_t * phost, uint32_t cmd );

void
App_Flush_Co_Buffer( Gpu_Hal_Context_t * phost );

void
App_Common_Init( Gpu_Hal_Context_t * phost );

void
App_Calibrate_Screen( Gpu_Hal_Context_t * phost );

void
App_Sketch( Gpu_Hal_Context_t * phost );

void
App_Set_Background( Gpu_Hal_Context_t * phost, uint8_t r, uint8_t g,
                    uint8_t b );

void
App_DisplayImage( Gpu_Hal_Context_t * phost, uint16_t img_w, uint16_t img_h,
                  uint16_t dw, uint16_t dh );

void
App_Show_Logo( Gpu_Hal_Context_t * phost );

#if 0
void App_WrDl_Buffer( Gpu_Hal_Context_t * phost, uint32_t cmd );
void App_WrCoStr_Buffer(Gpu_Hal_Context_t *phost,const char *s);
void App_Flush_DL_Buffer(Gpu_Hal_Context_t *phost);
void App_Flush_Co_Buffer_nowait(Gpu_Hal_Context_t *phost);
void App_Set_DlBuffer_Index(uint32_t index);
void App_Set_CmdBuffer_Index(uint32_t index);



/******************************************************************************/

uint8_t App_Read_Tag(Gpu_Hal_Context_t *phost);
uint8_t App_Touch_Update(Gpu_Hal_Context_t *phost, uint8_t *currTag, uint16_t *x, uint16_t *y);
void App_Play_Sound(Gpu_Hal_Context_t *phost, uint8_t sound,uint8_t vol,uint8_t midi);

void App_Common_Close(Gpu_Hal_Context_t *phost);

#endif

/******************************************************************************/

#endif /* _APP_COMMON_H_ */
