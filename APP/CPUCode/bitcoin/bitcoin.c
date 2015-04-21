/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <getopt.h>

#include "../common/gbuf.h"
#include "../common/timer.h"
#include "../common/data_struct.h"
#include "../common/json_wrapper.h"
#include "../common/dfe.h"
#include "../common/logging.h"
#include "../common/worker.h"

/**
 * \brief   Callback function called when CURL receives a response
 * \param   ptr         The data received
 * \param   size        Size of an element in bytes
 * \param   nmemb       Number of element received
 * \param   userdata    User data specified when setting the callback
 */
static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
	LOG_DEBUG_ENTER();

	gbuf_t *gbuf = (gbuf_t *) userdata;
	size_t message_size = size * nmemb;

	int ret = gbuf_cat(gbuf, ptr, message_size);

	return ret == 0 ? message_size : 0;
}



/**
 * \brief   Fetch work from the Bitcoin server
 * \param   worker  Worker
 * \param   work    Work returned by the Bitcoin server
 */
void bitcoin_send_getwork(bitcoin_worker_t *worker, bc_work_t *work) {
	LOG_DEBUG_ENTER();

	char *json_request = create_json_getwork(worker->id);
	if (NULL == json_request) {
		LOG_FATAL("Could not create JSON request");
	}

	CURLcode curl_ret;
	LOG_DEBUG("Getwork request: %s", json_request);
	curl_ret = curl_easy_setopt(worker->curl, CURLOPT_POSTFIELDS, json_request);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_setopt: %s", curl_easy_strerror(curl_ret));
	}

	gbuf_reset(&worker->gbuf);

	curl_ret = curl_easy_perform(worker->curl);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_perform: %s", curl_easy_strerror(curl_ret));
	}

	free(json_request);

	parse_json_getwork(worker->gbuf.data, work);

	if (work->id != worker->id) {
		LOG_FATAL("Request ID mismatch");
	}
}

/**
 * \brief   Send completed work to Bitcoin
 * \param   worker      Worker
 * \param   work        Work returned from previous 'getwork' command
 * \param   nonce       Nonce that satisfies target
 * \param   work_comp   Bitcoin response
 */
static void send_getwork_completion(bitcoin_worker_t *worker, const bc_work_t *work, uint32_t nonce, bc_work_comp_t *work_comp) {
	LOG_DEBUG_ENTER();

	char *json_request = create_json_getwork_completion(work, nonce);
	if (NULL == json_request) {
		LOG_FATAL("Could not create JSON request");
	}

	CURLcode curl_ret;
	curl_ret = curl_easy_setopt(worker->curl, CURLOPT_POSTFIELDS, json_request);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_setopt: %s", curl_easy_strerror(curl_ret));
	}

	gbuf_reset(&worker->gbuf);

	curl_ret = curl_easy_perform(worker->curl);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_perform: %s", curl_easy_strerror(curl_ret));
	}

	free(json_request);

	parse_json_getwork_completion(worker->gbuf.data, work_comp);

	if (work_comp->id != worker->id) {
		LOG_FATAL("Request ID mismatch");
	}
}



/**
 * \brief   Initialise a worker
 * \param   worker      Worker
 * \param   net_config  Network configuration
 */
static void worker_init(bitcoin_worker_t *worker, const net_config_t *net_config) {
	LOG_DEBUG_ENTER();

	worker->id = 1;

	worker->curl = curl_easy_init();
	if (NULL == worker->curl) {
		LOG_FATAL("Could not init Curl");
	}

	if (gbuf_init(&worker->gbuf, 1024)) {
		LOG_FATAL("Could not create worker buffer");
	}

	CURLcode curl_ret;
	curl_ret = curl_easy_setopt(worker->curl, CURLOPT_WRITEFUNCTION, write_cb);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_setopt: %s", curl_easy_strerror(curl_ret));
	}

	curl_ret = curl_easy_setopt(worker->curl, CURLOPT_WRITEDATA, &worker->gbuf);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_setopt: %s", curl_easy_strerror(curl_ret));
	}

	curl_ret = curl_easy_setopt(worker->curl, CURLOPT_URL, net_config->url);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_setopt: %s", curl_easy_strerror(curl_ret));
	}

	curl_ret = curl_easy_setopt(worker->curl, CURLOPT_USERPWD, net_config->usr_pwd);
	if (CURLE_OK != curl_ret) {
		LOG_FATAL("curl_easy_setopt: %s", curl_easy_strerror(curl_ret));
	}

	worker->state.stats.matches = 0;
	worker->state.stats.none = 0;
}


void bitcoin_process_test_result(bitcoin_worker_t *worker){

	dfe_results results = { NULL, 0 };
	uint32_t num_records = dfe_get_bitcoin_num_records(worker->dfe);

	for (uint32_t i = 0; i < num_records; i++) {
		if (worker->dfe_buffer[i].match) {
			results.num++;
			LOG_DEBUG("Match: 0x%08x", worker->dfe_buffer[i].nonce);
		}
	}

	results.nonces = malloc(results.num * sizeof(*results.nonces));

	for (uint32_t i = 0, j = 0; i < num_records; i++) {
		if (worker->dfe_buffer[i].match) {
			results.nonces[j++] = worker->dfe_buffer[i].nonce;
		}
	}

	if (results.num > 0) {
		for (int i = 0; i < results.num; i++) {
			LOG_INFO("Found nonce match: 0x%08x", results.nonces[i]);
		}
	}

	LOG_INFO("====BITCOIN TEST PASSED====");

	free(results.nonces);
}

void bitcoin_init_test(bitcoin_worker_t *worker, dfe_context * dfe,  dfe_output *dfe_buffer){
	memset(worker, 0, sizeof(*worker));

	worker->state.stats.matches = 0;
	worker->state.stats.none = 0;
	worker->dfe = dfe;
	worker->dfe_buffer = dfe_buffer;
}

void bitcoin_generate_test_work(bc_work_t *work){
	work->id = 1;
	static const uint8_t midstate[32] = { 0x33, 0x9A, 0x90, 0xBC, 0xF0, 0xBF,
			0x58, 0x63, 0x7D, 0xAC, 0xCC, 0x90, 0xA8, 0xCA, 0x59, 0x1E, 0xE9,
			0xD8, 0xC8, 0xC3, 0xC8, 0x03, 0x01, 0x4F, 0x36, 0x87, 0xB1, 0x96,
			0x1B, 0xF9, 0x19, 0x47 };
	static const uint8_t target[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
			0x00, 0x00, 0x00, 0x00 };
	static const uint8_t data[12] = { 0x4A, 0x5E, 0x1E, 0x4B, 0x49, 0x5F, 0xAB,
			0x29, 0x1D, 0x00, 0xFF, 0xFF };
	memcpy(work->result.midstate, midstate, 32);
	memcpy(work->result.target, target, 32);
	memcpy(work->result.data + 64, data, 12);
}

void bitcoin_init(bitcoin_worker_t *worker, net_config_t *netconf, dfe_context * dfe, dfe_output *dfe_buffer){
	memset(worker, 0, sizeof(*worker));

	worker_init(worker, netconf);
	worker->dfe = dfe;
	worker->dfe_buffer = dfe_buffer;
}

void bitcoin_process_result_and_send_work(bitcoin_worker_t *worker, bc_work_t work){
	uint64_t num = 0;
	uint32_t num_records = dfe_get_bitcoin_num_records(worker->dfe);
	for (uint32_t i = 0; i < num_records; i++) {
		if (worker->dfe_buffer[i].match) {
			num++;
			LOG_DEBUG("Match: 0x%08x", worker->dfe_buffer[i].nonce);

			bc_work_comp_t work_comp;
			memset(&work_comp, 0, sizeof(bc_work_comp_t));
			send_getwork_completion(worker, &work, worker->dfe_buffer[i].nonce, &work_comp);
			if(!work_comp.result){
				worker->state.stats.reject ++;
			}
			else{
				worker->state.stats.matches ++;
			}
			LOG_DEBUG("Sending work completed");
		}
	}

	if(0 == num){
		worker->state.stats.none ++;
	}

}


