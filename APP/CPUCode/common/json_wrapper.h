/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#ifndef JSON_WRAPPER_H_
#define JSON_WRAPPER_H_

#include <stdint.h>
#include "data_struct.h"



char * create_json_getwork(uint32_t id);
char * create_json_getwork_completion(const bc_work_t *work, uint32_t nonce);
char * create_json_getwork_completion_encode(const bc_work_t *work, uint32_t nonce);
void parse_json_getwork(const char *response, bc_work_t *work);
void parse_json_getwork_completion(const char *response, bc_work_comp_t *work_comp);
int parse_json_getwork_decode(const char *response, bc_work_t *work);




#endif // JSON_WRAPPER_H_
