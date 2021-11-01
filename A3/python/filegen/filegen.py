#!/bin/python3

import argparse
import hashlib
import os
import shutil
import struct


def gen_cascade_file(sourcefile, targetfile, cascade_file, blocksize):
    with open(sourcefile, 'rb') as src:
        # Place entire file in memory :)
        data = src.read()
        fullsize = len(data)
        fullhash = hashlib.sha256(data).digest()

    with open(cascade_file, 'wb') as dst:
        # Header part
        dst.write(bytes('CASCADE1', 'ascii')) # Magic signature
        dst.write(struct.pack('!Q', 0)) # Reserved
        dst.write(struct.pack('!Q', fullsize)) # File length
        dst.write(struct.pack('!Q', blocksize)) # Block size
        dst.write(fullhash)

        with open(sourcefile, 'rb') as src:
            while True:
                data = src.read(blocksize)
                if not data:
                    break
                dst.write(hashlib.sha256(data).digest())

    if not os.path.exists(targetfile):
        shutil.copy(sourcefile, targetfile)

blocksizes = [
    (1024, '1kib'), 
    (10240, '10kib'), 
    (102400, '100kib'), 
    (1048576, '1mib'), 
    (10485760, '10mib')
]

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", help="The file to generate the cascade file for")
    args = parser.parse_args()

    sourcefile = os.path.abspath(args.source)
    if not os.path.isfile(sourcefile):
        print(f"Source file not found: {sourcefile}")
        exit(1)

    for bs in blocksizes:
        basename, ext = os.path.splitext(sourcefile)
        targetfile = f"{basename}.{bs[1]}{ext}"
        cascade_file = f"{targetfile}.cascade"
        if os.path.isfile(targetfile):
            print(f"File already exists, deleting: {targetfile}")
            os.unlink(targetfile)

        print(f"Generating file: {targetfile}")
        gen_cascade_file(sourcefile, targetfile, cascade_file, bs[0])

    print("Done!")
