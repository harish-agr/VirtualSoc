
clear

	rm -f c
	rm -f s



mysql_config --libs
mysql_config --cflags
	gcc c.c -o c -lpthread
	gcc -o s $(mysql_config --cflags) s.c $(mysql_config --libs) -lpthread
	


./s




