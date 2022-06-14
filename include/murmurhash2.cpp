#include <cstdint>

static inline uint32_t getblock ( const uint32_t * p )
{
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return *p;
#else
  const uint8_t *c = (const uint8_t *)p;
  return (uint32_t)c[0] |
	 (uint32_t)c[1] <<  8 |
	 (uint32_t)c[2] << 16 |
	 (uint32_t)c[3] << 24;
#endif
}

static inline uint64_t getblock ( const uint64_t * p )
{
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return *p;
#else
  const uint8_t *c = (const uint8_t *)p;
  return (uint64_t)c[0] |
	 (uint64_t)c[1] <<  8 |
	 (uint64_t)c[2] << 16 |
	 (uint64_t)c[3] << 24 |
	 (uint64_t)c[4] << 32 |
	 (uint64_t)c[5] << 40 |
	 (uint64_t)c[6] << 48 |
	 (uint64_t)c[7] << 56;
#endif
}

//-----------------------------------------------------------------------------

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = getblock((const uint32_t *)data);

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 