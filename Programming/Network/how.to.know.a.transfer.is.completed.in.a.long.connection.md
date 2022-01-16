# How to a transfer is complished in a long connection

in a long connection, the receiver doen't know the data is transered, the sender need to add the length information in a package, like this:
-------------
| length     |
-------------
| data	     |
------------- 
