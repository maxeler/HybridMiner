/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#ifndef DATA_STRUCT_H_
#define DATA_STRUCT_H_
#include <curl/curl.h>
#include "gbuf.h"



#define UNUSED __attribute__((unused))
#define ALIGN(x) __attribute((aligned(x)))



/** Result returned by Bitcoin getwork RPC*/
typedef struct {
    uint8_t midstate[32];
    uint8_t data[128];
    char    data_str[257];
    uint8_t target[32];
} bc_result_t;


/** Work returned by Bitcoin getwork RPC */
typedef struct {
    uint32_t    id;
    bc_result_t result;
} bc_work_t;

/** Response returned by Bitcoin getwork(data) RPC */
typedef struct {
    uint32_t    id;
    uint32_t    result;
} bc_work_comp_t;

/** Hash search result */
typedef struct {
    uint8_t     match;
    uint32_t    nonce;
} result_t;


/** Network configuration to connect to the BC server */
typedef struct {
    char    usr_pwd[2048];
    char    url[2048];
} net_config_t;



#endif /* DATA_STRUCT_H_ */
