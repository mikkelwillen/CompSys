
#pragma once
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

struct RequestHeader
{
  char protocol[4];
  unsigned int version; 
  unsigned int command; 
  unsigned int length; 
};

struct RequestBody
{
  char hash[32];
  struct in_addr ip;
  unsigned short port;
};

struct ClientRequest
{
    char protocol[8];
    char reserved[16];
    uint64_t block_num;
    char hash[32];
};

struct ClientResponseHeader
{
    char error[1];
    uint64_t length; 
};

#include <stdint.h>

typedef struct csc_hashdata { uint8_t x[32]; } csc_hashdata_t;

typedef struct csc_ipport { 
    uint32_t ip; 
    uint16_t port;
} csc_ipport_t;

typedef struct csc_block {
    uint64_t index;             // The index of this block in the data file
    uint64_t offset;            // Number of bytes the start of this block is offset from the start of the file
    uint64_t length;            // Number of bytes within this block
    uint8_t completed;          // Flag of if this block is present in local file system or not
    csc_hashdata_t hash;        // Hash of this blocks bytes
} csc_block_t;

typedef struct csc_file {
    uint64_t targetsize;        // Size of completed data file
    uint64_t blocksize;         // Size of individual blocks data is divided into
    csc_hashdata_t targethash;  // Hash of complete data file
    uint64_t trailblocksize;    // Size of last block. Will differ from standard block size if data does not evenly divide amoungst blocks
    uint64_t blockcount;        // Number of blocks data is divided into
    csc_block_t* blocks;        // Pointer to array of all blocks
} csc_file_t;

typedef struct csc_peer { 
    char ip[16];                // IP of a peer
    char port[8];               // Port of a peer
    uint32_t lastseen;          // Timestamp of last time Tracker saw this Peer
    uint8_t good;               // Flag for if this is 'Good' peer. e.g. Always provides valid responses according to protocol
} csc_peer_t;

typedef struct csc_server {
} csc_server_t;

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
int csc_get_peers(csc_ipport_t tracker, csc_hashdata_t cascadehash, csc_ipport_t localaddress, uint8_t subscribe, uint32_t* peercount, csc_peer_t* peers);

/*
 * Downloads a block of data from a client.
 * The supplied buffer should be at least large enough to contain blocklength-bytes.
 * If the function is successful, the return code is zero, and the buffer contains data of the size requested by blocklength.
 * If the function fails, the return code is non-zero, and will reflect the peer statuscode (if given).
 */
int csc_download_block(csc_ipport_t client, csc_hashdata_t cascadehash, uint64_t blockno, uint64_t blocklength, void* buffer);

int get_peers_list(csc_peer_t** peers, unsigned char* hash);
void get_block(csc_block_t* block, csc_peer_t peer, unsigned char* hash, char* output_file);

#endif
