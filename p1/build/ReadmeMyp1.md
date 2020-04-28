To understand the code changes please look at changes_diff.txt (git diff from p0). Note that this diff does not include any assertions.

files added:
    ed-script1.txt: 

Files modified:
    wbadmin.c
    server.x
    client.x
    server.c
    client.c
    Makefile
    ed-script.txt

Some key modifications:
    1.) Now each server on startup gets a new rpc program number from transient.c. 
        This is done as now on same machine 2 servers can exist.
    2.) Every client now have to supply rpc program number to connect to a server along with 
        server's machine name.
    3.) make now builds administritive client wbadmin7370.    

build:
    make clean ; make

usage:
    To start a server:
        ./server7370

    To start a client on a board:
        
        ./client7370 <server-hostname> <server-program-num-in-hex> <wb-name> <X11-display-hostname> <color-integer>
        
        eg: ./client7370 localhost 40000005 b3 localhost ff0000 

    To start a new server via wbadmin:
    
        ./wbadmin7370 -n <existing-server-machine-nm> <prognum-in-hex> <new-server-machine-nm>
    
        eg: ./wbadmin7370 -n 127.0.0.1 40000000 127.0.0.1

    To transfer a board from one server to another using wbadmin:
      
        ./wbadmin7370 -t <from-server-machine-nm> <prognum-in-hex> <wb-nm> <to-server-machine-nm> <prognum>

        eg: ./wbadmin7370 -t 127.0.0.1 40000000 b4 127.0.0.1 40000005

    To queary a server via wbadmin:
    
        ./wbadmin7370 -q <server-machine-nm> <prognum-in-hex>

        eg: ./wbadmin7370 -q 127.0.0.1 40000005