mkdir -p bin

gcc -Wall -o bin/client-tcp client/client-tcp.c
gcc -Wall -o bin/server-tcp server/server-tcp.c server/db.c server/cli.c

gcc -Wall -o bin/client-udp client/client-udp.c
gcc -Wall -o bin/server-udp server/server-udp.c server/db.c server/cli.c
