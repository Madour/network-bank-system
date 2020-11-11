mkdir -p bin
gcc -Wall -o bin/client-tcp client/client-tcp.c
gcc -Wall -o bin/server-tcp server/server-tcp.c server/db.c server/cli.c
