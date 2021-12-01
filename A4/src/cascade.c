 #include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#ifdef __APPLE__
#include "./endian.h"
#else
#include <endian.h>
#endif

#include "./cascade.h"
#include "./sha256.h"

char tracker_ip[IP_LEN];
char tracker_port[PORT_LEN];
char my_ip[IP_LEN];
char my_port[PORT_LEN];

struct csc_file** casc_files;
struct socket_info** connections;
int number_of_connections = 0;

/*
 * Frees global resources that are malloc'ed during peer downloads.
 */

// der skal free's alle peers og alle casc_files
void free_resources() {
    // free(casc_files[0]->peers);
    // csc_free_file(*casc_files);
}


/*
 * Gets a sha256 hash of specified data, sourcedata. The hash itself is
 * placed into the given variable 'hash'. Any size can be created, but a
 * a normal size for the hash would be given by the global variable
 * 'SHA256_HASH_SIZE', that has been defined in sha256.h
 */
void get_data_sha(const char* sourcedata, hashdata_t hash, uint32_t data_size, int hash_size) {
    SHA256_CTX shactx;
    unsigned char shabuffer[hash_size];
    sha256_init(&shactx);
    sha256_update(&shactx, sourcedata, data_size);
    sha256_final(&shactx, shabuffer);

    for (int i = 0; i < hash_size; i++) {
        hash[i] = shabuffer[i];
    }
}


/*
 * Gets a sha256 hash of a specified file, sourcefile. The hash itself is
 * placed into the given variable 'hash'. Any size can be created, but a
 * a normal size for the hash would be given by the global variable
 * 'SHA256_HASH_SIZE', that has been defined in sha256.h
 */
void get_file_sha(const char* sourcefile, hashdata_t hash, int size) {
    int casc_file_size;

    FILE* fp = Fopen(sourcefile, "rb");
    if (fp == 0) {
        printf("Failed to open source: %s\n", sourcefile);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    casc_file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char buffer[casc_file_size];
    Fread(buffer, casc_file_size, 1, fp);
    Fclose(fp);

    get_data_sha(buffer, hash, casc_file_size, size);
}

void check_txt_file(char* cascade_file, int i) {
    if (access(cascade_file, F_OK ) != 0 ) {
        fprintf(stderr, ">> File %s does not exist\n", cascade_file);
        exit(EXIT_FAILURE);
    }
    printf("%s", cascade_file);
    char** output_files_temp = Malloc(strlen(cascade_file));
    printf("er det her2?\n");
    char* name = Malloc(strlen(cascade_file));
    printf("er det her3?\n");
    memcpy(name, cascade_file, strlen(cascade_file));
    printf("er det her4?\n");
    memcpy(output_files_temp, cascade_file, strlen(cascade_file));
    printf("er det her5?\n");
    char* r = strstr(cascade_file, "cascade");
    printf("er det her6?\n");
    int cutoff = r - cascade_file;
    printf("%d", cutoff);
    printf("output før: %s\n", output_files_temp);
    output_files_temp[cutoff - 1] = '\0';
    printf("output efter: %s\n", output_files_temp);
    char* new_temp = (char*) output_files_temp;
    new_temp[cutoff - 1] = '\0';
    printf("%s\n", new_temp);
    casc_files[i] = csc_parse_file(cascade_file, new_temp);
    casc_files[i]->output_file = output_files_temp;

    casc_files[i]->uncomp_count = 0;
    casc_files[i]->index = i;
    casc_files[i]->name = name;
    printf("wat\n");
    casc_files[i]->missing_blocks = Malloc(sizeof(csc_block_t) * casc_files[i]->blockcount);
    
    printf("Blockcount: %d", casc_files[i]->blockcount);
    for (uint64_t j = 0; j < casc_files[i]->blockcount; j++) {
        if (casc_files[i]->blocks[j].completed == 0) {
            casc_files[i]->missing_blocks[casc_files[i]->uncomp_count] = casc_files[i]->blocks[j];
            casc_files[i]->uncomp_count++;
        }
    }

    if (casc_files[i]->uncomp_count == 0) {
        printf("All blocks are already present, skipping external connections.\n");
        free_resources();
        casc_files[i]->got_all_blocks = 1;
    }

    hashdata_t hash_buf;
    get_file_sha(casc_files[i]->name, hash_buf, SHA256_HASH_SIZE);
    *casc_files[i]->hash = Malloc(sizeof(hashdata_t));
    memcpy(casc_files[i]->hash, hash_buf, SHA256_HASH_SIZE);
    printf("check_txt_file succesful");
}


/*
 * Perform all client based interactions in the P2P network for a given cascade file.
 * E.g. parse a cascade file and get all the relevent data from somewhere else on the
 * network.
 */
void download(void* vargp) {
    printf("Inside download\n");
    csc_file_t* cascade_file = ((csc_file_t *)vargp);
    printf("Managing download only for: %s\n", cascade_file->name);

    csc_peer_t peer = cascade_file->peers[0];
    printf("Check peer\n");
    // Get a good peer if one is available
    for (int i = 0; i < cascade_file->peercount; i++) {
        printf("Inde i forløkken i download\n");
        if (cascade_file->peers[i].good) {
            peer = cascade_file->peers[i];
            break;
        }
    }

    printf("Downloading blocks\n");
    for (int i = 0; i < cascade_file->uncomp_count; i++) {
        printf("forloop download blocks\n");
        get_block(&cascade_file->missing_blocks[i], peer, cascade_file->hash, cascade_file->output_file);
        printf("efter get_block\n");
    }

    printf("File downloaded successfully\n");
    cascade_file->got_all_blocks = 1;

    free_resources();

    Pthread_detach(pthread_self());
}


/*
 * Count how many times a character occurs in a string
 */
int count_occurences(char string[], char c) {
    int count = 0;
    for(size_t i = 0; i < strlen(string); i++) {
        if(string[i] == c) {
            count++;
        }
    }

    return count;
}


/*
 * Parses a cascade file, given the sourcepath input and destination, which may or may not exist.
 * Returns a pointer to a datastructure describing the file, or NULL if the file could not be parsed
 */
csc_file_t* csc_parse_file(const char* sourcefile, const char* destination) {
    FILE* fp = Fopen(sourcefile, "rb");
    if (fp == 0) {
        printf("Failed to open source: %s\n", sourcefile);
        return NULL;
    }

    const int FILE_HEADER_SIZE = 8 + 8 + 8 + 8 + 32;

    char header[FILE_HEADER_SIZE];
    if (Fread(header, 1, FILE_HEADER_SIZE, fp) != (size_t)FILE_HEADER_SIZE) {
        printf("Failed to read magic 8 bytes header from file\n");
        Fclose(fp);
        return NULL;
    }

    if (memcmp(header, "CASCADE1", 8) != 0) {
        printf("File does not contain magic 8 bytes in header\n");
        Fclose(fp);
        return NULL;
    }

    csc_file_t* res = (csc_file_t*) Malloc(sizeof(csc_file_t));
    res->targetsize = be64toh(*((unsigned long long*)&header[16]));
    res->blocksize = be64toh(*((unsigned long long*)&header[24]));
    memcpy(&res->targethash, &header[32], 32);
    res->trailblocksize = res->targetsize % res->blocksize;
    if (res->trailblocksize == 0) // If the filesize is evenly divisible by the blocksize
        res->trailblocksize = res->blocksize;

    res->blockcount = (res->targetsize + res->blocksize - 1) / res->blocksize;

    res->blocks = (csc_block_t*)Malloc(sizeof(csc_block_t) * res->blockcount);
    if (res->blocks == NULL) {
        printf("Failed to allocate memory for %lud blocks\n", res->blockcount);
        Free(res);
        Fclose(fp);
        return NULL;
    }

    for(uint64_t i = 0; i < res->blockcount; i++) {
        csc_block_t* b = &res->blocks[i];
        b->index = i;
        b->length = i == res->blockcount - 1 ? res->trailblocksize : res->blocksize;
        b->offset = i * res->blocksize;
        b->completed = 0;
        if (Fread(&(b->hash), 1, sizeof(hashdata_t), fp) != sizeof(hashdata_t)) {
            printf("Failed to read block %lud/%lud\n", i, res->blockcount);
            csc_free_file(res);
            Fclose(fp);
            return NULL;
        }
    }

    Fclose(fp);

    fp = Fopen(destination, "a+w");
    if (fp == NULL) {
        printf("Failed to open destination file %s\n", destination);
        csc_free_file(res);
        return NULL;
    }

    void* buffer = Malloc(res->blocksize);
    if (buffer == NULL) {
        printf("No block buffer asigned: %lud\n", res->blocksize);
        csc_free_file(res);
        Fclose(fp);
        return NULL;
    }

    SHA256_CTX shactx;
    for(uint64_t i = 0; i < res->blockcount; i++) {
        hashdata_t shabuffer;
        uint64_t size = res->blocks[i].length;
        if (fread(buffer, size, 1, fp) != 1) {
            break;
        }

        sha256_init(&shactx);
        sha256_update(&shactx, buffer, size);
        sha256_final(&shactx, shabuffer);

        if (memcmp(&res->blocks[i].hash, shabuffer, SHA256_HASH_SIZE) != 0) {
            break;
        }

        res->blocks[i].completed = 1;
    }

    Fclose(fp);
    Free(buffer);
    printf("færdig med parse\n");

    return res;
}

/*
 * Releases the memory allocated by a file datastructure
 */
void csc_free_file(csc_file_t* file) {
    Free(file->blocks);
    file->blocks = NULL;
    Free(file);
}

/*
 * Get a specified block from a peer on the network. The block is retrieved and then inserted directly into
 * the appropriate data file at the appropriate location.
 */
void get_block(csc_block_t* block, csc_peer_t peer, hashdata_t hash, char* output_file) {

    rio_t rio;
    char msg_buf[MAXLINE];

    int peer_socket = Open_clientfd(peer.ip, peer.port);
    Rio_readinitb(&rio, peer_socket);

    struct ClientRequest client_request;
    // memcpy as we don't have space for terminating null.
    memcpy(client_request.protocol, "CASCADE1", sizeof(client_request.protocol));

    client_request.block_num = htobe64(block->index);
    memcpy(client_request.hash, hash, SHA256_HASH_SIZE);

    memcpy(msg_buf, &client_request, PEER_REQUEST_HEADER_SIZE);

    Rio_writen(peer_socket, msg_buf, PEER_REQUEST_HEADER_SIZE);

    Rio_readlineb(&rio, msg_buf, MAXLINE);

    char reply_header[PEER_RESPONSE_HEADER_SIZE];
    memcpy(reply_header, msg_buf, PEER_RESPONSE_HEADER_SIZE);

    if (reply_header[0] != 0) {
        if (reply_header[0] == 1) {
            printf("Invalid hash. File not present on peer\n");
        } else if (reply_header[0] == 2) {
            printf("Invalid hash. Block not present on peer\n");
        } else if (reply_header[0] == 3) {
            printf("Invalid block number\n");
        } else if (reply_header[0] == 4) {
            printf("Request could not be parsed\n");
        } else {
            printf("Invalid error code\n");
        }
        Close(peer_socket);
        return;
    }

    uint64_t msglen = be64toh(*(uint64_t*)&reply_header[1]);

    if (msglen != block->length) {
        printf("Invalid block length supplied\n");
        Close(peer_socket);
        return;
    }

    // msglen + 1 to make room for terminating null byte.
    char *block_data = Calloc(msglen+1, sizeof(char));
    hashdata_t block_hash;

    strcpy(block_data, msg_buf + PEER_RESPONSE_HEADER_SIZE);

    uint64_t write_count = strlen(block_data);
    uint64_t to_read = msglen - write_count;
    uint64_t was_read = 0;

    while (write_count < msglen) {
        if (to_read > MAXLINE) {
            to_read = MAXLINE;
        }

        was_read = Rio_readnb(&rio, msg_buf, to_read);

        memcpy(block_data + write_count, msg_buf, was_read);

        write_count += was_read;
        to_read = msglen - write_count;
    }

    get_data_sha(block_data, block_hash, msglen, SHA256_HASH_SIZE);

    // Also make sure to check block hash is expected
    for (int i = 0; i < SHA256_HASH_SIZE; ++i) {
        if (block_hash[i] != block->hash[i]) {
            printf("Invalid block hash\n");
            Free(block_data);
            Close(peer_socket);
            return;
        }
    }

    FILE* fp = Fopen(output_file, "rb+");
    if (fp == 0) {
        printf("Failed to open destination: %s\n", output_file);
        free(block_data);
        Close(peer_socket);
        return;
    }

    fseek(fp, block->offset, SEEK_SET);
    Fputs(block_data, fp);

    Free(block_data);
    Close(peer_socket);
    Fclose(fp);
}

/*
 * Get a list of peers on the network from a tracker. Note that this query is doing double duty according to
 * the protocol, and by asking for a list of peers we are also enrolling on the network ourselves.
 */
void subscribe(csc_file_t* casc_file, int command) {
    rio_t rio;
    char msg_buf[MAXLINE];

    int tracker_socket = Open_clientfd(tracker_ip, tracker_port);
    Rio_readinitb(&rio, tracker_socket);

    struct RequestHeader request_header;
    // memcpy as it does not end with terminating null byte.
    memcpy(request_header.protocol, "CASC", sizeof(request_header.protocol));

    request_header.version = htonl(1);
    request_header.command = htonl(command);
    request_header.length = htonl(BODY_SIZE);
    memcpy(msg_buf, &request_header, HEADER_SIZE);

    struct in_addr byte_my_ip;
    inet_aton(my_ip, &byte_my_ip);

    struct RequestBody request_body;
    memcpy(request_body.hash, casc_file->hash, SHA256_HASH_SIZE);
    request_body.ip = byte_my_ip;
    request_body.port = htons(atoi(my_port));
    memcpy(msg_buf + HEADER_SIZE, &request_body, BODY_SIZE);

    Rio_writen(tracker_socket, msg_buf, MESSAGE_SIZE);

    if (command == 1) {
        printf("command er 1\n");
        Rio_readnb(&rio, msg_buf, MAXLINE);
        
        char reply_header[REPLY_HEADER_SIZE];
        memcpy(reply_header, msg_buf, REPLY_HEADER_SIZE);

        uint32_t msglen = ntohl(*(uint32_t*)&reply_header[1]);
        if (msglen == 0) {
            Close(tracker_socket);
        }

        if (reply_header[0] != 0) {
            char* error_buf = Malloc(msglen + 1);
            if (error_buf == NULL) {
                printf("Tracker error %d and out-of-memory reading error\n", reply_header[0]);
                Close(tracker_socket);
                return 0;
            }

            memset(error_buf, 0, msglen + 1);
            // memcpy(reply_header, error_buf, msglen);
            printf("Tracker gave error: %d - %s\n", reply_header[0], error_buf);
            Free(error_buf);
            Close(tracker_socket);
            return 0;
        }

        if (msglen % 12 != 0) {
            printf("LIST response from tracker was length %ud but should be evenly divisible by 12\n", msglen);
            Close(tracker_socket);
            return 0;
        }

        unsigned char body[msglen];
        memcpy(body, msg_buf+REPLY_HEADER_SIZE, msglen);

        int peercount = 0;
        peercount = (uint32_t)(msglen / 12);
        casc_file->peers = Malloc(sizeof(csc_peer_t) * peercount);
        casc_file->peercount = peercount;
        printf("før forløkke i subscribe\n");
        for(int i = 0; i < peercount; i++) {
            uint8_t peerdata[12];
            memcpy(peerdata, body+(12*i), 12);        
                
            uint32_t ip_buf;        
            memcpy(&ip_buf, peerdata, 4);

            char ip[IP_LEN];
            struct in_addr ip_addr;
            ip_addr.s_addr = ip_buf;
            memcpy(ip, inet_ntoa(ip_addr), IP_LEN);    
                    
            char port_buf[PORT_LEN]; 
            sprintf(port_buf, "%1d", ntohs(*((uint16_t*)&peerdata[4])));       
        
            uint32_t lastseen = *((uint32_t*)&peerdata[6]);

            memcpy(&casc_file->peers[i].ip, ip, IP_LEN);
            memcpy(&casc_file->peers[i].port, port_buf, PORT_LEN);
            casc_file->peers[i].lastseen = ntohl(lastseen);
            casc_file->peers[i].good = peerdata[10];

            printf("Got peer with IP: %s, and Port: %s\n", ip, port_buf);
        }
    }
    
    Close(tracker_socket); //Skal vi lukke connection???
}

void upload(void* vargp) {
    int connfd = *((int *)vargp);

    // det giver sgu ikke helt mening det her :)
    char msg_buf[MAXLINE];
    rio_t rio;

    Rio_readnb(&rio, msg_buf, MAXLINE);

    char request_header[PEER_REQUEST_HEADER_SIZE];
    memcpy(request_header, msg_buf, PEER_REQUEST_HEADER_SIZE);

    uint64_t msglen = be64toh(*(uint64_t*)&request_header[0]);

    if (msglen == 0) {
            // send errorcode
    }

    if (msglen != 64) {
        // send error code: not the right format
    }
    
    char* block_data = Calloc(msglen+1, sizeof(char));
    hashdata_t block_hash;
    struct ClientRequest* request = Malloc(sizeof(struct ClientRequest));

    strcpy(block_data, msg_buf);
    memcpy(request->protocol, block_data, 8);
    memcpy(request->reserved, block_data + 8, 16);
    memcpy(request->block_num, block_data + 24, 8);
    memcpy(request->hash, block_data + 32, 32);

    if (request->protocol != "CASCADE1") {
        // send error: forkert protocol
    }

    
}

/*
 * The entry point for the code. Parses command line arguments and starts up the appropriate peer code.
 */
int main(int argc, char **argv) {
    if (argc != MAIN_ARGNUM + 1) {
        fprintf(stderr, "Usage: %s <cascade file(s)> <tracker server ip> <tracker server port> <peer ip> <peer port>.\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (!is_valid_ip(argv[2])) {
        fprintf(stderr, ">> Invalid tracker IP: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    } else if (!is_valid_port(argv[3])) {
        fprintf(stderr, ">> Invalid tracker port: %s\n", argv[3]);
        exit(EXIT_FAILURE);
    } else if (!is_valid_ip(argv[4])) {
        fprintf(stderr, ">> Invalid peer IP: %s\n", argv[4]);
        exit(EXIT_FAILURE);
    } else if (!is_valid_port(argv[5])) {
        fprintf(stderr, ">> Invalid peer port: %s\n", argv[5]);
        exit(EXIT_FAILURE);
    }

    snprintf(tracker_ip,   IP_LEN, "%s",   argv[2]);
    snprintf(tracker_port, PORT_LEN, "%s", argv[3]);
    snprintf(my_ip,   IP_LEN, "%s",   argv[4]);
    snprintf(my_port, PORT_LEN, "%s", argv[5]);
    pthread_t tid;

    char cas_str[strlen(argv[1])];
    snprintf(cas_str, strlen(argv[1]) + 1, "%s", argv[1]);
    char delim[] = ":";

    int casc_count = count_occurences(argv[1], ':') + 1;
    char* cascade_files[casc_count];
    printf("cascade array made\n");

    char* ptr = strtok(cas_str, delim);
    int i = 0;

    while (ptr != NULL) {
        if (strstr(ptr, ".cascade") != NULL) {
            cascade_files[i++] = ptr;
            ptr = strtok(NULL, delim);
        } else {
            printf("Abort on %s\n", ptr);
            fprintf(stderr, ">> Invalid cascade file: %s\n", ptr);
            exit(EXIT_FAILURE);
        }
    }

    casc_files = Malloc(sizeof(csc_file_t) * casc_count);
    printf("inden txt check\n");
    // Laver en csc_file og sætter den en i det globale csc_files array
    for (int j = 0; j < casc_count; j++) {
        check_txt_file(cascade_files[j], j); // Thread?
    }

    printf("check_txt_file run\n");
    // For hver csc_file subscriber vi 
    for (int j = 0; j < casc_count; j++) {
        if (casc_files[j]->got_all_blocks == 0) {
            subscribe(casc_files[j], 1); // 1 = get peers list
        } else {
            subscribe(casc_files[j], 2); // 2 = subscribe
        }   
    }
    printf("subscribe run\n");
    printf("Peer port: %d\n", casc_files[0]->peers[0].port);
    printf("casc count: %d\n", casc_count);

    for (int j = 0; j < casc_count; j++) {
        printf("har alle blocks: %d\n", casc_files[j]->got_all_blocks);
        if (!casc_files[j]->got_all_blocks) {
            Pthread_create(&tid, NULL, download, casc_files[j]);
        }
    }
    printf("Efter download\n");

    // åbner port til at lytte
    int listenfd = Open_listenfd(my_port);
    connections = Malloc(sizeof(socket_info_t) * MAX_CONNECTIONS);

    // tjekker for forbindelser og sætter ind i array
    printf("connection sat op\n");
    socklen_t clientlen = sizeof(struct sockaddr_storage);
    struct sockaddr_storage clientaddr;
    int* connfdp;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    while(1) {
        printf("inde i while\n");
        connfdp = Malloc(sizeof(int));
        socket_info_t* new_connection = Malloc(sizeof(socket_info_t));
        
        *connfdp = Accept(listenfd, (SA*) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE,
                    client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n",
           client_hostname, client_port);

        new_connection->clientaddr = clientaddr;
        new_connection->clientlen = clientlen;
        new_connection->connfdp = *(int*)connfdp;
        if (number_of_connections < MAX_CONNECTIONS) {
            struct UploadData* upload_data = Malloc(sizeof(struct UploadData));
            upload_data->connection = new_connection;
            upload_data->ostehaps = connfdp;
            // mere data i upload_data
            Pthread_create(&tid, NULL, upload, upload_data);
        }

    }
}


// søg på:
// Skal vi lukke connection???

// Den fejler når vi allerede har txt-filerne
// Den prøver at connecte med fx python-peer selvom den er lukket
