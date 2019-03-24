#pragma once
#include "config.h"
#include <cstdint>

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
} __attribute__((packed));

struct ChunkListRequest {
  static constexpr uint8_t kType = 1;
  PacketHeader hdr;
  uint32_t file_id;
  uint64_t hash[128];

  uint8_t unserialized_num_hashes = 0;

  enum Flags { kRequestFileHeader = 0b00000001 };

  bool Write(int fd) const;
} __attribute__((packed));

struct ClientStatus {
  static constexpr uint8_t kType = 2;
  PacketHeader hdr;
  uint64_t client_id;

  bool Write(int fd) const;
} __attribute__((packed));

// Server to client.
struct ServerHello {
  static constexpr uint8_t kType = 3;
  PacketHeader hdr;
  uint32_t file_id;
  uint64_t client_id;
  // URL for updates.
  uint8_t url[256];

  enum Flags { kUpdateClient = 0b00000001 };

  bool Write(int fd) const;
} __attribute__((packed));

struct ChunkRequest {
  static constexpr uint8_t kType = 4;
  PacketHeader hdr;
  uint32_t file_id;
  Interval intervals[128];

  uint8_t unserialized_num_intervals;

  bool Write(int fd) const;
} __attribute__((packed));

struct ChunkListHeader {
  static constexpr uint8_t kType = 5;
  PacketHeader hdr;
  uint32_t file_id;
  uint32_t chunk_count;

  bool Write(int fd) const;
} __attribute__((packed));

struct ChunkList {
  static constexpr uint8_t kType = 6;
  PacketHeader hdr;
  uint32_t file_id;
  uint32_t start_idx;
  uint64_t hash[128];

  uint8_t unserialized_num_hashes = 0;

  bool Write(int fd) const;
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
} __attribute__((packed));

} // namespace net
