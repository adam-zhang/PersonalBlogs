reverse remove in python
```
#!/usr/bin/python3
import os
import sys

l = []
for i in range(1, len(sys.argv) ):
    l.append(sys.argv[i])

def inList(file, l):
    for f in l:
        if file == f:
            return True
    return False

for file in os.listdir("."):
    if not inList(file, l):
        os.remove(file)
```
