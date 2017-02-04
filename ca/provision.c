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

#include <stdio.h>
#include <memory.h>
#include <err.h>
#include <tee_client_api.h>
#include <drm_ta_api.h>
#include <ta_provision.h>

static uint8_t keybox[128] = {
    // deviceID
    0x54, 0x65, 0x73, 0x74, 0x4b, 0x65, 0x79, 0x30, // TestKey02
    0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
    // key
    0x76, 0x5d, 0xce, 0x01, 0x04, 0x89, 0xb3, 0xd0,
    0xdf, 0xce, 0x54, 0x8a, 0x49, 0xda, 0xdc, 0xb6,
    // data
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x19,
    0x92, 0x27, 0x0b, 0x1f, 0x1a, 0xd5, 0xc6, 0x93,
    0x19, 0x3f, 0xaa, 0x74, 0x1f, 0xdd, 0x5f, 0xb4,
    0xe9, 0x40, 0x2f, 0x34, 0xa4, 0x92, 0xf4, 0xae,
    0x9a, 0x52, 0x39, 0xbc, 0xb7, 0x24, 0x38, 0x13,
    0xab, 0xf4, 0x92, 0x96, 0xc4, 0x81, 0x60, 0x33,
    0xd8, 0xb8, 0x09, 0xc7, 0x55, 0x0e, 0x12, 0xfa,
    0xa8, 0x98, 0x62, 0x8a, 0xec, 0xea, 0x74, 0x8a,
    0x4b, 0xfa, 0x5a, 0x9e, 0xb6, 0x49, 0x0d, 0x80,
    // magic
    0x6b, 0x62, 0x6f, 0x78,
    // Crc
    0x2a, 0x3b, 0x3e, 0xe4,
};

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_PROVISION_UUID;
	uint32_t err_origin;

	(void)argc;
	(void)argv;

	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS) {
		printf("TEEC_InitializeContext failed with code 0x%x", res);
		goto exit_1;
	}

	printf("[Provision CA] start\n");
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
		printf("TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
		goto exit_2;
	}

	printf("[Provision CA] Widevine test\n");
	memset(&op, 0, sizeof(TEEC_Operation));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						TEEC_MEMREF_TEMP_INPUT,
						TEEC_NONE,
						TEEC_NONE);
	op.params[0].value.a = TA_WIDEVINE_SUBCMD_STORE_KEYBOX;
	op.params[1].tmpref.buffer = (void *)keybox;
	op.params[1].tmpref.size = 128;

	res = TEEC_InvokeCommand(&sess, TA_PROVISION_CMD_STORE_WVKEY, &op,
			&err_origin);
	if (res != TEEC_SUCCESS) {
		printf("[Provision CA] TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
		goto exit_3;
	}

	printf("[Provision CA] PlayReady test\n");
	memset(&op, 0, sizeof(TEEC_Operation));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
						TEEC_MEMREF_TEMP_INPUT,
						TEEC_NONE,
						TEEC_NONE);
	op.params[0].value.a = TA_PLAYREADY_SUBCMD_STORE_MODEL_PUBLIC_KEY;
	op.params[1].tmpref.buffer = (void *)keybox;
	op.params[1].tmpref.size = 128;

	res = TEEC_InvokeCommand(&sess, TA_PROVISION_CMD_STORE_PRKEY, &op,
			&err_origin);
	if (res != TEEC_SUCCESS) {
		printf("[Provision CA] TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
		goto exit_3;
	}

exit_3:
	TEEC_CloseSession(&sess);
exit_2:
	TEEC_FinalizeContext(&ctx);
exit_1:
	printf("[Provision CA] done\n");

	return 0;
}
