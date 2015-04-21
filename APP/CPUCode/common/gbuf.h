/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#ifndef GBUF_H_
#define GBUF_H_

#include <stddef.h>

/** Growable buffer*/
typedef struct {
    void *data;
    size_t size;
    size_t capacity;
} gbuf_t;

/**
 * Initialise growable buffer
 * \param   gbuf      Buffer
 * \param   capacity  Initial capacity of the buffer
 * \return  Zero on success, non-zero otherwise.
 */
int gbuf_init(gbuf_t *gbuf, size_t capacity);

/**
 * Append new data to the buffer
 * \param   gbuf        Buffer
 * \param   data        Data to append
 * \param   size        Size of the data in bytes
 * \return  Zero on success, non-zero otherwise
 */
int gbuf_cat(gbuf_t *gbuf, void* data, size_t size);

/**
 * Reset the buffer
 * \param   gbuf        Buffer
 */
void gbuf_reset(gbuf_t *gbuf);
/**
 * Release resources used by the buffer
 * \param   gbuf        Buffer
 */
void gbuf_deinit(gbuf_t *gbuf);

#endif /* GBUF_H_ */
