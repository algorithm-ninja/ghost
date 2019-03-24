#include "file/hash.h"
#include "net/multicast.h"
#include <gflags/gflags.h>

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  fprintf(stderr, "%s chunk list\n", argv[0]);
  for (uint64_t hash : HashFile(argv[0])) {
    fprintf(stderr, "%016lx\n", hash);
  }
  MulticastSocket sock = JoinMulticastGroup(1234, false);
  while (true) {
    sleep(1);
    const char c[] = "ciao";
    sock.send(c, sizeof c);
  }
}
