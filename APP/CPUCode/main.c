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
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>

#include "Maxfiles.h"
#include "bitcoin/bitcoin.h"
#include "scrypt/scrypt.h"
#include "common/data_struct.h"
#include "common/worker.h"
#include "common/logging.h"
#include "common/dfe.h"
#include "common/timer.h"

typedef struct cli_args {
    const char *bcuser;
    const char *bcpasswd;
    const char *bcip_port;
    const char *suser;
    const char *spasswd;
    const char *sip_port;
    bool		test;
} cli_args;

typedef struct thread_args{
	const cli_args *args;
	dfe_limits limits;
	dfe_context	*dfe;
	int id;
} thread_args;

static void print_usage(const char *name) {
	printf("Usage:\n");
	printf("  %s -i <bitcoin host:port> -u <bitcoin username> -p <bitcoin password>"
			" -l <scrypt host:port> -a <scrypt username> -k <scrypt password> \n", name);
	printf("     -t Test-mode\n");
	printf("     -h This help message\n");
	exit(EXIT_SUCCESS);
}

//Number of workers (2 means 2 bitcoin and 2 scrypt workers)
#define NUM_WORKERS 2

static sem_t bc_process_mutex[NUM_WORKERS];
static sem_t bc_data_mutex[NUM_WORKERS];
static sem_t bc_load_data_mutex[NUM_WORKERS];
static sem_t bc_dfe_data_ready_mutex[NUM_WORKERS];
static sem_t s_process_mutex[NUM_WORKERS];
static sem_t s_data_mutex[NUM_WORKERS];
static sem_t s_load_data_mutex[NUM_WORKERS];
static sem_t s_dfe_data_ready_mutex[NUM_WORKERS];



static cli_args process_cli(int argc, char **argv) {
	LOG_DEBUG_ENTER();

	cli_args args;
	memset(&args, 0, sizeof(args));

	char c;
	while ((c = getopt (argc, argv, "u:p:i:a:k:l:th")) != -1) {
		switch (c) {
			case 'u':
				args.bcuser = optarg;
				break;
			case 'p':
				args.bcpasswd = optarg;
				break;
			case 'i':
				args.bcip_port = optarg;
				break;
			case 'a':
				args.suser = optarg;
				break;
			case 'k':
				args.spasswd = optarg;
				break;
			case 'l':
				args.sip_port = optarg;
				break;
			case 't':
				args.test = true;
				break;
			case 'h':
				print_usage(argv[0]);
				exit(0);
			case '?':
				if (optopt == 'u' || optopt == 'p' || optopt == 'i' || optopt == 'd')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
			default:
				abort();
		}
	}

	if (!args.test) {
		if(HybridcoinMiner_Bitcoin_numPipes > 0){
			if (args.bcuser == NULL)    { fprintf(stderr, "Bitcoin Username required.\n"); exit(EXIT_FAILURE); }
			if (args.bcpasswd == NULL)  { fprintf(stderr, "Bitcoin Password required.\n"); exit(EXIT_FAILURE); }
			if (args.bcip_port == NULL) { fprintf(stderr, "Bitcoin Address required.\n"); exit(EXIT_FAILURE); }
		}
		if(HybridcoinMiner_Scrypt_NumPipes > 0){
			if (args.suser == NULL)    { fprintf(stderr, "Scrypt Username required.\n"); exit(EXIT_FAILURE); }
			if (args.spasswd == NULL)  { fprintf(stderr, "Scrypt Password required.\n"); exit(EXIT_FAILURE); }
			if (args.sip_port == NULL) { fprintf(stderr, "Scrypt Address required.\n"); exit(EXIT_FAILURE); }
		}
	}

	return args;
}


static void run_test(){
	dfe_limits limits;
	create_limits(&limits, 0, 10000000);

	dfe_context *dfe;
	LOG_INFO("initialising DFE..\n");
	dfe_init(&dfe, limits, 1, 1);

#if HybridcoinMiner_Scrypt_NumPipes > 0
	LOG_INFO("initialising scrypt worker\n");
	scrypt_worker_t *scrypt_worker = malloc(sizeof(scrypt_worker_t));
	scrypt_init_test(scrypt_worker, dfe, dfe_get_scrypt_dfe_buffer(dfe, 0));
#endif


#if HybridcoinMiner_Bitcoin_numPipes > 0
	LOG_INFO("initialising bitcoin worker\n");
	bitcoin_worker_t *bitcoin_worker = malloc(sizeof(bitcoin_worker_t));
	bitcoin_init_test(bitcoin_worker, dfe, dfe_get_bitcoin_dfe_buffer(dfe, 0));
#endif


#if HybridcoinMiner_Bitcoin_numPipes > 0
	bc_work_t bitcoin_work;
		LOG_INFO("Bitcoin: get work\n");
		bitcoin_generate_test_work(&bitcoin_work);
		dfe_setup_run_bitcoin(dfe, &bitcoin_work, limits);
#endif

#if HybridcoinMiner_Scrypt_NumPipes > 0
		bc_work_t scrypt_work;
		LOG_INFO("Scrypt: get work\n");
		scrypt_generate_test_work(&scrypt_work);

		LOG_INFO("Scrypt: prepare run\n");
		scrypt_prepare_run(scrypt_worker, scrypt_work);
		dfe_setup_run_scrypt(dfe);
#endif
		LOG_INFO("DFE run\n");
		dfe_process(dfe, 0);

#if HybridcoinMiner_Scrypt_NumPipes > 0
		LOG_INFO("Scrypt: process result\n");
		scrypt_process_test_result(scrypt_worker, scrypt_work);
#endif

#if HybridcoinMiner_Bitcoin_numPipes > 0
		LOG_INFO("Bitcoint: process result\n");
		bitcoin_process_test_result(bitcoin_worker);
#endif


#if HybridcoinMiner_Scrypt_NumPipes > 0
	curl_easy_cleanup(scrypt_worker->curl);
#endif

#if HybridcoinMiner_Bitcoin_numPipes > 0
	curl_easy_cleanup(bitcoin_worker->curl);
#endif


}

static void run_scrypt(void* ptr){
	thread_args *t_args = (thread_args *) ptr;
	int worker_id = t_args->id;
	if(HybridcoinMiner_Scrypt_NumPipes > 0){
		net_config_t *netconf_scrypt = malloc(sizeof(net_config_t));
		scrypt_worker_t *scrypt_worker = malloc(sizeof(scrypt_worker_t));
		LOG_INFO("Initiating scrypt worker (id: %d)", worker_id);
		sprintf(netconf_scrypt->usr_pwd, "%s:%s", t_args->args->suser, t_args->args->spasswd);
		sprintf(netconf_scrypt->url, "http://%s", t_args->args->sip_port);

		scrypt_init(scrypt_worker, netconf_scrypt, t_args->dfe, dfe_get_scrypt_dfe_buffer(t_args->dfe, worker_id));

		state_t *state = &scrypt_worker->state;

		while(true){
			bc_work_t scrypt_work;

			scrypt_send_getwork(scrypt_worker, &scrypt_work);
			scrypt_prepare_run(scrypt_worker, scrypt_work);

			sem_post(&s_process_mutex[worker_id]); //ready for processing
			sem_wait(&s_load_data_mutex[worker_id]); //waiting for dfe to be ready to load data in

			dfe_setup_run_scrypt(t_args->dfe);

			sem_post(&s_dfe_data_ready_mutex[worker_id]);
			sem_wait(&s_data_mutex[worker_id]); // wait for data to be ready again.

			scrypt_process_result_and_send_work(scrypt_worker, scrypt_work);

			LOG_INFO("[Scrypt %d] Matches: %5lu", worker_id,
					state->stats.matches
					);

		}

		curl_easy_cleanup(scrypt_worker->curl);
	}
	else{
		while(true){
			//There is no Scrypt kernel in the bitstream
			sem_post(&s_process_mutex[worker_id]); //ready for processing
			sem_wait(&s_load_data_mutex[worker_id]); //waiting for dfe to be ready to load data in
			sem_post(&s_dfe_data_ready_mutex[worker_id]);
			sem_wait(&s_data_mutex[worker_id]); // wait for data to be ready again.
		}
	}
}

static void run_bitcoin(void* ptr){
	thread_args *t_args = (thread_args*) ptr;
	int worker_id = t_args->id;
	if(HybridcoinMiner_Bitcoin_numPipes > 0){
		net_config_t *netconf_bitcoin = malloc(sizeof(net_config_t));;
		bitcoin_worker_t *bitcoin_worker = malloc(sizeof(bitcoin_worker_t));
		LOG_INFO("Initiating bitcoin worker (id: %d)", worker_id);
		sprintf(netconf_bitcoin->usr_pwd, "%s:%s", t_args->args->bcuser, t_args->args->bcpasswd);
		sprintf(netconf_bitcoin->url, "http://%s", t_args->args->bcip_port);
		bitcoin_init(bitcoin_worker, netconf_bitcoin, t_args->dfe,dfe_get_bitcoin_dfe_buffer(t_args->dfe, worker_id) );

		state_t *state = &bitcoin_worker->state;
		while(true){
			bc_work_t bitcoin_work;

			bitcoin_send_getwork(bitcoin_worker, &bitcoin_work);

			sem_post(&bc_process_mutex[worker_id]); //ready for processing
			sem_wait(&bc_load_data_mutex[worker_id]); //waiting for dfe to be ready to load data in

			dfe_setup_run_bitcoin(t_args->dfe, &bitcoin_work, t_args->limits);

			sem_post(&bc_dfe_data_ready_mutex[worker_id]);
			sem_wait(&bc_data_mutex[worker_id]); // wait for data to be ready again.

			bitcoin_process_result_and_send_work(bitcoin_worker, bitcoin_work);

			LOG_INFO("[Bitcoin %d] Matches: %5lu", worker_id,
					state->stats.matches
			);

		}

		curl_easy_cleanup(bitcoin_worker->curl);
	}
	else{
		while(true){
			//There is no bitcoin kernel in the bitstream
			sem_post(&bc_process_mutex[worker_id]); //ready for processing
			sem_wait(&bc_load_data_mutex[worker_id]); //waiting for dfe to be ready to load data in
			sem_post(&bc_dfe_data_ready_mutex[worker_id]);
			sem_wait(&bc_data_mutex[worker_id]); // wait for data to be ready again.
		}

	}

}

static void run_live(const cli_args *args){

	pthread_t thread_scrypt[NUM_WORKERS];
	pthread_t thread_bitcoin[NUM_WORKERS];

	int worker_to_process = 0;

	dfe_limits limits;
	create_limits(&limits, 0, (uint64_t)1 << 32);

	dfe_context *dfe;
	LOG_INFO("Initialising DFE..");
	dfe_init(&dfe, limits, NUM_WORKERS, 0);

	thread_args *args_to_use = malloc(sizeof(thread_args) * NUM_WORKERS);
	for(int i = 0; i < NUM_WORKERS; i++){
		sem_init(&bc_data_mutex[i], 0, 0);
		sem_init(&s_data_mutex[i], 0, 0);
		sem_init(&bc_process_mutex[i], 0, 0);
		sem_init(&s_process_mutex[i], 0, 0);
		sem_init(&bc_load_data_mutex[i], 0, 0);
		sem_init(&s_load_data_mutex[i], 0, 0);
		sem_init(&bc_dfe_data_ready_mutex[i], 0, 0);
		sem_init(&s_dfe_data_ready_mutex[i], 0, 0);


		args_to_use[i].args = args;
		args_to_use[i].id = i;
		args_to_use[i].limits = limits;
		args_to_use[i].dfe = dfe;
		pthread_create (&thread_bitcoin[i], NULL, (void *) &run_bitcoin, (void *) &args_to_use[i]);
		pthread_create (&thread_scrypt[i], NULL, (void *) &run_scrypt, (void *) &args_to_use[i]);

	}


	uint64_t nscrypt = dfe_get_num_scrypt(dfe);

	double wall_clock = 0.0;
	double dfe_time = 0.0;

	while (true) {
		wall_clock = 0.0;
		dfe_time = 0.0;

		timer_start(&wall_clock);
		/* Waiting for workers to get work*/
		sem_wait(&bc_process_mutex[worker_to_process]);
		sem_wait(&s_process_mutex[worker_to_process]);

		/* Got work - signal to workers to setup actions for dfe. */
		sem_post(&bc_load_data_mutex[worker_to_process]);
		sem_post(&s_load_data_mutex[worker_to_process]);

		sem_wait(&bc_dfe_data_ready_mutex[worker_to_process]);
		sem_wait(&s_dfe_data_ready_mutex[worker_to_process]);

		timer_start(&dfe_time);
		dfe_process(dfe, worker_to_process);
		timer_stop(&dfe_time);

		/* DFE processing complete. Signal to the workers to process the results */
		sem_post(&bc_data_mutex[worker_to_process]);
		sem_post(&s_data_mutex[worker_to_process]);
		worker_to_process = (worker_to_process +1) % NUM_WORKERS;

		timer_stop(&wall_clock);

		LOG_INFO("===== Bitcoin: %7.1f Mhash/s", limits.length / wall_clock / 1000000);
		LOG_INFO("===== Scrypt: %7.1f khash/s", nscrypt / wall_clock / 1000);
		LOG_INFO("===== Utilization: %5.1f", dfe_time / wall_clock * 100.0);

	}

	for(int i = 0; i < NUM_WORKERS; i++){
		sem_destroy(&bc_data_mutex[i]);
		sem_destroy(&s_data_mutex[i]);
		sem_destroy(&bc_process_mutex[i]);
		sem_destroy(&s_process_mutex[i]);
		sem_destroy(&bc_load_data_mutex[i]);
		sem_destroy(&s_load_data_mutex[i]);
		sem_destroy(&bc_dfe_data_ready_mutex[i]);
		sem_destroy(&s_dfe_data_ready_mutex[i]);
	}

}

int main(int argc, char **argv) {
	LOG_DEBUG_ENTER();
	int scrypt_enabled = HybridcoinMiner_Scrypt_NumPipes > 0;
	int bitcoin_enabled = HybridcoinMiner_Bitcoin_numPipes > 0;
	logging_set_file(stdout);
	LOG_INFO("Hybrid Coin miner, Bitcoin [%c] Scrypt [%c]", bitcoin_enabled? 'O' : 'X', scrypt_enabled? 'O' : 'X');

	cli_args args = process_cli(argc, argv);

	if(args.test){
		LOG_INFO("====TEST MODE====");

		run_test();
	}
	else{
		run_live(&args);
	}

	return EXIT_SUCCESS;
}
