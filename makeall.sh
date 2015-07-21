#!/bin/bash

echo "Compiling..."
cc server.c -pthread -w -o server
cc new_client.c -pthread -w -o new_client
cc client.c -w -o client
echo "Done!"
