#include "net/multicast.h"
#include "config.h"
#include "util.h"
#include <gflags/gflags.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

DEFINE_string(
    interface, "",
    "Interface to use for multicast. If not specified, lets the kernel choose");

MulticastSocket JoinMulticastGroup(int port, bool listen) {
  int fd;
  SYSCALL(fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP));

  int on = 1;
  int hops = 255;
  SYSCALL(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on));
  SYSCALL(
      setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops)));
  SYSCALL(setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &on, sizeof(on)));

  struct sockaddr_in6 addr = {AF_INET6, htons(port)};
  if (listen) {
    addr.sin6_addr = in6addr_any;
    SYSCALL(bind(fd, (struct sockaddr *)&addr, sizeof addr));
  }
  SYSCALL_F(inet_pton(AF_INET6, kMulticastAddress, &addr.sin6_addr), 0);

  unsigned int if_index = 0;
  if (FLAGS_interface != "") {
    SYSCALL_F(if_index = if_nametoindex(FLAGS_interface.c_str()), 0);
  }

  struct ipv6_mreq group {};
  group.ipv6mr_interface = if_index;
  memcpy(&group.ipv6mr_multiaddr, &addr.sin6_addr, sizeof addr.sin6_addr);
  SYSCALL(
      setsockopt(fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof group));

  if (group.ipv6mr_interface != 0) {
    SYSCALL(setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &if_index,
                       sizeof if_index));
  }

  return MulticastSocket(fd, addr);
}
