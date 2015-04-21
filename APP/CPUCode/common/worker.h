/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#ifndef WORKER_H_
#define WORKER_H_
#include "dfe.h"

typedef struct {
    struct {
        uint64_t    matches;
        uint64_t    reject;
        uint64_t    none;
        uint64_t	nhash;
    } stats;
} state_t;

/** Worker */
typedef struct {
	CURL			*curl;
	int				device;
	dfe_context    *dfe;
    gbuf_t          gbuf;
    state_t         state;
    uint32_t		id;
    dfe_output		*dfe_buffer;
} bitcoin_worker_t;



typedef struct {
	CURL			*curl;
	int				device;
	dfe_context    *dfe;
    gbuf_t          gbuf;
    state_t         state;
    uint32_t		id;
    uint32_t		*dfe_buffer;
    uint32_t		*tstate_buffer;
    uint32_t		*ostate_buffer;
} scrypt_worker_t;


#endif /* WORKER_H_ */
