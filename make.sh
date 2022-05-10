gcc -c -Wall -fpic jetgpio.c 
gcc -shared -o libjetgpio.so jetgpio.o
sudo cp ./libjetgpio.so /usr/lib/
sudo chmod 0755 /usr/lib/libjetgpio.so
sudo ldconfig
sudo ldconfig -p | grep libjetgpio.so
