/*********************************************************************
 * Maxeler Technologies : Hybrid Coin Miner                          *
 *                                                                   *
 * Version: 1.0                                                      *
 * Date:    13th March 2014                                          *
 *                                                                   *
 * CPU code source file                                              *
 *                                                                   *
 *********************************************************************/

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <MaxSLiCInterface.h>
#include "dfe.h"
#include "../common/data_struct.h"
#include "../common/logging.h"
#include "Maxfiles.h"


#define CEIL_DIV(a,b)       (((a) + (b) - 1) / (b))
#define ROUND_UP(a,b)       (CEIL_DIV((a),(b)) * (b))


#define BITCOIN_KERNEL_NAME	"BitcoinMinerKernel"
#define SCRYPT_KERNEL_NAME	"ScryptKernel"

struct dfe_context {
    max_file_t*     maxfile;
    max_engine_t*   engine;
    max_actions_t*  actions;
    bool            is_ram_based;
    int				num_pipes;
    int				num_records_per_pipe;
    int				num_records;
    int				num_scrypt;
    dfe_output     	**bitcoin_output;
    uint32_t	   	**scrypt_buffer;
};




#define UNLIKELY(x)  __builtin_expect(!!(x), 0)
#if  HybridcoinMiner_Scrypt_NumPipes > 0
static long next_multiple(long a, long b)
{
        if (b == 0)
                LOG_FATAL("Cannot find next multiple of 0.");

        bool neg_a = false;
        if (UNLIKELY(a < 0)) {
                neg_a = true;
                a = -a;
        }

        long res = (a + b - 1) / b * b;

        if(UNLIKELY(neg_a))
                res = -res;

        return res;
}
#endif
/**
 * Write an array of bytes as multiple scalar inputs
 * \param   actions     The actions to be run
 * \param   block_name  The name of the block containing the scalar parameter
 * \param   name        The name of the scalar parameter
 * \param   data        Array of bytes to write
 * \param   len         Size of the array in bytes
 */
//#if HybridcoinMiner_Bitcoin_numPipes > 0
static void set_multi_scalar_inputs(max_actions_t *actions, const char *block_name, const char *name, const uint8_t *data,
		size_t len) {
	size_t rem = len;
	int num_words = (len + 7) / 8;
	for (int w = 0; w < num_words; w++) {
		uint64_t v = 0;
		for (int j = 0; j < 8; j++) {
			if (rem == 0)
				break;
			v |= ((uint64_t) data[w * 8 + j] << ((uint64_t) j << 3));
			rem--;
		}
		char reg_name[256];
		sprintf(reg_name, "%s_%d", name, w);
		max_set_uint64t(actions, block_name, reg_name, v);
	}
}


#if HybridcoinMiner_Bitcoin_numPipes > 0
static uint64_t get_constant(dfe_context *ctx, const char *name) {
	int ret = max_get_constant_uint64t(ctx->maxfile, name);
	if (ret == -1) {
		ret = 0;
	}
	return ret;
}
#endif

void create_limits(dfe_limits *limit, uint64_t base, uint64_t length){
	uint64_t new_length = length;

#if  HybridcoinMiner_Scrypt_NumPipes > 0 & HybridcoinMiner_Bitcoin_numPipes > 0
	//round it up to a number that is friendly for Scrypt cycle calculation.
	uint64_t scrypt_const = HybridcoinMiner_Scrypt_Cslow * (HybridcoinMiner_Scrypt_NumPipes - 1);
	uint64_t bc_npipes = HybridcoinMiner_Bitcoin_numPipes;
	uint64_t bc_record = HybridcoinMiner_Bitcoin_numRecordsPerPipe;
	uint64_t denom = 2 * HybridcoinMiner_Scrypt_Cslow * HybridcoinMiner_Scrypt_Iterations;
	double bfreq_sfreq = (double) HybridcoinMiner_Bitcoin_Frequency / HybridcoinMiner_Scrypt_Frequency;
	double sfreq_bfreq = (double) HybridcoinMiner_Scrypt_Frequency / HybridcoinMiner_Bitcoin_Frequency ;

	new_length = next_multiple(length, bc_npipes);
	uint64_t bitcoin_cycle = new_length/ bc_npipes + bc_npipes * bc_record;
	uint64_t a = next_multiple(bitcoin_cycle * sfreq_bfreq + scrypt_const, denom);
	new_length = ((a - scrypt_const) * bfreq_sfreq - (bc_npipes * bc_record)) * bc_npipes;

#endif
	limit->base = base;
	limit->length = new_length;



}


uint32_t *dfe_get_scrypt_dfe_buffer(dfe_context *ctx, int id){
	return ctx->scrypt_buffer[id];

}

dfe_output *dfe_get_bitcoin_dfe_buffer(dfe_context *ctx, int id){
	return ctx->bitcoin_output[id];

}

uint32_t dfe_get_bitcoin_num_records(dfe_context *ctx){
	return ctx->num_records;
}

uint32_t dfe_get_num_scrypt(dfe_context *ctx){
	return ctx->num_scrypt;
}


void dfe_init(dfe_context **pctx, dfe_limits limits, int num_workers, int testmode) {
	LOG_DEBUG_ENTER();
	if(testmode)
		LOG_INFO("Initialising DFE with test mode : %d", testmode);
	LOG_DEBUG("num of workers : %d", num_workers);
	dfe_context *ctx = malloc(sizeof(*ctx));
	*pctx = ctx;

	max_file_t *HybridcoinMiner_init(void);
	ctx->maxfile = HybridcoinMiner_init();
	if (NULL == ctx->maxfile) {
		LOG_FATAL("Could not initialize maxfile");
	}
	
	max_errors_mode(ctx->maxfile->errors, 0);

	if(limits.length <=0){
		LOG_FATAL("limits.length cannot be smaller or equal to 0!");
	}

#if HybridcoinMiner_Bitcoin_numPipes > 0
	ctx->is_ram_based         = get_constant(ctx, "Bitcoin_isRamBased");
	ctx->num_pipes            = get_constant(ctx, "Bitcoin_numPipes");
	ctx->num_records_per_pipe = get_constant(ctx, "Bitcoin_numRecordsPerPipe");
	ctx->num_records          = ctx->num_pipes * ctx->num_records_per_pipe;

	LOG_BOOL(DEBUG, ctx->is_ram_based);
	LOG_INT(DEBUG, ctx->num_pipes);
	LOG_INT(DEBUG, ctx->num_records_per_pipe);
	LOG_INT(DEBUG, ctx->num_records);

	ctx->bitcoin_output = malloc(sizeof(*ctx->bitcoin_output) * num_workers);
	for(int i = 0; i < num_workers; i++){
		ctx->bitcoin_output[i] = malloc(ctx->num_records * sizeof(dfe_output));
		if (NULL == ctx->bitcoin_output[i]) {
			LOG_FATAL("Could not initialize bitcoin dfe buffer");
		}
	}

	if (!ctx->is_ram_based) {
		LOG_FATAL("Old-style maxfiles are not supported");  // TODO: remove?
	}


	LOG_PTR(DEBUG, ctx->bitcoin_output);
	LOG_INT(DEBUG, limits.length);

#endif

#if HybridcoinMiner_Scrypt_NumPipes > 0

#if HybridcoinMiner_Bitcoin_numPipes > 0
	const uint64_t length = ROUND_UP(limits.length, ctx->num_pipes);
	const uint64_t num_cycles = length / ctx->num_pipes + ctx->num_records;
	const double ratio = (double)HybridcoinMiner_Scrypt_Frequency / HybridcoinMiner_Bitcoin_Frequency;

	const uint64_t scrypt_N = (num_cycles * ratio  - (HybridcoinMiner_Scrypt_NumPipes-1) * HybridcoinMiner_Scrypt_Cslow )/ (HybridcoinMiner_Scrypt_Cslow*2*HybridcoinMiner_Scrypt_Iterations);
	ctx->num_scrypt			  = scrypt_N * HybridcoinMiner_Scrypt_ComputeUnit;

#else
	if(testmode)
		ctx->num_scrypt			  = 10000 / HybridcoinMiner_Scrypt_ComputeUnit;//ROUND_UP(limits.length, HybridcoinMiner_Scrypt_ComputeUnit);
	else
		ctx->num_scrypt			  = 2000000 / HybridcoinMiner_Scrypt_ComputeUnit;//ROUND_UP(limits.length, HybridcoinMiner_Scrypt_ComputeUnit);
#endif

	size_t scrypt_io_size = ctx->num_scrypt * sizeof(uint32_t) * 32;
	ctx->scrypt_buffer   = malloc(sizeof(*ctx->scrypt_buffer) * num_workers);

	for(int i = 0; i < num_workers; i++){
		ctx->scrypt_buffer[i] = malloc(scrypt_io_size);
		if (NULL == ctx->scrypt_buffer[i]) {
			LOG_FATAL("Could not initialize scrypt dfe buffer");
		}
	}


	LOG_PTR(DEBUG, ctx->scrypt_buffer);
#endif


	ctx->actions = max_actions_init(ctx->maxfile, NULL);
	if (NULL == ctx->actions) {
		LOG_FATAL("Could not initialize actions");
	}


	/* Note: This is a blocking call. State information should be
	 * initialised before.
	 */
	ctx->engine = max_load(ctx->maxfile, "*");
	LOG_INFO("Call done");

	if (NULL == ctx->engine) {
		LOG_FATAL("Could not load maxfile to engine");
	}


	max_errors_mode(ctx->actions->errors, 0);
	max_errors_mode(ctx->engine->errors, 0);

}



void dfe_deinit(dfe_context *ctx) {
	LOG_DEBUG_ENTER();

	max_unload(ctx->engine);
	max_actions_free(ctx->actions);
	max_file_free(ctx->maxfile);
	free(ctx->bitcoin_output);
	free(ctx->scrypt_buffer);
	memset(ctx, 0, sizeof(ctx));
	free(ctx);
}

void dfe_setup_run_bitcoin(dfe_context *ctx, const bc_work_t *work, const dfe_limits limits){
	set_multi_scalar_inputs(ctx->actions, BITCOIN_KERNEL_NAME, "target",      work->result.target, sizeof(work->result.target));
	set_multi_scalar_inputs(ctx->actions, BITCOIN_KERNEL_NAME, "midstate",    work->result.midstate, sizeof(work->result.midstate));
	set_multi_scalar_inputs(ctx->actions, BITCOIN_KERNEL_NAME, "hash1block2", work->result.data + 64, 12);

	const uint64_t length = ROUND_UP(limits.length, ctx->num_pipes);
	max_set_uint64t(ctx->actions, BITCOIN_KERNEL_NAME, "Control.base",    limits.base);
	max_set_uint64t(ctx->actions, BITCOIN_KERNEL_NAME, "Control.length",  length);

	const uint64_t num_cycles = length / ctx->num_pipes + ctx->num_records;

	LOG_INT(DEBUG, limits.base);
	LOG_INT(DEBUG, limits.length);
	LOG_INT(DEBUG, length);
	LOG_PTR(DEBUG, ctx->bitcoin_output);
	max_set_ticks(ctx->actions, BITCOIN_KERNEL_NAME, num_cycles);
}

void dfe_setup_run_scrypt(dfe_context *ctx){
#if HybridcoinMiner_Scrypt_NumPipes >0
	uint64_t num_cycles = (ctx->num_scrypt/HybridcoinMiner_Scrypt_ComputeUnit)*HybridcoinMiner_Scrypt_Cslow*2*HybridcoinMiner_Scrypt_Iterations +
				 (HybridcoinMiner_Scrypt_NumPipes-1)*HybridcoinMiner_Scrypt_Cslow;
	max_set_uint64t(ctx->actions, SCRYPT_KERNEL_NAME, "nmax",  ctx->num_scrypt/HybridcoinMiner_Scrypt_ComputeUnit);
	max_set_uint64t(ctx->actions, SCRYPT_KERNEL_NAME, "run_cycle_count",  num_cycles);
#else
	//this should never be called..

	LOG_FATAL("trying to setup scrypt run when there is no scrypt kernel!");
	uint64_t num_cycles = 0;
	max_set_uint64t(ctx->actions, SCRYPT_KERNEL_NAME, "run_cycle_count",  num_cycles);
#endif


}

/**
 * \brief   Process a chunk of work on the DFE
 * \param   worker    Worker
 * \param   work      Work as returned by 'getwork'
 * \return Results, which should be cleared up using dfe_free_results()
 */
void dfe_process(dfe_context *ctx, int worker_id) {
	LOG_DEBUG_ENTER();


	LOG_DEBUG("worker id = %d\n", worker_id);

	max_clear_queues(ctx->actions);
#if HybridcoinMiner_Scrypt_NumPipes > 0
	size_t scrypt_io_size = ctx->num_scrypt * sizeof(uint32_t) * 32;
	max_queue_output(ctx->actions, "toHost_scrypt", ctx->scrypt_buffer[worker_id], scrypt_io_size);
	max_queue_input(ctx->actions, "fromHost_scrypt", ctx->scrypt_buffer[worker_id], scrypt_io_size);
#endif

#if HybridcoinMiner_Bitcoin_numPipes > 0
	max_queue_output(ctx->actions, "toHost_bitcoin", ctx->bitcoin_output[worker_id], ctx->num_records * sizeof(dfe_output));
#endif


	max_run(ctx->engine, ctx->actions);

	max_errors_t *errors[] = { ctx->engine->errors, ctx->actions->errors };
	for (int i = 0; i < (int)(sizeof(errors)/sizeof(errors[0])); i++) {
		if (!max_ok(errors[i])) {
			LOG_FATAL(max_errors_trace(errors[i]));
		}
	}

	
}


void dfe_free_results(dfe_results results) {
	free(results.nonces);
}
