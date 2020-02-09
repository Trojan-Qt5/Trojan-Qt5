import os

for each in os.listdir():
    try:
        if each.split(".")[1] == "c":
            print(f"    src/privoxy/pcre/{each}" + " \\")
    except:
        continue