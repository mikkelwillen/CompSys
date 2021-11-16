#!/bin/python3

import argparse
import os
import hashlib
import struct
import math
import random
import socket
import time
import socketserver
import threading
from typing import Any, List, Union

HASH_LENGTH=32

# Helper method to mask network package fragmentation
def readbytes(s, count) -> bytes:
    res = bytearray()
    while count > 0:
        msg = s.recv(count)
        if not msg:
            raise Exception('Server did not respond')
        res.extend(msg)
        count -= len(msg)

    return bytes(res)


class CascadeBlock(object):
    def __init__(self, index: int, offset: int, length: int, hash: bytes) -> None:
        self.index = index
        self.offset = offset
        self.length = length
        self.hash = hash
        if len(self.hash) != HASH_LENGTH:
            raise Exception(f'Hash length should be {HASH_LENGTH} but was {len(self.hash)} (offset: {self.offset})')
        self.completed = False

class CascadeFile(object):
    def __init__(self, path: str, destination: str = None) -> None:
        self.destination = destination
        with open(path, 'rb') as f:
            self.cascadehash = hashlib.sha256(f.read()).digest()

        with open(path, 'rb') as f:
            self.header = f.read(64)
            if bytes('CASCADE1', 'ascii') != self.header[:8]:
                raise Exception(f'Signature in file header is invalid: {path}')
            self.targetsize = struct.unpack('!Q', self.header[16:24])[0]
            self.blocksize = struct.unpack('!Q', self.header[24:32])[0]
            self.targethash = self.header[32:64]
            self.trailblocksize = self.targetsize % self.blocksize
            if self.trailblocksize == 0: # In case the file size is evenly divisible with the block size
                self.trailblocksize = self.blocksize 
                
            blockcount = math.ceil(self.targetsize /  self.blocksize)

            self.blocks = []
            for n in range(blockcount):
                hash = f.read(HASH_LENGTH)
                if not hash:
                    raise Exception(f'Incorrect number of hashes in file: {path}, got {n}, but expected {blockcount}')

                self.blocks.append(
                    CascadeBlock(
                        n,
                        n * self.blocksize, 
                        self.trailblocksize if n == blockcount - 1 else self.blocksize,
                        hash
                    )
                )

            if destination is not None:
                self.prepare_download(destination)

    def prepare_download(self, destination: str) -> None:
        # Make sure the target exists
        if not os.path.isfile(destination):
            with open(destination, 'wb'):
                pass

        # Check existing blocks for completion
        with open(destination, 'rb') as f:
            for n in range(len(self.blocks)):
                size = self.blocksize if n != len(self.blocks) - 1 else self.trailblocksize
                data = f.read(size)
                if not data:
                    break
                if len(data) != size:
                    break
                if hashlib.sha256(data).digest() == self.blocks[n].hash:
                    self.blocks[n].completed = True

        # Create a queue of missing blocks
        self.queue = list([x for x in self.blocks if not x.completed])


class Peer(object):
    def __init__(self, ip: str, port: int, timestamp: int, good: bool):
        self.ip = ip 
        self.port = port
        self.timestamp = timestamp
        self.good = good

    def download_block(self, cascadehash, blockno, blocksize) -> bytes:
        req = bytearray(bytes('CASCADE1', 'ascii'))
        req.extend(struct.pack('!Q', 0))
        req.extend(struct.pack('!Q', 0))
        req.extend(struct.pack('!Q', blockno))
        req.extend(cascadehash)

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((self.ip, self.port))
            s.sendall(req)
            data = s.recv(9)
            size = struct.unpack('!Q', data[1:9])[0]

            if data[0] == 0:
                if size != blocksize:
                    raise Exception(f'The reported block size was {size} but {blocksize} was expected')
                return readbytes(s, size)
            elif data[0] == 1:
                msg = readbytes(s, size).decode('utf-8')
                raise Exception(f'Client does not have file: {msg}')
            elif data[0] == 2:
                msg = readbytes(s, size).decode('utf-8')
                print(f'Client does not have block {blockno}: {msg}')
                return None
            elif data[0] == 3:
                msg = readbytes(s, size).decode('utf-8')
                raise Exception(f'Client reported blockno was out-of-bounds: {msg}')
            elif data[0] == 4:
                msg = readbytes(s, size).decode('utf-8')
                raise Exception(f'Client reported invalid request: {msg}')
            else:
                raise Exception(f'Client gave unsupported error code: {data[0]}')


class Tracker(object):
    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port
    
    def list(self, hash: bytes, ip: Union[bytes, str], port: int) -> List[Peer]:
        return self.send_to_server(self.build_request(1, hash, ip, port))

    def subscribe(self, hash: bytes, ip: Union[bytes, str], port: int) -> List[Peer]:
        return self.send_to_server(self.build_request(2, hash, ip, port))

    def build_request(self, command: int, hash: bytes, ip: Union[bytes, str], port: int) -> bytes:
        if isinstance(ip, bytes) or isinstance(ip, bytearray):
            ipbytes = ip
        else:
            ipbytes = bytes(map(int, ip.split('.')))

        if len(ipbytes) != 4:
            raise Exception('Incorrect IP address?')

        req = bytearray(bytes('CASC', 'ascii'))
        req.extend(struct.pack('!I', 1))
        req.extend(struct.pack('!I', command))
        req.extend(struct.pack('!I', len(hash) + 4 + 2))
        req.extend(hash)
        req.extend(ipbytes)
        req.extend(struct.pack('!H', port))

        return req


    def unparse_peer(self, data: bytes) -> Peer:
        return Peer(
            '.'.join([str(x) for x in data[0:4]]), 
            struct.unpack('!H', data[4:6])[0], 
            struct.unpack('!I', data[6:10])[0], 
            True if data[10] == 1 else False
        )

    def send_to_server(self, request: bytes) -> List[Peer]:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((self.ip, self.port))
            s.sendall(request)
            data = s.recv(5)
            if not data:
                raise Exception('Server did not respond')
            if len(data) != 5:
                raise Exception('Network protocol error')

            size = struct.unpack('!I', data[1:5])[0]
            if size > 1024 * 1024:
                raise Exception(f'Too big server response: {size}')
            if data[0] != 0:
                msg = readbytes(s, size).decode('utf-8')
                raise Exception(f'Tracker gave error: {msg}')
            else:
                if size % 12 != 0:
                    raise Exception('Tracker gave a peer list with a size not a modulo of two')
                
                blob = readbytes(s, size)
                peers = size // 12
                return list([self.unparse_peer(blob[x*12:(x+1)*12]) for x in range(peers)])


class CascadePeerServe(socketserver.StreamRequestHandler):
    activefiles = {}
    activefileslock = threading.Lock()
    HEADER_LENGTH = 8+16+8+HASH_LENGTH

    def handle(self) -> None:
        while True:
            try:
                header = self.request.recv(self.HEADER_LENGTH)
            except:
                return

            if header is None:
                return
                
            if len(header) != self.HEADER_LENGTH:
                self.reporterror(4, f'Invalid header length, got {len(header)} but expected {self.HEADER_LENGTH}')
                return
            
            if header[:8] != bytes('CASCADE1', 'ascii'):
                self.reporterror(4, f'Invalid header, should start with "CASCADE1", got {header[:8].hex()}')
                return

            blockno = struct.unpack('!Q', header[24:32])[0]
            hash = header[32:64]

            file = None
            with self.activefileslock:
                if hash in self.activefiles:
                    file = self.activefiles[hash]

            if file is None:
                self.reporterror(1, f'Not serving file with hash: {hash.hex()}')
                return

            if blockno >= len(file.blocks):
                self.reporterror(3, f'File has {len(file.blocks)} blocks, but {blockno} was requested')
                return

            block = file.blocks[blockno]
            if not block.completed:
                self.reporterror(2, f'{blockno} is not currently held by this peer')
                continue

            with open(file.destination, 'rb') as f:
                if f.seek(block.offset) != block.offset:
                    self.reporterror(2, f'{blockno} is not currently held by this peer (corrupt state)')
                    return

                data = f.read(block.length)
                if len(data) != block.length:
                    self.reporterror(2, f'{blockno} is not currently held by this peer (corrupt state)')
                    return

                resp = bytearray()
                resp.extend(struct.pack('!B', 0))
                resp.extend(struct.pack('!Q', block.length))
                resp.extend(data)

                self.request.sendall(resp)

    def reporterror(self, code: int, msg: str) -> None:
        msgdata = bytes(msg, 'utf-8')
        data = bytearray(struct.pack('!B', code))
        data.extend(struct.pack('!Q', len(msgdata)))
        data.extend(msgdata)

        self.request.sendall(data)

class P2PServer(object):
    def __init__(self, tracker, ip, port) -> None:
        self.tracker = tracker
        self.ip = ip
        self.ipbytes = bytes(map(int, selfaddr.split('.')))
        self.port = port
        self.stopped = False
        self.refreshsemaphore = threading.Semaphore(0)
        self.peers = {}

        self.serverthread = threading.Thread(target=self.run_peer_server, daemon=True)
        self.refreshthread = threading.Thread(target=self.run_peer_subscribe, daemon=True)
        self.serverthread.start()
        self.refreshthread.start()

    def resubscribe(self) -> None:
        self.refreshsemaphore.release()

    def join(self) -> None:
        self.serverthread.join()

    def stop(self) -> None:
        self.stopped = True
        self.server.shutdown()
        self.refreshsemaphore.release()
        self.refreshthread.join()
        self.serverthread.join()

    def run_peer_server(self) -> None:
        print(f"Running peer server on {self.ip}:{self.port}")
        with socketserver.ThreadingTCPServer((self.ip, self.port), CascadePeerServe) as server:
            self.server = server
            server.serve_forever(poll_interval=10)

    def run_peer_subscribe(self) -> None:
        while not self.stopped:
            with CascadePeerServe.activefileslock:
                hashes = list(CascadePeerServe.activefiles)

            for h in hashes:
                try:
                    self.peers[h] = self.tracker.subscribe(h, self.ipbytes, self.port)
                except Exception as e:
                    print(f"Tracker register failed: {e}")

            self.refreshsemaphore.acquire(timeout=60*10)

def run_peer_download(tracker: Tracker, source: str, localip: str, localport: int, server:P2PServer = None, output:str = None, randomseq:bool =True) -> None:
    if not os.path.isfile(source):
        print("File not found: {args.source}")
        exit(1)

    if output is None:
        output = os.path.splitext(source)[0]

    print(f"Preparing download of {source} to {output}")
    file = CascadeFile(source, output)

    if len(file.queue) != 0:
        print(f"Download will require {len(file.queue)} blocks of size {file.blocksize}")

    with CascadePeerServe.activefileslock:
        CascadePeerServe.activefiles[file.cascadehash] = file

    if server is not None:
        server.resubscribe() # Trigger server subscription with new file

    peers = []
    last_peer_update = 0
    while len(file.queue) > 0:        

        # Ensure we have peers
        while len(peers) == 0 or time.time() - last_peer_update > 60*5:
            if time.time() - last_peer_update < 30:
                print(f'Throttling peer update; wait for 30s')
                time.sleep(30)

            try:
                peers = tracker.list(file.cascadehash, localip, localport)
            except Exception as e:
                print(f"Tracker error: {e}")
            finally:
                last_peer_update = time.time()


            if len(peers) == 0:
                print(f"We have no peers, sleeping 10s before trying again")
                time.sleep(10)
            else:
                print(f"We got {len(peers)} peer{'s' if len(peers) == 1 else ''}")

        # Pick a block
        blockid = 0 if not randomseq else random.randrange(0, len(file.queue))
        peerid = 0 if not randomseq else random.randrange(0, len(peers))
        block = file.queue[blockid]
        peer = peers[peerid]

        # Grab it
        print(f"Attempting to fetch block {block.index} from {peer.ip}:{peer.port}")
        print(f"Attempting to fetch block with hash {file.cascadehash}")
        try:
            data = peer.download_block(file.cascadehash, block.index, block.length)
            if data is None:
                raise Exception('Peer did not have the requested block')

            datahash = hashlib.sha256(data).digest()                
            if datahash != block.hash:
                print(f"Invalid hash for block {block.index} from {peer.ip}:{peer.port}. Got {datahash.hex()} but expected {block.hash.hex()}")
                raise Exception('Downloaded block was incorrect')

            with open(output, 'r+b') as f:
                f.seek(block.offset)
                f.write(data)

            block.completed = True
            file.queue.remove(block)
            print(f"Retrieved block {block.index}")

        except Exception as e:
            peers.remove(peer)
            print(f"Download failure ignoring peer. {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", help="The cascade source file(s) with information about the download")
    parser.add_argument("tracker", help="The tracker ip and port, eg: localhost:8888", default="127.0.0.1:8888")
    parser.add_argument("self", help="The address to report serving files from, e.g.: 1.2.3.4:5555", default="127.0.0.1:7777")
    parser.add_argument("-o", "--output", help="The target output file(s)", required=False)
    parser.add_argument("-r", "--random", help="Download blocks in random order from random peers", type=bool, required=False, default=True)
    parser.add_argument("-c", "--clientonly", help="Flag to set client-only mode (i.e. no serving)", type=bool, required=False, default=False)
    args = parser.parse_args()

    tracker = Tracker(args.tracker.split(':')[0], int(args.tracker.split(':')[1]))
    selfaddr, selfport = args.self.split(':')    
    selfport = int(selfport)

    server = None
    if not args.clientonly:
        server = P2PServer(tracker, selfaddr, selfport)

    source_files = args.source.split(os.pathsep)
    if args.output is None:
        target_files = []
    else:
        target_files = args.output.split(os.pathsep)

    for ix, f in zip(range(len(source_files)), source_files):
        target = None
        if ix < len(target_files):
            target = target_files[ix]
        run_peer_download(tracker, f, selfaddr, selfport, server, target, args.random)

    if args.clientonly:
        print("Download complete in client mode, stopping")
    else:
        print("Download complete serving forever ...")
        server.join()





