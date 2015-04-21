/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

/**
 * \brief Initialise a timer
 * \param v The timer (in s)
 */
static inline void timer_init(double *v)
{
    *v = 0;
}

/**
 * \brief Start the timer
 * \param v The timer (in s)
 */
static inline void timer_start(double *v)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *v -= tv.tv_sec + tv.tv_usec * 1e-6;
}
/**
 * \brief Stop the timer
 * \param v The timer (in s)
 */
static inline void timer_stop(double *v)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *v += tv.tv_sec + tv.tv_usec * 1e-6;
}

#endif /* TIMER_H_ */
