#include "file/hash.h"
#include "config.h"
#include "file/file.h"
#include "highwayhash/highwayhash.h"

using namespace highwayhash;

std::vector<uint64_t> HashFile(const std::string &path) {
  File f(path);

  size_t num_chunk = (f.size() + kChunkSize - 1) / kChunkSize;

  std::vector<uint64_t> ret(num_chunk);

  static const HHKey key HH_ALIGNAS(32) = {1, 2, 3, 4};

#pragma omp parallel for
  for (size_t i = 0; i < num_chunk; i++) {
    size_t off = i * kChunkSize;
    size_t sz = std::max<size_t>(f.size(), (i + 1) * kChunkSize) - off;
    HHResult64 result;
    HHStateT<HH_TARGET> state(key);
    HighwayHashT(&state, f.data() + off, sz, &result);
    ret[i] = result;
  }
  return ret;
}
