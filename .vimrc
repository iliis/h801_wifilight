nmap <F5> :wa<CR>:exec "make flash && echo '--'"<CR>
nmap <F6> :wa<CR>:exec "make flash && xfce4-terminal -e 'minicom --noinit --statline --baudrate 115200 --device /dev/ttyUSB5'"<CR>
