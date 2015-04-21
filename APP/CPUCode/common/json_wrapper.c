/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#include <json/json.h>
#include <stdint.h>
#include <string.h>
#include "json_wrapper.h"
#include "logging.h"
#include "sha2.h"

static void hex_to_bytes(
        const char *str,
        uint8_t *bytes,
        size_t size)
{
    char *str_cpy = strdup(str);
    char *ptr = str_cpy + (size - 1) * 2;
    for (size_t i = size; i > 0; i--) {
        bytes[i-1] = strtoul(ptr, NULL, 16);
        *ptr = '\0';
        ptr -= 2;
    }
    free(str_cpy);
}

static json_object *get_json_object(json_object *obj, const char *key) {
	json_object *ret = json_object_object_get(obj, key);
	if (NULL == ret) {
		LOG_FATAL("JSON parse failure");
	}
	return ret;
}


/**
 * \brief   Create JSON string for the getwork command
 * \param   id  Id of the request
 * \return  The JSON string. The returned string must be deallocated by the caller.
 */
char * create_json_getwork(uint32_t id) {
	json_object *json = json_object_new_object();
	if (json == NULL) {
		LOG_FATAL("json_object_new_object failed");
	}

	json_object_object_add(json, "jsonrpc", json_object_new_string("1.0"));
	json_object_object_add(json, "id", json_object_new_int(id));
	json_object_object_add(json, "method", json_object_new_string("getwork"));
	json_object_object_add(json, "params", json_object_new_array());
	const char *json_str = json_object_get_string(json);
	char *request = strdup(json_str);
	json_object_put(json);
	return request;
}


/**
 * \brief   Create JSON string for the 'getwork data' command
 * \param   work    Work fetched from previous getwork command
 * \param   nonce   Nonce that satisfies the target
 * \return  The JSON string.
 *          The returned string must be deallocated by the caller.
 */
char * create_json_getwork_completion(const bc_work_t *work, uint32_t nonce) {
	json_object *json = json_object_new_object();
	json_object_object_add(json, "jsonrpc", json_object_new_string("1.0"));
	json_object_object_add(json, "id", json_object_new_int(work->id));
	json_object_object_add(json, "method", json_object_new_string("getwork"));
	json_object *params = json_object_new_array();
	char data[257];
	strncpy(data, work->result.data_str, 257);
	char nonce_str[9];
	sprintf(nonce_str, "%08x", nonce);
	LOG_DEBUG("submit nonce hex: %s", nonce_str);
	memcpy(data + 152, nonce_str, 8);
	json_object_array_add(params, json_object_new_string(data));
	json_object_object_add(json, "params", params);
	const char *json_str = json_object_get_string(json);
	char *request = strdup(json_str);
	json_object_put(json);
	LOG_DEBUG("Packet: %s", request);
	return request;

}

char * create_json_getwork_completion_encode(const bc_work_t *work, uint32_t nonce) {
	json_object *json = json_object_new_object();
	json_object_object_add(json, "jsonrpc", json_object_new_string("1.0"));
	json_object_object_add(json, "id", json_object_new_int(work->id));
	json_object_object_add(json, "method", json_object_new_string("getwork"));
	json_object *params = json_object_new_array();
	char data[257];
	strncpy(data, work->result.data_str, 257);
	char nonce_str[9];
    uint8_t p[4];
    p[0] = nonce & 0xff;
    p[1] = (nonce >> 8) & 0xff;
    p[2] = (nonce >> 16) & 0xff;
    p[3] = (nonce >> 24) & 0xff;

	sprintf(nonce_str, "%02x%02x%02x%02x", p[0],p[1],p[2],p[3]);
	LOG_DEBUG("submit nonce hex: %s", nonce_str);
	memcpy(data + 152, nonce_str, 8);
	json_object_array_add(params, json_object_new_string(data));
	json_object_object_add(json, "params", params);
	const char *json_str = json_object_get_string(json);
	char *request = strdup(json_str);
	json_object_put(json);
	LOG_DEBUG("Packet: %s", request);
	return request;

}


/**
 * \brief   Parse the JSON string returned by the 'getwork' command
 * \param   response    The JSON string
 * \param   work        The work given by Bitcoin
 */
void parse_json_getwork(const char *response, bc_work_t *work) {
	LOG_DEBUG("Getwork result: %s", response);
	json_object *json = json_tokener_parse(response);
	if (NULL == json) {
		LOG_FATAL("json_tokener_parse failed");
	}

	work->id = json_object_get_int(get_json_object(json, "id"));

	json_object *error = json_object_object_get(json, "error");
	if (NULL != error) {
		const int code = json_object_get_int(get_json_object(error, "code"));
		const char *msg = json_object_get_string(get_json_object(error, "message"));
		LOG_FATAL("JSON error code %d: %s", code, msg);
	}

	json_object *result = json_object_object_get(json, "result");
	if (NULL != result) {
		const char *midstate = json_object_get_string(get_json_object(result, "midstate"));
		const char *data = json_object_get_string(get_json_object(result, "data"));
		const char *target = json_object_get_string(get_json_object(result, "target"));

		hex_to_bytes(midstate, work->result.midstate, 32);
		hex_to_bytes(data, work->result.data, 128);
		hex_to_bytes(target, work->result.target, 32);
		strncpy(work->result.data_str, data, 257);	// TODO: what does this do?
		work->result.data_str[256] = '\0';
	} else {
		memset(work->result.midstate, 0, 32);
		memset(work->result.data, 0, 128);
		memset(work->result.target, 0, 32);
		memset(work->result.data_str, 0, 257);
	}

	json_object_put(json);
}


/**
 * \brief   Parse the JSON string returned by the 'getwork' command
 * \param   response    The JSON string
 * \param   work        The work given by Bitcoin
 */
int parse_json_getwork_decode(const char *response, bc_work_t *work) {
	LOG_DEBUG("Getwork result: %s", response);
	json_object *json = json_tokener_parse(response);
	if (NULL == json) {
		LOG_INFO("json_tokener_parse failed");
		return -1;
	}

	work->id = json_object_get_int(get_json_object(json, "id"));

	json_object *error = json_object_object_get(json, "error");
	if (NULL != error) {
		const int code = json_object_get_int(get_json_object(error, "code"));
		const char *msg = json_object_get_string(get_json_object(error, "message"));
		LOG_INFO("JSON error code %d: %s", code, msg);
		return -1;
	}

	json_object *result = json_object_object_get(json, "result");
	if (NULL != result) {
		const char *target = json_object_get_string(get_json_object(result, "target"));
		const char *midstate = target;//json_object_get_string(get_json_object(result, "midstate"));
		const char *data = json_object_get_string(get_json_object(result, "data"));

		hex_to_bytes(midstate, work->result.midstate, 32);
		hex_to_bytes(data, work->result.data, 128);
		hex_to_bytes(target, work->result.target, 32);

		uint32_t *data_32 = (uint32_t *) work->result.data;
		uint32_t *target_32 = (uint32_t *) work->result.target;
		for (int i = 0; i < 20; i++){
			data_32[i] = le32dec(data_32 + i);
		}
		for (int i = 0; i < 8; i++){
			target_32[i] = le32dec(target_32 + i);
		}

		strncpy(work->result.data_str, data, 257);	// TODO: what does this do?
		work->result.data_str[256] = '\0';
	} else {
		memset(work->result.midstate, 0, 32);
		memset(work->result.data, 0, 128);
		memset(work->result.target, 0, 32);
		memset(work->result.data_str, 0, 257);
	}

	json_object_put(json);
	return 0;
}

/**
 * \brief   Parse the JSON string returned by the 'getwork data' command
 * \param   response    The JSON string
 * \param   work_comp   The parser object
 */
void parse_json_getwork_completion(const char *response, bc_work_comp_t *work_comp) {
	LOG_DEBUG("Response: %s\n", response);
	json_object *json = json_tokener_parse(response);
	if (NULL == json) {
		LOG_FATAL("json_tokener_parse failed");
	}
	work_comp->id = json_object_get_int(get_json_object(json, "id"));

	json_object *error = json_object_object_get(json, "error");
	if (NULL != error) {
		const int code = json_object_get_int(get_json_object(error, "code"));
		const char *msg = json_object_get_string(get_json_object(error, "message"));
		LOG_FATAL("JSON error code %d: %s", code, msg);
	}

	work_comp->result = json_object_get_int(get_json_object(json, "result"));

	json_object_put(json);
}
