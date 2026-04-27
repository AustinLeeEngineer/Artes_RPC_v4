socat pty,link=/tmp/virtualserial,raw tcp:192.168.0.250:10001 &
sudo minicom -D /tmp/virtualserial
sudo kill $(pidof socat)
