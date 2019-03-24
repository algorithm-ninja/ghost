#include "hash/hash.h"
#include "config.h"
#include "highwayhash/highwayhash.h"
#include "util.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace highwayhash;

std::vector<uint64_t> HashFile(const std::string &path) {
  int fd;
  SYSCALL(fd = open(path.c_str(), O_RDONLY));
  off_t len;
  SYSCALL(len = lseek(fd, 0, SEEK_END));
  SYSCALL(lseek(fd, 0, SEEK_SET));
  char *addr;
  SYSCALL_F(addr = (char *)mmap(nullptr, len, PROT_READ, MAP_SHARED, fd, 0),
            nullptr);

  off_t num_chunk = (len + kChunkSize - 1) / kChunkSize;

  std::vector<uint64_t> ret(num_chunk);

  static const HHKey key HH_ALIGNAS(32) = {1, 2, 3, 4};

#pragma omp parallel for
  for (size_t i = 0; i < num_chunk; i++) {
    size_t off = i * kChunkSize;
    size_t sz = std::max<size_t>(len, (i + 1) * kChunkSize) - off;
    HHResult64 result;
    HHStateT<HH_TARGET> state(key);
    HighwayHashT(&state, addr + off, sz, &result);
    ret[i] = result;
  }
  return ret;
}
