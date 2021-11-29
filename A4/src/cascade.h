
#pragma once
#include "sha256.h"
#include "common.h"

#ifndef CASCADE_H
#define CASCADE_H
#define MAIN_ARGNUM                 5   // number of command line arguments to main().
#define BODY_SIZE                   38
#define HEADER_SIZE                 16
#define MESSAGE_SIZE                54
#define REPLY_HEADER_SIZE           5
#define PEER_REQUEST_DELAY          10
#define PEER_REQUEST_HEADER_SIZE    64
#define PEER_RESPONSE_HEADER_SIZE   9
#define MAX_LINE                    128
#define MAX_CONNECTIONS             12

typedef uint8_t hashdata_t[SHA256_HASH_SIZE];

struct RequestHeader {
    char protocol[4];
    unsigned int version;
    unsigned int command;
    unsigned int length;
};

struct RequestBody {
    hashdata_t hash;
    struct in_addr ip;
    unsigned short port;
};

struct ClientRequest {
    char protocol[8];
    char reserved[16];
    uint64_t block_num;
    hashdata_t hash;
};

struct ClientResponseHeader {
    char error[1];
    uint64_t length;
};

#include <stdint.h>

typedef struct csc_ipport {
    uint32_t ip;
    uint16_t port;
} csc_ipport_t;

typedef struct csc_block {
    uint64_t index;
    uint64_t offset;
    uint64_t length;
    uint8_t completed;
    hashdata_t hash;
} csc_block_t;

typedef struct csc_file {
    uint64_t targetsize;
    uint64_t blocksize;
    hashdata_t targethash;
    uint64_t trailblocksize;
    uint64_t blockcount;
    csc_block_t* blocks;
    // nye
    int uncomp_count;
    char* output_file;
} csc_file_t;

typedef struct csc_peer {
    char ip[16];
    char port[8];
    uint32_t lastseen;
    uint8_t good;
} csc_peer_t;

typedef struct socket_info {
    int connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
} socket_info_t;

/*
 * Parses a hex-string and returns the bytes corresponding to the value
 */
uint8_t* hex_to_bytes(const char* string);

/*
 * Parses a cascade file, given the sourcepath input and destination, which may or may not exist.
 * Returns a pointer to a datastructure describing the file, or NULL if the file could not be parsed
 */
csc_file_t* csc_parse_file(const char* sourcefile, const char* destination);

/*
 * Releases the memory allocated by a file datastructure
 */
void csc_free_file(csc_file_t* sourcefile);

/*
 * Attempts to get a list of peers from the tracker.
 * The peers should contain a pre-allocated set of memory that can hold at least peercount elements.
 * If the function is successful, the return code is zero and the value of peercount has been updated to contain the number of peers.
 * If the function fails, the return code is non-zero
 */
int csc_get_peers(csc_ipport_t tracker, hashdata_t cascadehash, csc_ipport_t localaddress, uint8_t subscribe, uint32_t* peercount, csc_peer_t* peers);

/*
 * Downloads a block of data from a client.
 * The supplied buffer should be at least large enough to contain blocklength-bytes.
 * If the function is successful, the return code is zero, and the buffer contains data of the size requested by blocklength.
 * If the function fails, the return code is non-zero, and will reflect the peer statuscode (if given).
 */
int csc_download_block(csc_ipport_t client, hashdata_t cascadehash, uint64_t blockno, uint64_t blocklength, void* buffer);

int get_peers_list(hashdata_t hash);
void get_block(csc_block_t* block, csc_peer_t peer, hashdata_t hash, char* output_file);

#endif
