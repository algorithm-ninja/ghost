#pragma once
#include "config.h"
#include "util.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

namespace net {

struct PacketHeader {
  uint8_t magic[3] = {'i', 'o', 'i'};
  // On messages from the client, represents the version of the client.
  // On messages from the server, represents the minimum required client
  // version.
  uint8_t version = kVersion;
  uint8_t type;
  uint8_t flags;
} __attribute__((packed));

struct Interval {
  uint32_t start;
  uint32_t end;
} __attribute__((packed));

// Packets sent from the client to the server.
struct ClientHello {
  static constexpr uint8_t kType = 0;
  PacketHeader hdr;
  uint8_t client_version;
  uint8_t filename[256];

  bool Write(int fd) const;
  ClientHello() { hdr.type = kType; }
} __attribute__((packed));

struct ChunkListRequest {
  static constexpr uint8_t kType = 1;
  PacketHeader hdr;
  uint32_t file_id;
  uint64_t hash[128];

  uint8_t unserialized_num_hashes = 0;

  enum Flags { kRequestFileHeader = 0b00000001 };

  bool Write(int fd) const;
  ChunkListRequest() { hdr.type = kType; }
} __attribute__((packed));

struct ClientStatus {
  static constexpr uint8_t kType = 2;
  PacketHeader hdr;

  bool Write(int fd) const;
  ClientStatus() { hdr.type = kType; }
} __attribute__((packed));

// Server to client.
struct ServerHello {
  static constexpr uint8_t kType = 3;
  PacketHeader hdr;
  uint32_t file_id;
  // URL for updates.
  uint8_t url[256];

  enum Flags { kUpdateClient = 0b00000001 };

  bool Write(int fd) const;
  ServerHello() { hdr.type = kType; }
} __attribute__((packed));

struct ChunkRequest {
  static constexpr uint8_t kType = 4;
  PacketHeader hdr;
  uint32_t file_id;
  Interval intervals[128];

  uint8_t unserialized_num_intervals;

  bool Write(int fd) const;
  ChunkRequest() { hdr.type = kType; }
} __attribute__((packed));

struct ChunkListHeader {
  static constexpr uint8_t kType = 5;
  PacketHeader hdr;
  uint32_t file_id;
  uint32_t chunk_count;

  bool Write(int fd) const;
  ChunkListHeader() { hdr.type = kType; }
} __attribute__((packed));

struct ChunkList {
  static constexpr uint8_t kType = 6;
  PacketHeader hdr;
  uint32_t file_id;
  uint32_t start_idx;
  uint64_t hash[128];

  uint8_t unserialized_num_hashes = 0;

  bool Write(int fd) const;
  ChunkList() { hdr.type = kType; }
} __attribute__((packed));

struct Chunk {
  static constexpr uint8_t kType = 7;
  PacketHeader hdr;
  uint64_t hash;
  uint32_t file_id;
  uint8_t start_idx;
  uint8_t data[1024 + 64]; // Up to 64 bytes of extra info.

  uint16_t unserialized_data_size = 0;

  enum Flags { kIsLast = 0b00000001, kIsCompressed = 0b00000010 };

  bool Write(int fd) const;
  Chunk() { hdr.type = kType; }
} __attribute__((packed));

template <typename F> bool Read(int fd, const F &callback) {
  const constexpr uint32_t kMaxPacketSize = sizeof(Chunk);
  uint8_t data[kMaxPacketSize] = {};
  int32_t len = 0;

  struct sockaddr_in6 addr = {};
  socklen_t address_len = sizeof addr;

  if ((len = recvfrom(fd, data, kMaxPacketSize, 0, (sockaddr *)&addr,
                      &address_len)) == -1) {
    return false;
  }

  char src_ip[INET6_ADDRSTRLEN + 1] = {};
  SYSCALL_F(inet_ntop(AF_INET6, &addr, src_ip, sizeof src_ip), nullptr);

  const PacketHeader *hdr = (PacketHeader *)data;
  switch (hdr->type) {
  case ClientHello::kType: {
    auto pkt = (ClientHello *)data;
    return callback(*pkt);
  }
  case ChunkListRequest::kType: {
    auto pkt = (ChunkListRequest *)data;
    pkt->unserialized_num_hashes =
        (len - offsetof(ChunkListRequest, hash)) / sizeof *pkt->hash;
    return callback(src_ip, *pkt);
  }
  case ClientStatus::kType: {
    auto pkt = (ClientStatus *)data;
    return callback(src_ip, *pkt);
  }
  case ServerHello::kType: {
    auto pkt = (ServerHello *)data;
    return callback(src_ip, *pkt);
  }
  case ChunkRequest::kType: {
    auto pkt = (ChunkRequest *)data;
    pkt->unserialized_num_intervals =
        (len - offsetof(ChunkRequest, intervals)) / sizeof *pkt->intervals;
    return callback(src_ip, *pkt);
  }
  case ChunkListHeader::kType: {
    auto pkt = (ChunkListHeader *)data;
    return callback(src_ip, *pkt);
  }
  case ChunkList::kType: {
    auto pkt = (ChunkList *)data;
    pkt->unserialized_num_hashes =
        (len - offsetof(ChunkList, hash)) / sizeof *pkt->hash;
    return callback(src_ip, *pkt);
  }
  case Chunk::kType: {
    auto pkt = (Chunk *)data;
    pkt->unserialized_data_size =
        (len - offsetof(Chunk, data) - pkt->start_idx) / sizeof *pkt->data;
    return callback(src_ip, *pkt);
  }
  default:
    return false;
  };
}

} // namespace net
