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

#include "drm_ta_api.h"
#include "ta_provision.h"


static TEE_Result provision_dump_prop(void);
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

	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */

	provision_dump_prop();
}

#if 1
static TEE_Result provision_dump_prop(void)
{
	TEE_Result res;
	TEE_PropSetHandle h;
	TEE_UUID uuid;
	TEE_UUID device_uuid;
	TEE_Identity id;
	const char *device_id = "gpd.tee.deviceID";
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

	res = TEE_GetPropertyAsUUID(h, (void *)device_id, &device_uuid);
	IMSG("\n********* Device ID : %pUl\n\n", (void *)&device_uuid);

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
#endif

static TEE_Result provision_ipc_widevine(uint32_t subcmd, uint8_t *buf, uint32_t buflen)
{
	TEE_Result res = TEE_SUCCESS;
	uint32_t ret_orig;
	uint32_t l_pts;
	TEE_TASessionHandle sess = TEE_HANDLE_NULL;
	static const TEE_UUID uuid = TA_WIDEVINE_UUID;
	TEE_Param l_params[4] = { { {0} } };

	res = TEE_OpenTASession(&uuid, 0, 0, NULL, &sess, &ret_orig);
	if (res != TEE_SUCCESS) {
		EMSG("WV TEE_OpenTASession failed res %x\n", res);
		return TEE_ERROR_ACCESS_DENIED;
	}

	l_pts = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
					TEE_PARAM_TYPE_MEMREF_INPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE);
	l_params[0].value.a = subcmd;
	l_params[1].memref.buffer = buf;
	l_params[1].memref.size = buflen;
	res = TEE_InvokeTACommand(sess, 0, TA_WIDEVINE_CMD_STORE_KEY,
								  l_pts, l_params, &ret_orig);
	if (res != TEE_SUCCESS) {
		EMSG("WV TEE_InvokeTACommand failed res %x\n", res);
	}

	TEE_CloseTASession(sess);
	return res;
}

static TEE_Result provision_ipc_playready(uint32_t subcmd, uint8_t *buf, uint32_t buflen)
{
	TEE_Result res = TEE_SUCCESS;
	uint32_t ret_orig;
	uint32_t l_pts;
	TEE_TASessionHandle sess = TEE_HANDLE_NULL;
	static const TEE_UUID uuid = TA_PLAYREADY_UUID;
	TEE_Param l_params[4] = { { {0} } };

	res = TEE_OpenTASession(&uuid, 0, 0, NULL, &sess, &ret_orig);
	if (res != TEE_SUCCESS) {
		EMSG("PR TEE_OpenTASession failed res %x\n", res);
		return TEE_ERROR_ACCESS_DENIED;
	}

	l_pts = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
					TEE_PARAM_TYPE_MEMREF_INPUT,
					TEE_PARAM_TYPE_NONE,
					TEE_PARAM_TYPE_NONE);
	l_params[0].value.a = subcmd;
	l_params[1].memref.buffer = buf;
	l_params[1].memref.size = buflen;
	res = TEE_InvokeTACommand(sess, 0, TA_PLAYREADY_CMD_STORE_KEY,
								  l_pts, l_params, &ret_orig);
	if (res != TEE_SUCCESS) {
		EMSG("PR TEE_InvokeTACommand failed res %x\n", res);
	}

	TEE_CloseTASession(sess);
	return res;
}

static TEE_Result provision_process_playready(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res = TEE_SUCCESS;
	uint32_t subcmd = 0;
	uint8_t *buf = NULL;
	uint32_t buflen = 0;
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	if (param_types != exp_param_types) {
		EMSG("param error!\n");
		return TEE_ERROR_BAD_PARAMETERS;
	}
	subcmd = params[0].value.a;
	buf = params[1].memref.buffer;
	buflen = params[1].memref.size;

	res = provision_ipc_playready(subcmd, buf, buflen);

	return res;
}

static TEE_Result provision_process_widevine(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res = TEE_SUCCESS;
	uint32_t subcmd = 0;
	uint8_t *buf = NULL;
	uint32_t buflen = 0;
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
						   TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	if (param_types != exp_param_types) {
		return TEE_ERROR_BAD_PARAMETERS;
	}
	subcmd = params[0].value.a;
	buf = params[1].memref.buffer;
	buflen = params[1].memref.size;

	res = provision_ipc_widevine(subcmd, buf, buflen);

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
	case TA_PROVISION_CMD_STORE_WVKEY:
		return provision_process_widevine(param_types, params);
	case TA_PROVISION_CMD_STORE_PRKEY:
		return provision_process_playready(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
