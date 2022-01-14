#!/usr/bin/env python3.3
# -*- coding: utf-8 -*-
#
#  IP.py

def result(arg):
    out=''
    cpy=arg
    for i in range(3):
        out='.'+str(cpy&0xFF)+out
        cpy>>=8
    return str(cpy&0xFF)+out

binIP=0
IP=input("Give me IPv4: ")
temp=0
dots=0
for i in range(len(IP)):
    try:
        temp=temp*10+int(IP[i])
    except ValueError:
        binIP<<=8
        binIP|=temp
        temp=0
        if dots==3:
            bin_mask=0xFFFFFFFF>>int(IP[i+1:])
            break
        dots+=1
print()
print("Network  :", result(binIP&~bin_mask))
print("Broadcast:", result(binIP|bin_mask))