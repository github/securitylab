all: direct_attack crash_all

direct_attack: direct_attack.o send_packet.o utils.o
	gcc -O2 -Wall direct_attack.o send_packet.o utils.o -o direct_attack

crash_all: crash_all.o utils.o
	gcc -O2 -Wall crash_all.o send_packet.o utils.o -o crash_all

direct_attack.o: direct_attack.c send_packet.h utils.h
	gcc -O2 -Wall -c direct_attack.c

crash_all.o: crash_all.c send_packet.h utils.h
	gcc -O2 -Wall -c crash_all.c

send_packet.o: send_packet.c send_packet.h utils.h
	gcc -O2 -Wall -c send_packet.c

utils.o: utils.c utils.h
	gcc -O2 -Wall -c utils.c

clean:
	rm -f *~ *.o direct_attack crash_all
