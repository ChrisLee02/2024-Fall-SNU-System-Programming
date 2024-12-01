#!/bin/bash

# 1. Create text files for each client with respective commands


# 2. Start clients sequentially
../ref/client < client1.txt &
sleep 1  # Wait 1 second before starting the next client
../ref/client < client2.txt &
sleep 1
../ref/client < client3.txt &
sleep 1
../ref/client < client4.txt &
sleep 1
../ref/client < client5.txt &
