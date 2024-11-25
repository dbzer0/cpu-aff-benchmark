#ifdef SOLARIS
#include <sys/processor.h>
#include <sys/procset.h>
#include <thread.h>
#endif

#ifdef _AIX
#include <sys/processor.h>
#endif

#ifdef __APPLE__
#include <mach/thread_act.h>
#include <mach/mach_init.h>
#include <pthread/qos.h>
#include <sys/sysctl.h>  // Для sysctlbyname
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Configuration constants
#define MIN_PRIME 300000
#define MIN_LOOP 300
#define ARR_SIZE 200
#define DEBUG 0

// Error handling macro
#define HANDLE_ERROR(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Thread argument structure
typedef struct {
    unsigned long primes;
    int cpu;
} thread_args_t;

// Global data array
static int data[ARR_SIZE][ARR_SIZE];

#ifdef __APPLE__
// Структура для хранения информации о CPU
typedef struct {
    int logical_cpu_count;
    int physical_cpu_count;
    int cores_per_cpu;
} cpu_topology_t;

// Получение информации о CPU для macOS
static cpu_topology_t get_cpu_topology(void) {
    cpu_topology_t topology = {0, 0, 0};
    size_t len;

    len = sizeof(topology.logical_cpu_count);
    sysctlbyname("hw.logicalcpu", &topology.logical_cpu_count, &len, NULL, 0);

    len = sizeof(topology.physical_cpu_count);
    sysctlbyname("hw.physicalcpu", &topology.physical_cpu_count, &len, NULL, 0);

    if (topology.physical_cpu_count > 0) {
        topology.cores_per_cpu = topology.logical_cpu_count / topology.physical_cpu_count;
    }

    return topology;
}

// Установка QoS для потока в macOS
static void set_thread_qos(void) {
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0);
}

#if DEBUG
static void print_thread_info(void) {
    mach_port_t thread = mach_thread_self();
    thread_basic_info_data_t info;
    mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
    
    if (thread_info(thread, THREAD_BASIC_INFO, (thread_info_t)&info, &count) == KERN_SUCCESS) {
        printf("Thread CPU usage: %d seconds\n", info.cpu_usage);
        printf("Thread policy: %d\n", info.policy);
        printf("Thread run state: %d\n", info.run_state);
    }
    
    mach_port_deallocate(mach_task_self(), thread);
}
#endif
#endif

// Function prototypes
static void *worker(void *arg);
static void *worker_mem(void *arg);
static void *worker_mem_non(void *arg);
static void set_aff(uint8_t nids);
static double get_elapsed_time(struct timeval *start, struct timeval *end);
static void wait_for(unsigned int secs);

#ifdef __APPLE__
// Оптимизация производительности для macOS
static void optimize_thread_performance(void) {
    // Установка высокого приоритета QoS
    set_thread_qos();
    
    #if DEBUG
    print_thread_info();
    #endif
}
#endif

// Memory test functions
static unsigned long seq_mem_read(const unsigned long primes) {
    unsigned long r = 0;
    const int target = 42; // Искомое значение
    
    for (unsigned long i = 0; i < primes; i++) {
        for (int n = 0; n < ARR_SIZE; n++) {
            for (int m = 0; m < ARR_SIZE; m++) {
                if (data[n][m] % 100 == target) { // Ищем числа, которые при делении на 100 дают остаток 42
                    r++;
                }
            }
        }
    }
    return r;
}

static unsigned long non_seq_mem_read(const unsigned long primes) {
    unsigned long r = 0;
    const int target = 42; // Искомое значение
    
    for (unsigned long i = 0; i < primes; i++) {
        for (int n = 0; n < ARR_SIZE; n++) {
            for (int m = 0; m < ARR_SIZE; m++) {
                if (data[m][n] % 100 == target) { // Ищем числа, которые при делении на 100 дают остаток 42
                    r++;
                }
            }
        }
    }
    return r;
}

// CPU test functions
#if TEST_RANDOM_NUMBERS
static void *rnd(const unsigned long primes) {
    unsigned rand_state = 0;
    
    for (unsigned long i = 0; i < primes; i++) {
        rand_r(&rand_state);
    }
    return NULL;
}
#endif

static unsigned long do_kotiks(const unsigned long primes) {
    #ifdef __APPLE__
    optimize_thread_performance();
    #endif

    unsigned long long prime_count = 0;
    
    for (unsigned long long c = 3; c < primes; c++) {
        double sqrt_c = sqrt((double)c);
        bool is_prime = true;
        
        for (unsigned long long l = 2; l <= sqrt_c; l++) {
            if (c % l == 0) {
                is_prime = false;
                break;
            }
        }
        
        if (is_prime) {
            prime_count++;
        }
    }
    
    return prime_count;
}

// Platform-specific thread binding
static void bind_thread(int cpu_id) {
    #ifdef _AIX
    if (bindprocessor(BINDTHREAD, thread_self(), cpu_id) != 0) {
        HANDLE_ERROR("bindprocessor failed");
    }
    #elif defined(__APPLE__)
    (void)cpu_id;  // Избегаем предупреждения о неиспользуемом параметре
    optimize_thread_performance();
    #endif

    #if DEBUG
    printf("Thread optimization performed for CPU %d\n", cpu_id);
    #endif
}

// Thread worker functions
static void *worker(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    
    #if defined(_AIX) || defined(__APPLE__)
    bind_thread(args->cpu);
    #endif
    
    do_kotiks(args->primes);
    return NULL;
}

static void *worker_mem(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    
    #if defined(_AIX) || defined(__APPLE__)
    bind_thread(args->cpu);
    #endif
    
    seq_mem_read(args->primes);
    return NULL;
}

static void *worker_mem_non(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    
    #if defined(_AIX) || defined(__APPLE__)
    bind_thread(args->cpu);
    #endif
    
    non_seq_mem_read(args->primes);
    return NULL;
}

// Utility functions
static double get_elapsed_time(struct timeval *start, struct timeval *end) {
    return (double)(end->tv_usec - start->tv_usec) / 1000000 +
           (double)(end->tv_sec - start->tv_sec);
}

static void wait_for(unsigned int secs) {
    time_t end_time = time(NULL) + secs;
    while (time(NULL) < end_time);
}

// Platform specific affinity setting
static void set_aff(uint8_t nids) {
    #ifdef SOLARIS
    procset_t ps;
    uint32_t flags = PA_TYPE_CPU | PA_AFF_STRONG;
    id_t *ids = malloc(nids * sizeof(id_t));
    
    if (ids == NULL) {
        HANDLE_ERROR("Failed to allocate memory for CPU IDs");
    }

    for (int i = 0; i < nids; i++) {
        ids[i] = i;
    }

    setprocset(&ps, POP_AND, P_PID, P_MYID, P_LWPID, thr_self());

    if (processor_affinity(&ps, &nids, ids, &flags) != 0) {
        free(ids);
        HANDLE_ERROR("Failed to set processor affinity");
    }
    
    free(ids);
    #elif defined(__APPLE__)
    cpu_topology_t topology = get_cpu_topology();
    
    #if DEBUG
    printf("CPU Topology:\n");
    printf("  Logical CPUs: %d\n", topology.logical_cpu_count);
    printf("  Physical CPUs: %d\n", topology.physical_cpu_count);
    printf("  Cores per CPU: %d\n", topology.cores_per_cpu);
    #endif

    if (nids > topology.logical_cpu_count) {
        fprintf(stderr, "Warning: Requested thread count (%d) exceeds available logical CPUs (%d)\n",
                nids, topology.logical_cpu_count);
    }
    #endif
}

// Test execution functions
static void seq_load(int num_threads) {
    pthread_t *tid = malloc(num_threads * sizeof(pthread_t));
    if (!tid) {
        HANDLE_ERROR("malloc failed for thread IDs");
    }
    
    for (int j = 1; j <= num_threads; j++) {
        wait_for(1);
        
        struct timeval start, end;
        gettimeofday(&start, NULL);
        
        thread_args_t args = {
            .primes = num_threads * MIN_PRIME / j,
            .cpu = 0
        };
        
        for (int i = 0; i < j; i++) {
            args.cpu = i;
            if (pthread_create(&tid[i], NULL, worker, &args) != 0) {
                HANDLE_ERROR("pthread_create failed");
            }
        }
        
        for (int i = 0; i < j; i++) {
            if (pthread_join(tid[i], NULL) != 0) {
                HANDLE_ERROR("pthread_join failed");
            }
        }
        
        gettimeofday(&end, NULL);
        printf("%d threads, %lu kotik's per thread. Total time = %f seconds\n",
               j, args.primes, get_elapsed_time(&start, &end));
    }
    
    free(tid);
}

static void test_memory_seq_read(int num_threads) {
    pthread_t *tid = malloc(num_threads * sizeof(pthread_t));
    if (!tid) {
        HANDLE_ERROR("malloc failed for thread IDs");
    }
    
    for (int j = 1; j <= num_threads; j++) {
        wait_for(1);
        
        struct timeval start, end;
        gettimeofday(&start, NULL);
        
        thread_args_t args = {
            .primes = num_threads * MIN_LOOP / j,
            .cpu = 0
        };
        
        for (int i = 0; i < j; i++) {
            args.cpu = i;
            if (pthread_create(&tid[i], NULL, worker_mem, &args) != 0) {
                HANDLE_ERROR("pthread_create failed");
            }
        }
        
        for (int i = 0; i < j; i++) {
            if (pthread_join(tid[i], NULL) != 0) {
                HANDLE_ERROR("pthread_join failed");
            }
        }
        
        gettimeofday(&end, NULL);
        printf("%d threads, %lu loops, memory sequence read time = %f seconds\n",
               j, args.primes, get_elapsed_time(&start, &end));
    }
    
    free(tid);
}

static void test_memory_non_seq_read(int num_threads) {
    pthread_t *tid = malloc(num_threads * sizeof(pthread_t));
    if (!tid) {
        HANDLE_ERROR("malloc failed for thread IDs");
    }
    
    for (int j = 1; j <= num_threads; j++) {
        wait_for(1);
        
        struct timeval start, end;
        gettimeofday(&start, NULL);
        
        thread_args_t args = {
            .primes = num_threads * MIN_LOOP / j,
            .cpu = 0
        };
        
        for (int i = 0; i < j; i++) {
            args.cpu = i;
            if (pthread_create(&tid[i], NULL, worker_mem_non, &args) != 0) {
                HANDLE_ERROR("pthread_create failed");
            }
        }
        
        for (int i = 0; i < j; i++) {
            if (pthread_join(tid[i], NULL) != 0) {
                HANDLE_ERROR("pthread_join failed");
            }
        }
        
        gettimeofday(&end, NULL);
        printf("%d threads, %lu loops, memory random read time = %f seconds\n",
               j, args.primes, get_elapsed_time(&start, &end));
    }
    
    free(tid);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s num_threads\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Error: Number of threads must be positive\n");
        return EXIT_FAILURE;
    }
    
    printf("  1 memory allocating... ");
    fflush(stdout);
    
    // Инициализация генератора случайных чисел
    unsigned int seed = time(NULL);
    
    // Заполнение массива случайными данными
    for (int i = 0; i < ARR_SIZE; i++) {
        for (int j = 0; j < ARR_SIZE; j++) {
            // Генерируем случайные числа в диапазоне 0-999
            data[i][j] = rand_r(&seed) % 1000;
        }
    }
    printf("OK.\n");
    
    printf("  2 setting affinity for %d cpu's... ", num_threads);
    fflush(stdout);
    
    #ifdef __APPLE__
    cpu_topology_t topology = get_cpu_topology();
    printf("OK.\n");
    printf("\nDetected CPU configuration:\n");
    printf("  Logical CPUs: %d\n", topology.logical_cpu_count);
    printf("  Physical CPUs: %d\n", topology.physical_cpu_count);
    printf("  Cores per CPU: %d\n", topology.cores_per_cpu);
    printf("\nNote: macOS does not support hard CPU affinity. Using QoS optimizations instead.\n");
    #else
    printf("OK.\n");
    #endif
    
    set_aff(num_threads);
    
    printf("\nStarting CPU sequence test...\n");
    seq_load(num_threads);
    
    printf("\nStarting sequence memory read test...\n");
    test_memory_seq_read(num_threads);
    
    printf("\nStarting random memory read test...\n");
    test_memory_non_seq_read(num_threads);
    
    return EXIT_SUCCESS;
}
