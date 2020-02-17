import hashlib
from sys import argv, path

md5 = hashlib.md5(open(argv[1],'rb').read()).hexdigest()

print("MD5: %s" % md5)

f = open(argv[2], "w")
f.write(md5)
f.close()