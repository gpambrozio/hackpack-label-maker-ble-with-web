#!/bin/bash

# Deployment configuration - edit these variables for your server
SERVER_USER="gustavo"
SERVER_HOST="nas.local"
DEPLOY_PATH="/zfspool/data/caddy/site"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Deploying Label Maker to server...${NC}"

# Check if required files exist
if [ ! -f "web-client.html" ]; then
    echo -e "${RED}Error: web-client.html not found${NC}"
    exit 1
fi

echo -e "${YELLOW}Deploying web-client.html as label.html...${NC}"
scp web-client.html "${SERVER_USER}@${SERVER_HOST}:${DEPLOY_PATH}/label.html"

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Failed to deploy web client${NC}"
    exit 1
fi

# Check if TTF files exist
TTF_FILES=(*.ttf)
if [ -e "${TTF_FILES[0]}" ]; then
    echo -e "${YELLOW}Deploying TTF font files...${NC}"
    scp *.ttf "${SERVER_USER}@${SERVER_HOST}:${DEPLOY_PATH}/"
fi

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Failed to deploy font files${NC}"
    exit 1
fi

echo -e "${GREEN}Deployment completed successfully!${NC}"
echo -e "${GREEN}Label maker is now available at: http://${SERVER_HOST}/label.html${NC}"
