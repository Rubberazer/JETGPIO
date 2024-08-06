/* SPDX-License-Identifier: GPL-2.0
 * 
 * jetclocks.h - jetclocks kernel module header
 *
 * Based on the NVIDIA MODS kernel driver by NVIDIA CORPORATION. 
 * The jetclocks kernel module is a facility that provides a friendly API
 * for applications in user space to manipulate the clocks in Nvidia
 * Jetson Orin machines.
 *
 */

#ifndef __JETCLOCKS_H__
#define __JETCLOCKS_H__

#include <linux/ioctl.h>

struct jetclk {
    int clk_enabled;
    unsigned long clk_rate;
    unsigned long clk_set_rate;
    char clk[30];
};

#define CLK_ENABLE		_IOW('{', 1, struct jetclk *)
#define CLK_DISABLE		_IOW('{', 2, struct jetclk *)
#define CLK_IS_ENABLED          _IOWR('{', 3, struct jetclk *)
#define CLK_SET_RATE            _IOW('{', 4, struct jetclk *)
#define CLK_GET_RATE            _IOWR('{', 5, struct jetclk *)

#endif /* __JETCLOCKS_H__ */
