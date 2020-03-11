import hashlib
from sys import argv, path

md5 = hashlib.md5(open(argv[1],'rb').read()).hexdigest()
sha1 = hashlib.sha1(open(argv[1],'rb').read()).hexdigest()
sha256 = hashlib.sha256(open(argv[1],'rb').read()).hexdigest()
sha384 = hashlib.sha384(open(argv[1],'rb').read()).hexdigest()
sha512 = hashlib.sha512(open(argv[1],'rb').read()).hexdigest()

print("MD5: %s" % md5)
print("SHA1: %s" % sha1)
print("SHA256: %s" % sha256)
print("SHA384: %s" % sha384)
print("SHA512: %s" % sha512)

f = open(argv[2], "w")
f.write("MD5: %s\n" % md5)
f.write("SHA1: %s\n" % sha1)
f.write("SHA256: %s\n" % sha256)
f.write("SHA384: %s\n" % sha384)
f.write("SHA512: %s" % sha512)
f.close()