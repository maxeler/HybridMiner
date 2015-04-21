/**\file */
#ifndef SLIC_DECLARATIONS_HybridcoinMiner_H
#define SLIC_DECLARATIONS_HybridcoinMiner_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HybridcoinMiner_Scrypt_NumPipes (0)
#define HybridcoinMiner_Scrypt_Frequency (0)
#define HybridcoinMiner_Scrypt_Cslow (10)
#define HybridcoinMiner_Scrypt_ComputeUnit (0)
#define HybridcoinMiner_Bitcoin_numRecordsPerPipe (8)
#define HybridcoinMiner_Scrypt_Iterations (1024)
#define HybridcoinMiner_Bitcoin_numPipes (7)
#define HybridcoinMiner_Bitcoin_Frequency (210)


/*----------------------------------------------------------------------------*/
/*---------------------------- Interface default -----------------------------*/
/*----------------------------------------------------------------------------*/



/**
 * \brief Basic static function for the interface 'default'.
 * 
 * \param [in] ticks_BitcoinMinerKernel The number of ticks for which kernel "BitcoinMinerKernel" will run.
 * \param [in] inscalar_BitcoinMinerKernel_Control_base Input scalar parameter "BitcoinMinerKernel.Control.base".
 * \param [in] inscalar_BitcoinMinerKernel_Control_length Input scalar parameter "BitcoinMinerKernel.Control.length".
 * \param [in] inscalar_BitcoinMinerKernel_hash1block2_0 Input scalar parameter "BitcoinMinerKernel.hash1block2_0".
 * \param [in] inscalar_BitcoinMinerKernel_hash1block2_1 Input scalar parameter "BitcoinMinerKernel.hash1block2_1".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_0 Input scalar parameter "BitcoinMinerKernel.midstate_0".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_1 Input scalar parameter "BitcoinMinerKernel.midstate_1".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_2 Input scalar parameter "BitcoinMinerKernel.midstate_2".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_3 Input scalar parameter "BitcoinMinerKernel.midstate_3".
 * \param [in] inscalar_BitcoinMinerKernel_target_0 Input scalar parameter "BitcoinMinerKernel.target_0".
 * \param [in] inscalar_BitcoinMinerKernel_target_1 Input scalar parameter "BitcoinMinerKernel.target_1".
 * \param [in] inscalar_BitcoinMinerKernel_target_2 Input scalar parameter "BitcoinMinerKernel.target_2".
 * \param [in] inscalar_BitcoinMinerKernel_target_3 Input scalar parameter "BitcoinMinerKernel.target_3".
 * \param [out] outstream_toHost_bitcoin Stream "toHost_bitcoin".
 * \param [in] outstream_size_toHost_bitcoin The size of the stream outstream_toHost_bitcoin in bytes.
 */
void HybridcoinMiner(
	uint64_t ticks_BitcoinMinerKernel,
	uint64_t inscalar_BitcoinMinerKernel_Control_base,
	uint64_t inscalar_BitcoinMinerKernel_Control_length,
	uint64_t inscalar_BitcoinMinerKernel_hash1block2_0,
	uint64_t inscalar_BitcoinMinerKernel_hash1block2_1,
	uint64_t inscalar_BitcoinMinerKernel_midstate_0,
	uint64_t inscalar_BitcoinMinerKernel_midstate_1,
	uint64_t inscalar_BitcoinMinerKernel_midstate_2,
	uint64_t inscalar_BitcoinMinerKernel_midstate_3,
	uint64_t inscalar_BitcoinMinerKernel_target_0,
	uint64_t inscalar_BitcoinMinerKernel_target_1,
	uint64_t inscalar_BitcoinMinerKernel_target_2,
	uint64_t inscalar_BitcoinMinerKernel_target_3,
	void *outstream_toHost_bitcoin,
	size_t outstream_size_toHost_bitcoin);

/**
 * \brief Basic static non-blocking function for the interface 'default'.
 * 
 * Schedule to run on an engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 * 
 * 
 * \param [in] ticks_BitcoinMinerKernel The number of ticks for which kernel "BitcoinMinerKernel" will run.
 * \param [in] inscalar_BitcoinMinerKernel_Control_base Input scalar parameter "BitcoinMinerKernel.Control.base".
 * \param [in] inscalar_BitcoinMinerKernel_Control_length Input scalar parameter "BitcoinMinerKernel.Control.length".
 * \param [in] inscalar_BitcoinMinerKernel_hash1block2_0 Input scalar parameter "BitcoinMinerKernel.hash1block2_0".
 * \param [in] inscalar_BitcoinMinerKernel_hash1block2_1 Input scalar parameter "BitcoinMinerKernel.hash1block2_1".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_0 Input scalar parameter "BitcoinMinerKernel.midstate_0".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_1 Input scalar parameter "BitcoinMinerKernel.midstate_1".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_2 Input scalar parameter "BitcoinMinerKernel.midstate_2".
 * \param [in] inscalar_BitcoinMinerKernel_midstate_3 Input scalar parameter "BitcoinMinerKernel.midstate_3".
 * \param [in] inscalar_BitcoinMinerKernel_target_0 Input scalar parameter "BitcoinMinerKernel.target_0".
 * \param [in] inscalar_BitcoinMinerKernel_target_1 Input scalar parameter "BitcoinMinerKernel.target_1".
 * \param [in] inscalar_BitcoinMinerKernel_target_2 Input scalar parameter "BitcoinMinerKernel.target_2".
 * \param [in] inscalar_BitcoinMinerKernel_target_3 Input scalar parameter "BitcoinMinerKernel.target_3".
 * \param [out] outstream_toHost_bitcoin Stream "toHost_bitcoin".
 * \param [in] outstream_size_toHost_bitcoin The size of the stream outstream_toHost_bitcoin in bytes.
 * \return A handle on the execution status, or NULL in case of error.
 */
max_run_t *HybridcoinMiner_nonblock(
	uint64_t ticks_BitcoinMinerKernel,
	uint64_t inscalar_BitcoinMinerKernel_Control_base,
	uint64_t inscalar_BitcoinMinerKernel_Control_length,
	uint64_t inscalar_BitcoinMinerKernel_hash1block2_0,
	uint64_t inscalar_BitcoinMinerKernel_hash1block2_1,
	uint64_t inscalar_BitcoinMinerKernel_midstate_0,
	uint64_t inscalar_BitcoinMinerKernel_midstate_1,
	uint64_t inscalar_BitcoinMinerKernel_midstate_2,
	uint64_t inscalar_BitcoinMinerKernel_midstate_3,
	uint64_t inscalar_BitcoinMinerKernel_target_0,
	uint64_t inscalar_BitcoinMinerKernel_target_1,
	uint64_t inscalar_BitcoinMinerKernel_target_2,
	uint64_t inscalar_BitcoinMinerKernel_target_3,
	void *outstream_toHost_bitcoin,
	size_t outstream_size_toHost_bitcoin);

/**
 * \brief Advanced static interface, structure for the engine interface 'default'
 * 
 */
typedef struct { 
	uint64_t ticks_BitcoinMinerKernel; /**<  [in] The number of ticks for which kernel "BitcoinMinerKernel" will run. */
	uint64_t inscalar_BitcoinMinerKernel_Control_base; /**<  [in] Input scalar parameter "BitcoinMinerKernel.Control.base". */
	uint64_t inscalar_BitcoinMinerKernel_Control_length; /**<  [in] Input scalar parameter "BitcoinMinerKernel.Control.length". */
	uint64_t inscalar_BitcoinMinerKernel_hash1block2_0; /**<  [in] Input scalar parameter "BitcoinMinerKernel.hash1block2_0". */
	uint64_t inscalar_BitcoinMinerKernel_hash1block2_1; /**<  [in] Input scalar parameter "BitcoinMinerKernel.hash1block2_1". */
	uint64_t inscalar_BitcoinMinerKernel_midstate_0; /**<  [in] Input scalar parameter "BitcoinMinerKernel.midstate_0". */
	uint64_t inscalar_BitcoinMinerKernel_midstate_1; /**<  [in] Input scalar parameter "BitcoinMinerKernel.midstate_1". */
	uint64_t inscalar_BitcoinMinerKernel_midstate_2; /**<  [in] Input scalar parameter "BitcoinMinerKernel.midstate_2". */
	uint64_t inscalar_BitcoinMinerKernel_midstate_3; /**<  [in] Input scalar parameter "BitcoinMinerKernel.midstate_3". */
	uint64_t inscalar_BitcoinMinerKernel_target_0; /**<  [in] Input scalar parameter "BitcoinMinerKernel.target_0". */
	uint64_t inscalar_BitcoinMinerKernel_target_1; /**<  [in] Input scalar parameter "BitcoinMinerKernel.target_1". */
	uint64_t inscalar_BitcoinMinerKernel_target_2; /**<  [in] Input scalar parameter "BitcoinMinerKernel.target_2". */
	uint64_t inscalar_BitcoinMinerKernel_target_3; /**<  [in] Input scalar parameter "BitcoinMinerKernel.target_3". */
	void *outstream_toHost_bitcoin; /**<  [out] Stream "toHost_bitcoin". */
	size_t outstream_size_toHost_bitcoin; /**<  [in] The size of the stream outstream_toHost_bitcoin in bytes. */
} HybridcoinMiner_actions_t;

/**
 * \brief Advanced static function for the interface 'default'.
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in,out] interface_actions Actions to be executed.
 */
void HybridcoinMiner_run(
	max_engine_t *engine,
	HybridcoinMiner_actions_t *interface_actions);

/**
 * \brief Advanced static non-blocking function for the interface 'default'.
 *
 * Schedule the actions to run on the engine and return immediately.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * 
 * \param [in] engine The engine on which the actions will be executed.
 * \param [in] interface_actions Actions to be executed.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *HybridcoinMiner_run_nonblock(
	max_engine_t *engine,
	HybridcoinMiner_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'default'.
 * 
 * \param [in] group Group to use.
 * \param [in,out] interface_actions Actions to run.
 *
 * Run the actions on the first device available in the group.
 */
void HybridcoinMiner_run_group(max_group_t *group, HybridcoinMiner_actions_t *interface_actions);

/**
 * \brief Group run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule the actions to run on the first device available in the group and return immediately.
 * The status of the run must be checked with ::max_wait. 
 * Note that use of ::max_nowait is prohibited with non-blocking running on groups:
 * see the ::max_run_group_nonblock documentation for more explanation.
 *
 * \param [in] group Group to use.
 * \param [in] interface_actions Actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *HybridcoinMiner_run_group_nonblock(max_group_t *group, HybridcoinMiner_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'default'.
 * 
 * \param [in] engarray The array of devices to use.
 * \param [in,out] interface_actions The array of actions to run.
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void HybridcoinMiner_run_array(max_engarray_t *engarray, HybridcoinMiner_actions_t *interface_actions[]);

/**
 * \brief Array run advanced static non-blocking function for the interface 'default'.
 * 
 *
 * Schedule to run the array of actions on the array of engines, and return immediately.
 * The length of interface_actions must match the size of engarray.
 * The status of the run can be checked either by ::max_wait or ::max_nowait;
 * note that one of these *must* be called, so that associated memory can be released.
 *
 * \param [in] engarray The array of devices to use.
 * \param [in] interface_actions The array of actions to run.
 * \return A handle on the execution status of the actions, or NULL in case of error.
 */
max_run_t *HybridcoinMiner_run_array_nonblock(max_engarray_t *engarray, HybridcoinMiner_actions_t *interface_actions[]);

/**
 * \brief Converts a static-interface action struct into a dynamic-interface max_actions_t struct.
 *
 * Note that this is an internal utility function used by other functions in the static interface.
 *
 * \param [in] maxfile The maxfile to use.
 * \param [in] interface_actions The interface-specific actions to run.
 * \return The dynamic-interface actions to run, or NULL in case of error.
 */
max_actions_t* HybridcoinMiner_convert(max_file_t *maxfile, HybridcoinMiner_actions_t *interface_actions);

/**
 * \brief Initialise a maxfile.
 */
max_file_t* HybridcoinMiner_init(void);

/* Error handling functions */
int HybridcoinMiner_has_errors(void);
const char* HybridcoinMiner_get_errors(void);
void HybridcoinMiner_clear_errors(void);
/* Free statically allocated maxfile data */
void HybridcoinMiner_free(void);
/* These are dummy functions for hardware builds. */
int HybridcoinMiner_simulator_start(void);
int HybridcoinMiner_simulator_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SLIC_DECLARATIONS_HybridcoinMiner_H */

