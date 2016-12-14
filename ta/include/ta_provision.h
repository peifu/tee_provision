/*
 * Copyright (c) 2014, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TA_PROVISION_H
#define TA_PROVISION_H

/* This UUID is generated with Linux uuidgen */
#define TA_PROVISION_UUID { 0x83186819, 0x896e, 0x42cc, \
		{ 0xae, 0xe9, 0x17, 0xa2, 0x69, 0xdd, 0xac, 0x0b} }

/* The TAFs ID implemented in this TA */
#define TA_PROVISION_CMD_TEST 0

/* Hello world TA UUID */
#define TA_HELLO_WORLD_UUID { 0x8aaaf200, 0x2450, 0x11e4, \
		{ 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

#define TA_WVCENC_UUID { 0xe043cde0, 0x61d0, 0x11e5, \
		{ 0x9c, 0x26, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

/* The TAFs ID implemented in Hello world TA */
#define TA_HELLO_WORLD_CMD_INC_VALUE	0
#define TA_HELLO_WORLD_CMD_WRITE_KEY	1
#define TA_HELLO_WORLD_CMD_READ_KEY		2


/* The TAFs ID implemented in Widevine TA */
#define TA_WVCENC_CMD_INITIALIZE
#define  TA_WVCENC_CMD_PRIVATE                              64
#define  TA_WVCENC_CMD_PROVISION_STORE_KEY                  TA_WVCENC_CMD_PRIVATE + 4

#endif
