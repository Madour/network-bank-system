mkdir -p bin

gcc -Wall -I. -o bin/client-tcp client/client-tcp.c common/utils.c
gcc -Wall -I. -o bin/server-tcp server/server-tcp.c server/db.c server/cli.c common/utils.c

gcc -Wall -I. -o bin/client-udp client/client-udp.c common/utils.c
gcc -Wall -I. -o bin/server-udp server/server-udp.c server/db.c server/cli.c common/utils.c
