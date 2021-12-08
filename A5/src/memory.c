#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "trace_read.h"
#include "support.h"

#define BLOCK_SIZE 16384
#define BLOCK_MASK (BLOCK_SIZE - 1)

typedef uint64_t word;
typedef uint8_t byte;

struct block {
    word start_addr;
    byte* data;
};

struct memory {
    struct block* blocks;
    int num_blocks;
    trace_p m_tracer;
    trace_p i_tracer;
    trace_p o_tracer;
};

mem_p memory_create() {
    mem_p res = (mem_p) malloc(sizeof(struct memory));
    res->num_blocks = 0;
    res->blocks = 0;
    res->m_tracer = 0;
    return res;
}

void memory_destroy(mem_p mem) {
    for (int i = 0; i < mem->num_blocks; ++i) {
        free(mem->blocks[i].data);
    }
    free(mem->blocks);
    if (mem->m_tracer) {
        if (!trace_all_matched(mem->m_tracer))
            error("Parts of trace for memory writes was not matched");
        trace_reader_destroy(mem->m_tracer);
    }
    if (mem->i_tracer) {
        if (!trace_all_matched(mem->i_tracer))
            error("Parts of trace for input was not matched");
        trace_reader_destroy(mem->i_tracer);
    }
    if (mem->o_tracer) {
        if (!trace_all_matched(mem->o_tracer))
            error("Parts of trace for output was not matched");
        trace_reader_destroy(mem->o_tracer);
    }
    free(mem);
}

void memory_tracefile(mem_p mem, const char* filename) {
    mem->m_tracer = trace_reader_create('M', filename);
    mem->i_tracer = trace_reader_create('I', filename);
    mem->o_tracer = trace_reader_create('O', filename);
}

struct block* get_block(mem_p mem, word start_addr) {
    struct block* bp = 0;
    for (int i = 0; i < mem->num_blocks; ++i) {
        if (mem->blocks[i].start_addr == start_addr) {
            bp = &mem->blocks[i];
            break;
        }
    }
    if (bp == 0) {
        mem->blocks = realloc(mem->blocks, (mem->num_blocks + 1) * sizeof(struct block));
        mem->blocks[mem->num_blocks].data = malloc(sizeof(unsigned char) * BLOCK_SIZE);
        mem->blocks[mem->num_blocks].start_addr = start_addr;
        bp = &mem->blocks[mem->num_blocks];
        mem->num_blocks++;
    }
    return bp;
}

/* uses byte addresses */
word memory_read_byte(mem_p mem, word byte_addr) {
    word byte_number = byte_addr & BLOCK_MASK;
    word block_addr = byte_addr - byte_number;
    struct block* bp = get_block(mem, block_addr);
    return bp->data[byte_number];
}

void memory_write_byte(mem_p mem, word byte_addr, word byte) {
    word byte_number = byte_addr & BLOCK_MASK;
    word block_addr = byte_addr - byte_number;
    struct block* bp = get_block(mem, block_addr);
    bp->data[byte_number] = byte;
}

word memory_read_quad(mem_p mem, word byte_addr) {
    word res = memory_read_byte(mem, byte_addr++);
    res = res | (memory_read_byte(mem, byte_addr++) << 8);
    res = res | (memory_read_byte(mem, byte_addr++) << 16);
    res = res | (memory_read_byte(mem, byte_addr++) << 24);
    res = res | (memory_read_byte(mem, byte_addr++) << 32);
    res = res | (memory_read_byte(mem, byte_addr++) << 40);
    res = res | (memory_read_byte(mem, byte_addr++) << 48);
    res = res | (memory_read_byte(mem, byte_addr++) << 56);
    return res;
}

void memory_write_quad(mem_p mem, word byte_addr, word value) {
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
    memory_write_byte(mem, byte_addr++, value); value >>= 8;
}


void error(const char*);

void memory_read_from_file(mem_p mem, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
	error("Failed to open file");
    }
    int res;
    do {
        word addr;
        char buf[21]; // most we'll need, plus terminating 0
        res = fscanf(f, "%lx : ", &addr);
        if (res != EOF) {
            res = fscanf(f, " %[0123456789ABCDEFabcdef]", buf);
            if (res == 1) {
                //printf("%lx : %s\n", addr, buf);
                char* p = buf;
                while (*p != 0) {
                    // convert byte by byte (= 2 char at a time)
                    char buf2[3];
                    buf2[0] = *p++;
                    buf2[1] = *p++;
                    buf2[2] = 0;
                    int byte_from_hex;
                    sscanf(buf2, "%x", &byte_from_hex);
                    memory_write_byte(mem, addr, byte_from_hex);
                    int check = memory_read_byte(mem, addr);
                    if (check != byte_from_hex)
                      printf("Memory error: at %lx, wrote %x, read back %x\n", addr, byte_from_hex, check);
                    addr++;
                }
            }
            // fscanf(f,"#");
            while ('\n' != getc(f));
        }
    } while (res != EOF);
    fclose(f);
}

// read 10 bytes from memory, unaligned, uses byte addressing
void memory_read_into_buffer(mem_p mem, val address, val bytes[], bool enable) {
    word addr = address.val;
    for (int i = 0; i < 10; ++i) {
        if (enable)
            bytes[i] = from_int(memory_read_byte(mem, addr + i));
        else
            bytes[i] = from_int(0);
    }
}

bool is_io_device(val address) {
    return address.val >= 0x10000000 && address.val < 0x20000000;
}

bool is_argv_area(val address) {
    return address.val >= 0x20000000 && address.val < 0x30000000;
}

val memory_read(mem_p mem, val address, bool enable) {
    if (!enable) return from_int(0);
    if (is_io_device(address)) {
        val retval;
        if (mem->i_tracer) {
            if (trace_match_and_get_next(mem->i_tracer, address, &retval))
                return retval;
            else
                error("Trace mismatch on input from device");
        }
        if (address.val == 0x10000000) {
          int status;
          status = scanf("%lx", &retval.val);
          (void)status; // silence warning - we should perhaps one day check this?
        }
        else if (address.val == 0x10000001) retval.val = rand();
        else error("Input from unknown port");
        return retval;
    }
    else if (is_argv_area(address)) {
        val retval;
        if (mem->i_tracer) {
            if (trace_match_and_get_next(mem->i_tracer, address, &retval))
                return retval;
            else
                error("Trace mismatch on read from argv area");
        }
        // if no tracer, just access arguments from memory. Presumably set from commandline
        return from_int(memory_read_quad(mem, address.val));
    }
    else {
        return from_int(memory_read_quad(mem, address.val));
    }
}

void memory_write(mem_p mem, val address, val value, bool wr_enable) {
    if (wr_enable) {
        if (is_io_device(address)) {
            if (mem->o_tracer) {
                if (trace_match_next(mem->o_tracer, address, value))
                    return;
                else
                    error("Trace mismatch on output to device");
            }
            if (address.val == 0x10000002) printf("%lx ", value.val);
            else error("Output to unknown port");
        } else {
            // With respect to writes, we treat the argv area as a normal memory area
            if (trace_match_next(mem->m_tracer, address, value))
                memory_write_quad(mem, address.val, value.val);
            else
                error("Trace mismatch on write to memory");
        }
    }
}

void memory_load_argv(mem_p mem, int argc, char* argv[]) {
    val address;
    address.val = 0x20000000;
    val value;
    value.val = argc;
    memory_write(mem, address, value, true);
    for (int k = 0; k < argc; ++k) {
        int v;
        int res = sscanf(argv[k],"%d", &v);
        if (res != 1)
            error("Invalid command line argument");
        address.val += 8;
        value.val = v;
        memory_write(mem, address, value, true);
    }
}
