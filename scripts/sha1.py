import hashlib
from sys import argv, path

sha1 = hashlib.sha1(open(argv[1],'rb').read()).hexdigest()

print("SHA1: %s" % sha1)

f = open(argv[2], "w")
f.write(sha1)
f.close()