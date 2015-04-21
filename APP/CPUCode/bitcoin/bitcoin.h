/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/


#ifndef BITCOIN_H_
#define BITCOIN_H_
#include "../common/worker.h"

int bitcoin_classic(int argc, char **argv);
void bitcoin_init(bitcoin_worker_t *worker, net_config_t *netconf, dfe_context * dfe, dfe_output *dfe_buffer);
void bitcoin_process_result_and_send_work(bitcoin_worker_t *worker, bc_work_t work);
void bitcoin_send_getwork(bitcoin_worker_t *worker, bc_work_t *work);
void bitcoin_generate_test_work(bc_work_t *work);
void bitcoin_init_test(bitcoin_worker_t *worker, dfe_context * dfe,  dfe_output *dfe_buffer);
void bitcoin_process_test_result(bitcoin_worker_t *worker);

#endif /* BITCOIN_H_ */
