//
// This file defines the function signatures necessary for creating the three
// cache systems and defines the prefetcher struct.
//

#include "prefetchers.h"

#define RPT_SIZE 128

// Null Prefetcher
// ============================================================================
uint32_t null_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                uint32_t address, bool is_miss)
{
    return 0; // No lines prefetched
}

void null_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *null_prefetcher_new()
{
    struct prefetcher *null_prefetcher = calloc(1, sizeof(struct prefetcher));
    null_prefetcher->handle_mem_access = &null_handle_mem_access;
    null_prefetcher->cleanup = &null_cleanup;
    return null_prefetcher;
}

// Sequential Prefetcher
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

uint32_t sequential_handle_mem_access(struct prefetcher *prefetcher,
                                      struct cache_system *cache_system, uint32_t address,
                                      bool is_miss)
{
    uint32_t N = *((uint32_t*)prefetcher->data);
    // TODO: Return the number of lines that were prefetched.

    int successes = 0;
    for (uint32_t i=1; i<=N; i++) {
        int success = cache_system_mem_access(cache_system, address + i*(cache_system->line_size), 'R', true);

        if (success == 0) {
            successes++;
        }
    }

    return successes;
}

void sequential_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // sequential_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *sequential_prefetcher_new(uint32_t prefetch_amount)
{
    struct prefetcher *sequential_prefetcher = calloc(1, sizeof(struct prefetcher));
    sequential_prefetcher->handle_mem_access = &sequential_handle_mem_access;
    sequential_prefetcher->cleanup = &sequential_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to sequential_prefetcher->data.
    sequential_prefetcher->data = calloc(1, sizeof(uint32_t));
    *((uint32_t*)sequential_prefetcher->data) = prefetch_amount;

    return sequential_prefetcher;
}

// Adjacent Prefetcher
// ============================================================================
uint32_t adjacent_handle_mem_access(struct prefetcher *prefetcher,
                                    struct cache_system *cache_system, uint32_t address,
                                    bool is_miss)
{
    // TODO perform the necessary prefetches for the adjacent strategy.
    int success = cache_system_mem_access(cache_system, address + cache_system->line_size, 'R', true);

    // TODO: Return the number of lines that were prefetched.
    if (success == 0) {
        return 1;
    } else {
        return 0;
    }
}

void adjacent_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // adjacent_prefetcher_new function.
}

struct prefetcher *adjacent_prefetcher_new()
{
    struct prefetcher *adjacent_prefetcher = calloc(1, sizeof(struct prefetcher));
    adjacent_prefetcher->handle_mem_access = &adjacent_handle_mem_access;
    adjacent_prefetcher->cleanup = &adjacent_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to adjacent_prefetcher->data.

    return adjacent_prefetcher;
}

// Custom Prefetcher
// ============================================================================
uint32_t custom_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                  uint32_t address, bool is_miss)
{
    // Try to locate address in RPT
    bool found = false;
    struct line_access *access;
    for (int i=0; i<RPT_SIZE; i++) {
        access = &((struct line_access*) prefetcher->data)[i];
        if (access->address == address) {
            found = true;
            break;
        }
    }

    int max_access_time = 0;
    int max_access_index = 0;

    int min_access_time = 2147483647;
    int min_access_index = 0;
    for (int i=0; i<RPT_SIZE; i++) {
        struct line_access *access = &((struct line_access*) prefetcher->data)[i];
        if (access->time_of_access > max_access_time) {
            max_access_time = access->time_of_access;
            max_access_index = i;
        }
        if (access->time_of_access < min_access_time) {
            min_access_time = access->time_of_access;
            min_access_index = i;
        }
    }

    struct line_access *prev_access = &((struct line_access*) prefetcher->data)[max_access_index];

    // If address not found in RPT, replace the least recently accessed address with current address
    if (!found) {
        struct line_access *new_access = malloc(sizeof(struct line_access));
        new_access->address = address;
        new_access->stride = prev_access->address - address;
        new_access->time_of_access = max_access_time + 1;
        ((struct line_access*) prefetcher->data)[min_access_index] = *new_access;

        // Don't prefetch anything
        return 0;
    }

    // If the address is in the array, examine its stride to see if it matches the previous stride
    int prev_stride = prev_access->stride;
    int stride = access->address - prev_access->address;

    access->stride = stride;

    if (stride == prev_stride) {
        int success = cache_system_mem_access(cache_system, address + cache_system->line_size, 'R', true);
        if (success == 0) {
            return 1;
        } else {
            return 0;
        }
    }
    
    return 0;
}

void custom_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // custom_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *custom_prefetcher_new()
{
    struct prefetcher *custom_prefetcher = calloc(1, sizeof(struct prefetcher));
    custom_prefetcher->handle_mem_access = &custom_handle_mem_access;
    custom_prefetcher->cleanup = &custom_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to custom_prefetcher->data.
    custom_prefetcher->data = calloc(RPT_SIZE, sizeof(struct line_access));

    return custom_prefetcher;
}
