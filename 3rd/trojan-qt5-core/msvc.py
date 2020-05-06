import os
import shutil

f = open("trojan-qt5-core.h", "r")
f2 = open("trojan-qt5-core-new.h", "w")

for line in f.readlines():
    if "#line 1" in line or "_Complex" in line or "__SIZE_TYPE__" in line:
        pass
    else:
        f2.write(line)

f.close()
f2.close()

shutil.copyfileobj(open("trojan-qt5-core-new.h", "r"), open("trojan-qt5-core.h", "w"))
os.remove("trojan-qt5-core-new.h")