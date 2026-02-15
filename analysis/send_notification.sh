#!/bin/bash

# Konfiguracja
TOKEN=""
CHAT_ID=""
MESSAGE="$1"

# Wysyłanie wiadomości
curl -s -X POST "https://api.telegram.org/bot$TOKEN/sendMessage" \
    -d chat_id="$CHAT_ID" \
    -d text="$MESSAGE"  | grep ddddddd
