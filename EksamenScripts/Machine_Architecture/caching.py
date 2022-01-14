import math
import sys
print(sys.argv)

def calc(argv):
    # count elements in list, if not 5, return how to call the function
    if (len(argv) != 5):
        return print("usage: ./caching.py bitAddress waySet dataCache blockSize")
    
    bitAddr = sys.argv[1]   # bit
    waySet = sys.argv[2]    # decimal
    dataCache = sys.argv[3] # byte
    blockSize = sys.argv[4] # byte

    # b bits (block offset)
    OBits = math.log(float(blockSize), 2)
    print("block offset: ", + OBits)

    # s bits (set index)
    IBits = math.log(float(dataCache) / (float(waySet) * float(blockSize)), 2)
    print("set index: ", + IBits)

    # t bits (tag)
    TBits = float(bitAddr) - float(OBits) - float(IBits)
    print("cache tag: ", + TBits)

    # total bits
    print("\n\ntotal bits: ", + OBits + IBits + TBits)

#    a = []
#    for _ in range(TBits):
#        a.append("T")
#    for _ in range(IBits):
#        a.append("I")
#    for _ in range(OBits):
#        a.append("O")
#    print(a)

calc(sys.argv)

#math.log(num, base)