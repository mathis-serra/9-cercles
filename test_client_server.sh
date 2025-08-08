#!/bin/bash

echo "Testing LPTF Client-Server Communication"
echo "========================================="

# Test client connection
echo "Starting client and sending test message..."

# Use expect to automate client interaction
expect << 'EOF'
spawn ./main client
expect ">"
send "Hello from automated test!\r"
expect ">"
send "This is a protocol test\r"
expect ">"
send "quit\r"
expect eof
EOF

echo "Client test completed!"
