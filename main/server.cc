#include "net/multicast.h"
#include <gflags/gflags.h>

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  MulticastSocket sock = JoinMulticastGroup(1234, false);
  while (true) {
    sleep(1);
    const char c[] = "ciao";
    sock.send(c, sizeof c);
  }
}
