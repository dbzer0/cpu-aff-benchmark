# CPU Affinity and Memory Access Performance Benchmark

This tool is designed to measure CPU performance and memory access patterns in multi-threaded environments. It performs three types of tests:
1. CPU-intensive prime number calculations
2. Sequential memory access patterns
3. Random memory access patterns

## Features

- Cross-platform support (Linux, Solaris, AIX, macOS)
- Thread affinity optimization where available
- Configurable number of threads
- Memory access pattern analysis
- Prime number calculation benchmarks
- Platform-specific optimizations

## System Requirements

- POSIX-compliant operating system (Linux, Solaris, AIX, macOS)
- C compiler with C99 support
- pthread library
- Math library

## Building

**Using Make**:
```bash
make        # build the project
make clean  # remove build files
make help   # show help and available commands
```

The Makefile automatically detects your operating system and uses appropriate compiler flags.

## Usage

Run with predefined thread counts:
```bash
make run1   # run with 1 thread
make run2   # run with 2 threads
make run4   # run with 4 threads
make run8   # run with 8 threads
make run16  # run with 16 threads
```

Run with custom thread count:
```bash
make runN THREADS=12  # run with 12 threads
```

Show available run options:
```bash
make run
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

### Linux
- Full CPU affinity support through sched_setaffinity()
- Detailed CPU topology information

### macOS
- Does not support hard CPU affinity
- Uses QoS (Quality of Service) for thread optimization
- Displays additional CPU topology information

### Solaris
- Supports processor affinity through processor_affinity()
- Uses strong affinity flags

### AIX
- Uses bindprocessor() for thread-to-CPU binding
- Uses xlc compiler

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

BSD 3-Clause License

Copyright (c) 2024 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
