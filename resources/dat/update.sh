#!/bin/bash

# Update geoip.dat
GEOIP_TAG=$(curl --silent "https://api.github.com/repos/v2ray/geoip/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')
curl -L -o geoip.dat "https://github.com/v2ray/geoip/releases/download/${GEOIP_TAG}/geoip.dat"
sleep 1

# Update geosite.dat
GEOSITE_TAG=$(curl --silent "https://api.github.com/repos/v2ray/domain-list-community/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')
curl -L -o geosite.dat "https://github.com/v2ray/domain-list-community/releases/download/${GEOSITE_TAG}/dlc.dat"
sleep 1