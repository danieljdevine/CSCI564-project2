//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"

// LRU Replacement Policy
// ============================================================================

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    int latest_access_time = 0;
    for (int i = 0; i < cache_system->associativity; i++) {
        int *access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];
        if (*access_time > latest_access_time) {
            latest_access_time = *access_time;
        }
    }

    int current_access_time = latest_access_time + 1;

    for (int i = 0; i < cache_system->associativity; i++) {
        struct cache_line *line = &cache_system->cache_lines[set_idx * cache_system->associativity + i];

        if (line->tag == tag) {
            int *last_access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];
            memcpy(last_access_time, &current_access_time, sizeof(int));
        }
    }
}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{

    uint32_t oldest_idx = 0;
    int min_access_time = (int) ((& (cache_system->stats))->accesses) + 1;

    for (int i = 0; i < cache_system->associativity; i++) {

        int *last_access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];

        if (*last_access_time < min_access_time) {
            oldest_idx = i;
            min_access_time = *last_access_time;
        }
    }

    return oldest_idx;
}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    free(replacement_policy->data);
}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;

    lru_rp->data = calloc(sets * associativity, sizeof(int));

    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{

}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    return rand() % cache_system->associativity;
}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{

}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    srand(time(NULL));

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================
void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx,
                                   uint32_t tag)
{
    int latest_access_time = 0;
    for (int i = 0; i < cache_system->associativity; i++) {
        int *access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];
        if (*access_time > latest_access_time) {
            latest_access_time = *access_time;
        }
    }

    int current_access_time = latest_access_time + 1;

    for (int i = 0; i < cache_system->associativity; i++) {
        struct cache_line *line = &cache_system->cache_lines[set_idx * cache_system->associativity + i];
 
        if (line->tag == tag) {
            int *last_access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];
            memcpy(last_access_time, &current_access_time, sizeof(int));
        }
    }
}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx)
{
    int oldest_idx = -1;
    int min_access_time = (int) ((& (cache_system->stats))->accesses) + 1;

    for (int i = 0; i < cache_system->associativity; i++) {
        struct cache_line *line = &cache_system->cache_lines[set_idx * cache_system->associativity + i];
        int *last_access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];

        if (line->status != MODIFIED && *last_access_time < min_access_time) {
            oldest_idx = i;
            min_access_time = *last_access_time;
        }
    }

    if (oldest_idx != -1) {
        return (uint32_t) (oldest_idx);
    }

    min_access_time = (int) ((& (cache_system->stats))->accesses) + 1;
    for (int i = 0; i < cache_system->associativity; i++) {

        int *last_access_time = &((int *) replacement_policy->data)[set_idx * cache_system->associativity + i];

        if (*last_access_time < min_access_time) {
            oldest_idx = i;
            min_access_time = *last_access_time;
        }
    }

    return (uint32_t) (oldest_idx);
}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    free(replacement_policy->data);
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;

    lru_prefer_clean_rp->data = calloc(sets * associativity, sizeof(int));

    return lru_prefer_clean_rp;
}
