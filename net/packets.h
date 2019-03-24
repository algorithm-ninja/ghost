#pragma once
#include <cstdint>

namespace Packets {

struct PacketHeader {
    uint8_t magic[3];
    uint8_t version;
    uint8_t type;
    uint8_t flags;
} __attribute__((packed));

struct ClientHello {
    PacketHeader hdr;
    uint8_t client_version;
    uint8_t filename[256];
} __attribute__((packed));

struct ServerHello {
    PacketHeader hdr;
    uint32_t file_id;
    uint8_t url[256];

    enum Flags {
        UPDATE_CLIENT = 1 << 0
    };
} __attribute__((packed));

struct ChunkListRequest {
    PacketHeader hdr;
    uint32_t file_id;
    uint64_t hash[128];

    enum Flags {
        REQUEST_HEADER = 1 << 0
    };
} __attribute__((packed));

struct Interval {
    uint32_t start;
    uint32_t end;
} __attribute__((packed));

struct ChunkRequest {
    PacketHeader hdr;
    uint32_t file_id;
    Interval intervals[128];
} __attribute__((packed));

struct ClientStatus {
    PacketHeader hdr;

} __attribute__((packed));

struct ChunkListHeader {
    PacketHeader hdr;
    uint32_t file_id;
    uint32_t chunk_count;
} __attribute__((packed));

struct ChunkList {
    PacketHeader hdr;
    uint32_t file_id;
    uint32_t start_idx;
    uint64_t hash[128];
} __attribute__((packed));

struct Chunk {
    PacketHeader hdr;
    uint64_t hash;
    uint32_t file_id;
    uint32_t start_idx;
    uint8_t data[1024];

    enum Flags {
        IS_LAST = 1 << 0,
        IS_COMPRESSED = 1 << 1
    };
} __attribute__((packed));

}
