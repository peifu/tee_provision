/*
 * Copyright (C) 2016 Amlogic, Inc. All rights reserved.
 *
 * All information contained herein is Amlogic confidential.
 *
 * This software is provided to you pursuant to Software License
 * Agreement (SLA) with Amlogic Inc ("Amlogic"). This software may be
 * used only in accordance with the terms of this agreement.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification is strictly prohibited without prior written permission
 * from Amlogic.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRM_TA_API_H_
#define _DRM_TA_API_H_

/* WideVine TA UUID */
#define TA_WIDEVINE_UUID { 0xe043cde0, 0x61d0, 0x11e5, \
	{ 0x9c, 0x26, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

/* The Command ID implemented in WideVine TA */
#define TA_WIDEVINE_CMD_STORE_KEY                     0x44
/* store widevine keybox */
#define TA_WIDEVINE_SUBCMD_STORE_KEYBOX               0x1


/* PlayReady TA UUID */
#define TA_PLAYREADY_UUID { 0x9a04f079, 0x9840, 0x4286, \
	{ 0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95} }

/* The Command ID implemented in PlayReady TA */
#define TA_PLAYREADY_CMD_STORE_KEY                    0x1F
/* store playready model public key */
#define TA_PLAYREADY_SUBCMD_STORE_MODEL_PUBLIC_KEY    0x1
/* store playready model private key */
#define TA_PLAYREADY_SUBCMD_STORE_MODEL_PRIVATE_KEY   0x2

#endif
