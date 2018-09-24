/******************************************************************************
 *
 * Copyright (C) 2018 Xilinx, Inc.	All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
******************************************************************************/
/*****************************************************************************/
/**
 *
 * @file xv_scenechange_l2.c
 * @addtogroup v_scenechange_v1_0
 * @{
 *
 * The SceneChange Layer-2 Driver.
 * The functions in this file provides an abstraction from the register peek/poke
 * methodology by implementing most common use-case provided by the core.
 *
 * ******************************************************************************/

#include "xv_scenechange.h"
#include "xv_scenechange_hw.h"

static void XV_scenechange_layer_height(XV_scenechange *InstancePtr, u32 Data,
		u8 streamid)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_HEIGHT_0_DATA),
			Data);
}

static void XV_scenechange_layer_set_width(XV_scenechange *InstancePtr, u32 Data,
		u8 streamid)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_WIDTH_0_DATA),
			Data);
}

static void XV_scenechange_layer_set_stride(XV_scenechange *InstancePtr, u32 Data,
		u8 streamid)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_STRIDE_0_DATA),
			Data);
}

static void XV_scenechange_layer_set_vfmt(XV_scenechange *InstancePtr, u32 Data,
		u8 streamid)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_VIDEO_FORMAT_0_DATA),
			Data);
}

static void XV_scenechange_layer_set_subsample(XV_scenechange *InstancePtr, u32 Data,
		u8 streamid)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_SUBSAMPLE_0_DATA),
			Data);
}

static void XV_scenechange_layer_set_bufaddr(XV_scenechange *InstancePtr, u64 Data,
		u8 streamid)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_FRM_BUFFER0_V_DATA),
			(u32) Data);

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			(((streamid * XV_SCD_LAYER_OFFSET) +
			 XV_SCENECHANGE_CTRL_ADDR_HWREG_FRM_BUFFER0_V_DATA) + 4),
			(u32) (Data >> 32));
}

int XV_scenechange_Layer_config(XV_scenechange *InstancePtr, u8 layerid)
{
	u16 subsample = (1 << InstancePtr->scd_config->histogram_bits);

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	if (InstancePtr->glconfig[layerid].layerid >
			InstancePtr->scd_config->num_streams)
		return XST_FAILURE;

	if (InstancePtr->glconfig[layerid].width >
			InstancePtr->scd_config->cols)
		return XST_FAILURE;

	if (InstancePtr->glconfig[layerid].height >
			InstancePtr->scd_config->rows)
		return XST_FAILURE;

	if (InstancePtr->glconfig[layerid].subsample > subsample)
		return XST_FAILURE;

	if (!((InstancePtr->scd_config->has_y8 &&
			(InstancePtr->glconfig[layerid].
			video_format == XV_SCD_HAS_Y8)) ||
			(InstancePtr->scd_config->has_y10 &&
			(InstancePtr->glconfig[layerid].
			video_format == XV_SCD_HAS_Y10))))
		return XST_FAILURE;

	XV_scenechange_layer_set_width(InstancePtr,
			InstancePtr->glconfig[layerid].width, layerid);
	XV_scenechange_layer_height(InstancePtr,
			InstancePtr->glconfig[layerid].height, layerid);
	XV_scenechange_layer_set_stride(InstancePtr,
			InstancePtr->glconfig[layerid].stride, layerid);
	XV_scenechange_layer_set_vfmt(InstancePtr,
			InstancePtr->glconfig[layerid].video_format, layerid);
	XV_scenechange_layer_set_subsample(InstancePtr,
			InstancePtr->glconfig[layerid].subsample, layerid);
	XV_scenechange_layer_set_bufaddr(InstancePtr,
			InstancePtr->glconfig[layerid].buffer, layerid);

	return XST_SUCCESS;
}

void XV_scenechange_Layer_stream_enable(XV_scenechange *InstancePtr,
		u32 streams)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	XV_scenechange_Set_HwReg_stream_enable(InstancePtr, streams);
}

static void XV_scenechange_SetFlushbit(XV_scenechange *InstancePtr) {
	u32 Data;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	Data = XV_scenechange_ReadReg(InstancePtr->Ctrl_BaseAddress,
			XV_SCENECHANGE_CTRL_ADDR_AP_CTRL);
	Data |= XV_SCD_CTRL_FLUSH_DONE_BIT_MASK;

	XV_scenechange_WriteReg(InstancePtr->Ctrl_BaseAddress,
			XV_SCENECHANGE_CTRL_ADDR_AP_CTRL, Data);
}

static u32 XV_scenechange_Get_FlushDone(XV_scenechange *InstancePtr) {
	u32 Data;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	Data = XV_scenechange_ReadReg(InstancePtr->Ctrl_BaseAddress,
			XV_SCENECHANGE_CTRL_ADDR_AP_CTRL) &
		XV_SCD_CTRL_FLUSH_DONE_BIT_MASK;
	return Data;
}

u32 XV_scenechange_Stop(XV_scenechange *InstancePtr)
{
	int Status = XST_FAILURE;
	u32 isIdle = 0;
	u32 cnt = 0;
	u32 Data = 0;

	XV_scenechange_InterruptGlobalDisable(InstancePtr);
	XV_scenechange_InterruptDisable(InstancePtr,
			XV_SCENECHANGE_CTRL_ADDR_ISR_AP_DONE);
	XV_scenechange_DisableAutoRestart(InstancePtr);
	XV_scenechange_SetFlushbit(InstancePtr);

	if (InstancePtr->scd_config->memory_based) {
		do {
			Data = XV_scenechange_Get_FlushDone(InstancePtr);
			usleep(XV_SCD_WAIT_FOR_FLUSH_DELAY);
			cnt++;
		} while((Data == 0) && (cnt < XV_SCD_WAIT_FOR_FLUSH_DONE));

		if (Data == 0)
			return XST_FAILURE;
	}
	/* Wait for idle */
	do {
		isIdle = XV_scenechange_IsIdle(InstancePtr);
		cnt++;
	} while((isIdle!=1) && (cnt < XV_SCD_IDLE_TIMEOUT));

	if (isIdle == 1 ) {
		Status = XST_SUCCESS;
	}

	return Status;
}
void XV_scenechange_EnableInterrupts(void *InstancePtr)
{
	XV_scenechange_InterruptEnable(InstancePtr,
			XV_SCENECHANGE_CTRL_ADDR_ISR_AP_DONE);
}