/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#ifndef DFE_H_
#define DFE_H_
#include "data_struct.h"


typedef struct dfe_context dfe_context;

/** Data sent from the DFE to host */
typedef struct dfe_output {
    uint32_t    nonce;
    uint8_t     match :1;
    uint8_t      :7;
    uint8_t     pad[11];
} dfe_output;

typedef struct dfe_limits {
	uint32_t base;
	uint64_t length;
} dfe_limits;


typedef struct dfe_results {
	uint32_t *nonces;
	int num;
} dfe_results;


void dfe_init(dfe_context **pctx, dfe_limits limits, int num_workers, int testmode);
void dfe_deinit(dfe_context *ctx);
uint32_t *dfe_get_scrypt_dfe_buffer(dfe_context *ctx, int id);
dfe_output *dfe_get_bitcoin_dfe_buffer(dfe_context *ctx, int id);
uint32_t dfe_get_bitcoin_num_records(dfe_context *ctx);
void dfe_setup_run_bitcoin(dfe_context *ctx, const bc_work_t *work, const dfe_limits limits);
void dfe_setup_run_scrypt(dfe_context *ctx);
void dfe_process(dfe_context *ctx, int worker_id) ;
void dfe_free_results(dfe_results results);
uint32_t dfe_get_num_scrypt(dfe_context *ctx);
void create_limits(dfe_limits *limit, uint64_t base, uint64_t length);

#endif // DFE_H_
