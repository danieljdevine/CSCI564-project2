1. Describe how your prefetcher works.
My prefetcher uses a form of strided prefetching. The basic concept is that if we see an access pattern of b, b+N, b+2N, then we will assume there is a stride of size N and prefetch b+3N. The implementation is as follows:

When a miss happens, I check if the address if in an array of N previous address misses. 

If it is not in the array, I replace the least recently accessed address with a new access. I find the previously accessed address and set the current stride to `current_address - previous_address`. In this case, we do not prefetch anything.

If the address is in the array, I update its stride to `current_address - previous_address` and check if the current stride is is equal to the stride of the previous address. If it is, then we will fetch `current_address + stride`, otherwise we will not prefetch anything.

2. Explain how you chose that prefetch strategy.

I arrived at this strategy because I found it to be a common implementation in hardware across a lot of systems. It also seemed like a logical approach to prefetching. Programs use a lot of loops that may look something like:
```
for i from 0 to N
   access array[i*2]
```
which will not be caught under adjacent and may even be missed by sequential prefetching.

3. Discuss the pros and cons of your prefetch strategy.
The pros are that it has all of the advantages of adjacent prefetching with the option to fetch larger strides and it is a logical approach. The cons are that it incurs a space cost to store the previous accesses. In order for it to be a viable strategy, the previous access array must be a decently sized array. In hardware this could take the form of a cache, which will incur more cost for the chip to be made.

4. Demonstrate that the prefetcher could be implemented in hardware (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).
The prefetcher would use the same concept as a cache. We would store a tag, the address, the stride, and the access time. When the cache misses, we would check the cache for a tag representing the address in the cache. If the cache did not have the tag, we would evict the least recently used address. If the cache did contain the tag, we would compare strides as described in **1**.

5. Cite any additional sources that you used to develop your prefetcher.
https://www.youtube.com/watch?v=cDh1AeJ7fLo
