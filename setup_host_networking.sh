#!/bin/bash -e

# create virtual network interface

sudo ip tuntap add dev tap0 mode tap user samuel
sudo ip a a dev tap0 10.44.45.1/24
sudo ifconfig tap0 up

# connect it to the internet

sudo sysctl net.ipv4.ip_forward=1
sudo sysctl net.ipv6.conf.default.forwarding=1
sudo sysctl net.ipv6.conf.all.forwarding=1

export INTERNET_IF=wlp4s0 # <!--- Make sure to replace wlan0 with the network interface connected to Internet

sudo iptables -t nat -A POSTROUTING -o $INTERNET_IF -j MASQUERADE
sudo iptables -A FORWARD -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i tap0 -o $INTERNET_IF -j ACCEPT
