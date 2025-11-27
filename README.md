# http_server
system call と socket programming で HTTP server を作ってみる

gcc server.c util.c -o server && ./server PORT
gcc client.c util.c -o client && ./client 127.0.0.1 PORT
> 4+5