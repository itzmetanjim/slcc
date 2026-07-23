import sys
from itertools import chain
from bisect import bisect_left as bl
def ST(line):
    le=len(line.lstrip(" \t"))
    st= line[:-le] if le!=0 else line
    return st.count(" ")+4*st.count("\t")
IS=chain(iter(sys.stdin),("",))
TS=[0]
try:
    Line=next(IS).rstrip("\n")
    while True:
        try:
            LINE=next(IS).rstrip("\n")
        except StopIteration:
            LINE=None
        if Line.startswith("#"):
            #im not gonna even try
            print(Line)
            if LINE is None:
                break
            Line=LINE
            continue
        if len(TS)!=1:
            idx=bl(TS,ST(Line))
            if idx==len(TS) or idx==len(TS)-1:
                pass
            else:
                lTS=len(TS)
                TS=TS[:idx+1]
                Line="}"*(lTS-idx-1)+Line
        if Line.endswith(":"):
            lw=Line[:-1].rstrip(" ")
            if lw.endswith(")") or lw.endswith("else"):
                if LINE is None:
                    print("Error: Colon on last line.")
                    exit(1)
                Line=Line[:-1]+"{"
                TS+=[ST(LINE)]
        print(Line)
        if LINE is None:
            break
        Line=LINE
except StopIteration:
    pass

