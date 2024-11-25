# CPU Affinity and Memory Access Performance Benchmark

This tool is designed to measure CPU performance and memory access patterns in multi-threaded environments. It performs three types of tests:
1. CPU-intensive prime number calculations
2. Sequential memory access patterns
3. Random memory access patterns

## Features

- Cross-platform support (Solaris, AIX, macOS)
- Thread affinity optimization where available
- Configurable number of threads
- Memory access pattern analysis
- Prime number calculation benchmarks
- Platform-specific optimizations

## System Requirements

- POSIX-compliant operating system (Solaris, AIX, macOS)
- C compiler with C99 support
- pthread library
- Math library

## Building

To compile the program, use:

```bash
gcc -o cpu-aff cpu-aff.c -lpthread -lm
```

On macOS, you might need to specify additional flags:

```bash
gcc -o cpu-aff cpu-aff.c -lpthread -lm -D__APPLE__
```

## Usage

Run the program with the number of threads as an argument:

```bash
./cpu-aff <num_threads>
```

For example, to run with 8 threads:

```bash
./cpu-aff 8
```

## Output Example

```
  1 memory allocating... OK.
  2 setting affinity for 8 cpu's... OK.

Starting CPU sequence test...
1 threads, 300000 kotik's per thread. Total time = 1.234567 seconds
2 threads, 150000 kotik's per thread. Total time = 0.654321 seconds
...

Starting sequence memory read test...
1 threads, 300 loops, memory sequence read time = 0.123456 seconds
2 threads, 150 loops, memory sequence read time = 0.098765 seconds
...

Starting random memory read test...
1 threads, 300 loops, memory random read time = 0.234567 seconds
2 threads, 150 loops, memory random read time = 0.187654 seconds
...
```

## Test Descriptions

### CPU Sequence Test
Tests CPU performance by calculating prime numbers. Each thread processes a portion of the work, demonstrating CPU scaling with multiple threads.

### Sequential Memory Read Test
Measures memory access performance when reading data in a sequential pattern. This test helps evaluate cache efficiency and memory bandwidth.

### Random Memory Read Test
Measures memory access performance when reading data in a non-sequential pattern. This test helps evaluate the impact of cache misses and memory access patterns.

## Platform-Specific Notes

### macOS
- Does not support hard CPU affinity
- Uses QoS (Quality of Service) for thread optimization
- Displays additional CPU topology information

### Solaris
- Supports processor affinity through processor_affinity()
- Uses strong affinity flags

### AIX
- Uses bindprocessor() for thread-to-CPU binding

## Configuration

The following constants can be modified in the source code:

```c
#define MIN_PRIME 300000  // Base number for prime calculations
#define MIN_LOOP 300      // Base number for memory access loops
#define ARR_SIZE 200      // Size of the test array
#define DEBUG 0           // Debug output toggle
```

## Performance Considerations

- The program uses random data for memory tests to ensure realistic cache behavior
- Thread affinity is used where available to improve performance consistency
- Each test includes a warm-up period to ensure stable measurements
- Memory tests include both sequential and non-sequential access patterns to measure different aspects of memory system performance

## License

This software is provided "as is" without any warranty. You are free to use, modify, and distribute it as needed.
