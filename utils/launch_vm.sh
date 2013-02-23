#!/bin/bash

echo 'launching VM'

kvm -curses -kernel linux-2.6.22.19-cs543/arch/x86_64/boot/bzImage -append 'root=/dev/hda1 ro' -drive file=local.qcow2
