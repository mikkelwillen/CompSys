import math
import sys
print(sys.argv, "\n")

def calc(argv):
    # count elements in list, if not 5, return how to call the function
    if (len(argv) != 5):
        return print("usage: ./caching.py virtualAddrLen physicalAddrLen pageSize hexa")
    
    virtualAddrLen = sys.argv[1]
    physicalAddrLen = sys.argv[2]
    pageSize = sys.argv[3]
    hexa = sys.argv[4]

    # hex til bineær med længde virtualAddrLen
    input = bin(int(hexa, 16))[2:].zfill(int(virtualAddrLen))
    # bineær til array
    virtualAddr = [int(x) for x in str(input)]
    
    # vpo længde i decimal
    vpolen = math.log(int(pageSize), 2)
    # vpn og vpo til array
    vpn = virtualAddr[:int(virtualAddrLen)-int(vpolen)]
    vpo = virtualAddr[len(vpn):]

    # vpo og vpn string
    vpnNum = convert(vpn)
    #vpoNum = convert(vpn)

    # tlbi længde i sets
    tlbiLen = round(math.log(len(vpn)/2, 2))
    # tlbi i hex
    tlbi = convert(vpn[len(vpn)-int(tlbiLen):])
    # tlbt i hex
    tlbt = convert(vpn[:len(vpn)-int(tlbiLen)])

    print("virtualAddr:", virtualAddr)
    #print("vpn: ", vpn)
    #print("vpo: ", vpo)
    #print("vpo len: ", vpolen)
    print("vpn: ", vpnNum)
    #print("vpoNum: ", vpoNum)
    #print("tlbi len: ", tlbiLen)
    print("tlb index: ", tlbi)
    print("tlb tag: ", tlbt)

    ppn = ppnCalc(vpo, physicalAddrLen)
    print("physicalAddr:", ppn)

def convert(list): 
    # int liste til string liste 
    s = [str(i) for i in list] 
    # læg sammen vha. join() 
    res = str("".join(s))
    # bineær string til hex int
    num = hex(int(res, 2))

    return num

def ppnCalc(vpo_, PhysicalAddrLen_):
    ppn = input("enter PPN: ")
    # ppn til string
    ppnBin = bin(int(ppn, 16))[2:]
    # vpo til string
    vpoBin = ''.join([str(i) for i in vpo_])

    # sætter strings sammen og 0 for enden. Så string til array
    physicalAddr = [int(x) for x in str(ppnBin + vpoBin).zfill(int(PhysicalAddrLen_))]

    return physicalAddr

calc(sys.argv)