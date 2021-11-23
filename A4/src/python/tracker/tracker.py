#!/bin/python3

import os
import argparse
import struct
import socketserver
import json
import time
import random
import threading
import ipaddress

HEADER_LENGTH=16
HASH_LENGTH=32
CMD_LENGTH=HASH_LENGTH + 4 + 2
IP_LENGTH=4
PORT_LENGTH=2
MAX_PEERS_REPORTED=20
MAX_PEER_AGE_SECONDS=1800

def is_good_client(ip, goods):
    return len([x for x in goods if ip == x]) > 0

class TrackerServer(socketserver.StreamRequestHandler):
    active_clients = {}
    lock = threading.Lock()
    last_cleanup = 0

    def handle(self):
        # self.request is the TCP socket connected to the client
        header = self.request.recv(HEADER_LENGTH)
        if len(header) != HEADER_LENGTH:
            self.respond_error(f'Header must be at least {HEADER_LENGTH} bytes, got {len(header)}')
            return
        
        if header[:4] != bytes('CASC', 'ascii'):
            self.respond_error(f'Header must start with CASC ({bytes("CASC", "ascii").hex()}), got: {header[:4].hex()}')
            return

        protocol_version = struct.unpack('!I', header[4:8])[0]
        command = struct.unpack('!I', header[8:12])[0]
        datalen = struct.unpack('!I', header[12:16])[0]
        if protocol_version != 1:
            self.respond_error(f'Protocol version must be 1, got: {protocol_version}')
            return

        if command == 1 or command == 2:
            if datalen != CMD_LENGTH:
                self.respond_error(f'Data for command=1 must be of length {CMD_LENGTH}, got {datalen}')
                return
                
            data = self.request.recv(CMD_LENGTH)
            if len(data) != CMD_LENGTH:
                self.respond_error(f'Data from socket had length {len(data)}, expected {CMD_LENGTH}')
                return

            hash = data[:32]
            hashhex = hash.hex()
            ipportkey = data[32:].hex()

            with self.lock:
                hashfound = hashhex in self.active_clients

            if not hashfound:
                self.respond_error(f"The supplied hash '{hashhex}' is not tracked by this tracker")
                return

            if command == 2:
                # For local testing you may need to remove this check
#                if data[32:36] == b'\0\0\0\0':
#                    self.respond_error(f'The IP {({".".join([str(x) for x in data[32:36]])})} is not supported')
#                    return

                if self.banprivateips and ipaddress.IPv4Address(data[32:36]).is_private:
                    self.respond_error(f'The IP reported to the tracker ({".".join([str(x) for x in data[32:36]])}) appears to be a private IP')
                    return

                # Avoid peers registering as good peers without approval
                if is_good_client(data[32:36], self.goodips):
                    if str(ipaddress.IPv4Address(data[32:36])) == self.client_address[0]:
                        print("Registering a good client")
                    else:
                        self.respond_error(f'The IP reported to the tracker ({".".join([str(x) for x in data[32:36]])}) is a reserved IP')
                        return
                else:
                    print("Not a good client")

                with self.lock:
                    self.active_clients[hashhex][ipportkey] = int(time.time())

            self.report_clients(hash.hex(), ipportkey)

        else:
            self.respond_error(f'Only commands (list=1, subscribe=2) supported, got: {command}')
            return

    def report_clients(self, hashhex, selfipport=None):
        now = int(time.time())

        with self.lock:
            # Perform cleanup, if required
            if self.last_cleanup == 0:
                self.last_cleanup = now
            elif now - self.last_cleanup > (MAX_PEER_AGE_SECONDS * 2):
                self.last_cleanup = now
                for h in self.active_clients:
                    for x in list([n for n in h if now - n > MAX_PEER_AGE_SECONDS]):
                        del h[x]

            # Build list of peers
            d = self.active_clients[hashhex]
            peers = list([(
                bytes.fromhex(x)[:IP_LENGTH], # Ip is tupple item 0
                bytes.fromhex(x)[IP_LENGTH:IP_LENGTH+PORT_LENGTH], # Port is tupple item 1
                d[x], # Timestamp is tupple item 2
                is_good_client(bytes.fromhex(x)[:IP_LENGTH], self.goodips) # Good flag is tupple item 3
                ) for x in d if now - d[x] < MAX_PEER_AGE_SECONDS and x != selfipport])

        # Random order
        random.shuffle(peers)
        peers.sort(key=lambda x: x[3] != 1) # Keep good ones at top

        # Build response
        result = bytearray()
        for n,ix in zip(peers, range(MAX_PEERS_REPORTED)):
            result.extend(n[0]) # ip
            result.extend(n[1]) # port
            result.extend(struct.pack('!l', n[2])) #timestamp
            result.extend(struct.pack('!B', 1 if n[3] else 0)) # Good flag
            result.extend(struct.pack('!B', 0)) # reserved

        header = bytearray()
        header.extend(struct.pack('!B', 0))
        header.extend(struct.pack('!I', len(result)))
        header.extend(result)

        self.request.sendall(header)

    def respond_error(self, msg):
        msgdata = bytes(msg, 'utf-8')
        data = bytearray(struct.pack('!B', 1))
        data.extend(struct.pack('!I', len(msgdata)))
        data.extend(msgdata)

        self.request.sendall(data)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("config", help="The configuration file")
    args = parser.parse_args()

    if not os.path.isfile(args.config):
        print(f"Config file not found: {args.config}")

    with open(args.config) as jf:
        config = json.load(jf)
    if not "port" in config:
        print("Config is missing port")
        exit(1)
    if not "allowedhashes" in config:
        print("Config is missing allowedhashes")
        exit(1)

    if len(config["allowedhashes"]) <= 0:
        print("Need at least one allowed hash")
        exit(1)

    if not "good-ips" in config:
        config["good-ips"] = []

    HOST, PORT = "0.0.0.0", int(config['port'])
    hashes = list(bytes.fromhex(x) for x in config['allowedhashes'])
    invalids = [x for x in hashes if len(x) != 32]
    if len(invalids) > 0:
        print(f"Invalid hashes: {', '.join([x.hex() for x in invalids])}")
        exit(1)

    goodips = list([bytes(map(int, ip.split('.'))) for ip in config["good-ips"]])
    invalids = [x for x in goodips if len(x) != 4]
    if len(invalids) > 0:
        print(f"Invalid IPs: {', '.join(['.'.join([str(x) for x in ip]) for ip in invalids])}")
        exit(1)

    TrackerServer.active_clients = dict(zip([x.hex() for x in hashes], [{} for x in range(len(hashes))]))
    TrackerServer.goodips = goodips
    TrackerServer.banprivateips = False
    if 'ban-private' in config:
        TrackerServer.banprivateips = config['ban-private']

    with socketserver.ThreadingTCPServer((HOST, PORT), TrackerServer) as server:
        server.serve_forever()

