/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#include <stdlib.h>
#include <string.h>

#include "gbuf.h"

int gbuf_init(gbuf_t *gbuf, size_t capacity)
{
    gbuf->data = malloc(capacity);
    if (gbuf->data == NULL) {
        return 1;
    }
    gbuf->capacity = capacity;
    gbuf->size = 0;
    return 0;
}

int gbuf_cat(gbuf_t *gbuf, void *data, size_t size)
{
    if (gbuf->size + size > gbuf->capacity) {
        size_t new_capacity = gbuf->size + size;
        void *tmp = realloc(gbuf->data, new_capacity);
        if (NULL == tmp) {
            return 1;
        }
        gbuf->data = tmp;
        gbuf->capacity = new_capacity;
    }
    memcpy((char *)gbuf->data + gbuf->size, data, size);
    gbuf->size += size;
    return 0;
}

void gbuf_reset(gbuf_t *gbuf)
{
    gbuf->size = 0;
}

void gbuf_deinit(gbuf_t *gbuf)
{
    free(gbuf->data);
}
