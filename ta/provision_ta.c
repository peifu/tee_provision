/*
 * Copyright (c) 2015, Linaro Limited
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

#define STR_TRACE_USER_TA "PROVISION"

#include <string.h>

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include "ta_provision.h"

static TEE_UUID helloworld_id = TA_HELLO_WORLD_UUID;

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param  params[4], void **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	IMSG("Provision open!\n");
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Provision close!\n");
}

static TEE_Result provision_dump_prop(void)
{
	TEE_Result res;
	TEE_PropSetHandle h;
	TEE_UUID uuid;
	TEE_Identity id;
	const char *uuid_name = "gpd.ta.appID";
	const char *identity_name = "gpd.client.identity";
	char namebuf[64] = { 0 };
	uint32_t namebuflen = 64;
	uint32_t count = 1;

	IMSG("@@@@ property\n");
	res = TEE_AllocatePropertyEnumerator(&h);
	if (res != TEE_SUCCESS) {
		EMSG("TEE_AllocatePropertyEnumerator: returned 0x%x\n",
				(unsigned int)res);
		return TEE_ERROR_GENERIC;
	}
	IMSG("@@1. TEE_PROPSET_TEE_IMPLEMENTATION\n");
	TEE_StartPropertyEnumerator(h, TEE_PROPSET_TEE_IMPLEMENTATION);
	while (res == TEE_SUCCESS) {
		namebuflen = 64;
		res = TEE_GetPropertyName(h, (void *)namebuf, &namebuflen);
		IMSG(" %02d %s(%d)\n", count++, namebuf, namebuflen);
		res = TEE_GetNextProperty(h);
	}

	IMSG("@@2. TEE_PROPSET_CURRENT_CLIENT\n");
	TEE_StartPropertyEnumerator(h, TEE_PROPSET_CURRENT_CLIENT);
	count = 1;
	res = TEE_SUCCESS;
	while (res == TEE_SUCCESS) {
		namebuflen = 64;
		res = TEE_GetPropertyName(h, (void *)namebuf, &namebuflen);
		IMSG(" %02d %s(%d)\n", count++, namebuf, namebuflen);
		if (strcmp(identity_name, namebuf) == 0) {
			res = TEE_GetPropertyAsIdentity(h, NULL, &id);
			if (res == TEE_SUCCESS) {
				IMSG(" Client Identity->login %d\n", id.login);
				IMSG(" Client Identity->uuid  %pUl\n", (void *)&(id.uuid));
			}
		}
		res = TEE_GetNextProperty(h);
	}

	IMSG("@@3. TEE_PROPSET_CURRENT_TA\n");
	TEE_StartPropertyEnumerator(h, TEE_PROPSET_CURRENT_TA);
	count = 1;
	res = TEE_SUCCESS;
	while (res == TEE_SUCCESS) {
		namebuflen = 64;
		res = TEE_GetPropertyName(h, (void *)namebuf, &namebuflen);
		IMSG(" %02d %s(%d)\n", count++, namebuf, namebuflen);
		if (strcmp(uuid_name, namebuf) == 0) {
			res = TEE_GetPropertyAsUUID(h, NULL, &uuid);
			if (res == TEE_SUCCESS) {
				IMSG(" TA uuid  %pUl\n", (void *)&uuid);
			}
		}
		res = TEE_GetNextProperty(h);
	}

	IMSG("@@@@ property ok\n");

	return res;
}

static TEE_Result provision_ipc_helloworld(void)
{
	TEE_TASessionHandle ipc_session;
	TEE_Result res;
	uint32_t origin;
	uint32_t cmd;
	TEE_Param params[4];
	uint32_t param_types = 0;
	const char *key = "provision";

	res = TEE_OpenTASession(&helloworld_id, 0, 0, NULL, &ipc_session,
				&origin);
	if (res != TEE_SUCCESS) {
		EMSG("rpc_haha - TEE_OpenTASession returned 0x%x\n",
		     (unsigned int)res);
		return res;
	}

	param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	TEE_MemFill(params, 0, sizeof(TEE_Param) * 4);
	params[0].value.a = 8;
	params[0].value.b = 9;
	params[1].memref.buffer = (void *)key;
	params[1].memref.size = strlen(key) + 1;
	IMSG("Invoke: value.a=%d\n", params[0].value.a);
	IMSG("Invoke: value.b=%d\n", params[0].value.b);
	IMSG("Invoke: key=%s\n", (char *)params[2].memref.buffer);
	cmd = TA_HELLO_WORLD_CMD_WRITE_KEY;
	res = TEE_InvokeTACommand(ipc_session, 0, cmd, param_types,
				  params, &origin);

	if (res != TEE_SUCCESS) {
		EMSG("rpc_haha - TEE_InvokeTACommand returned 0x%x\n",
		     (unsigned int)res);
	}

	TEE_CloseTASession(ipc_session);

	return res;
}

static char kKeybox[128] = {
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

static TEE_Result provision_ipc_widevine(void)
{

	TEE_Result res;
	uint32_t ret_orig;
	uint32_t l_pts;
	TEE_TASessionHandle sess = TEE_HANDLE_NULL;
	static const TEE_UUID wvcenc_uuid = TA_WVCENC_UUID;
	TEE_Param l_params[4] = { { {0} } };

	IMSG("cmd_test_provison_store_key>>>>>>>>>>>");
	res = TEE_OpenTASession(&wvcenc_uuid, 0, 0, NULL, &sess, &ret_orig);
	if (res != TEE_SUCCESS) {
		EMSG("cmd_test_provison_store_key: TEE_OpenTASession failed res %d\n", res);
		return TEE_ERROR_ACCESS_DENIED;
	}

	l_pts = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
					TEE_PARAM_TYPE_VALUE_OUTPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE);
	l_params[0].memref.buffer = kKeybox;
	l_params[0].memref.size = sizeof(kKeybox);
	res = TEE_InvokeTACommand(sess, 0, TA_WVCENC_CMD_PROVISION_STORE_KEY,
								  l_pts, l_params, &ret_orig);
	if (res != TEE_SUCCESS) {
		EMSG("cmd_test_provison_store_key: TEE_InvokeTACommand failed res %d\n", res);
	}
	EMSG("l_params[2].value.a %d\n", l_params[2].value.a);

	TEE_CloseTASession(sess);
	return TEE_SUCCESS;
}

static TEE_Result provision_test(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res = TEE_SUCCESS;
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	if (param_types != exp_param_types) {
		return TEE_ERROR_BAD_PARAMETERS;
	}
	IMSG("Invoke: value.a=%d\n", params[0].value.a);
	IMSG("Invoke: value.b=%d\n", params[0].value.b);
	params[0].value.a = 10;
	params[0].value.b = 20;
	IMSG("Invoke: memref.buffer=%p\n", params[1].memref.buffer);
	IMSG("Invoke: memref.buffer=%s\n", (char *)params[1].memref.buffer);

	res = provision_dump_prop();
	if (res != TEE_SUCCESS) {
		EMSG("@@@@ property dump\n");
	}

	IMSG("@@@@ invoking helloworld\n");
	res = provision_ipc_helloworld();

	IMSG("@@@@ invoking widevine\n");
	res = provision_ipc_widevine();

	return res;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_PROVISION_CMD_TEST:
		return provision_test(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
