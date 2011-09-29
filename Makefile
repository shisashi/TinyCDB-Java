# customize here
JDK_HOME   = /opt/jdk1.7.0
LIBCDBPICA = /usr/lib/libcdb_pic.a

CC         = gcc
CFLAGS     = -O3 -fPIC -Wall -Isrc/main/java -I. -I$(JDK_HOME)/include -I$(JDK_HOME)/include/linux
JAVAH      = $(JDK_HOME)/bin/javah
OBJS       = jcdb.o

libjcdb.so: $(OBJS)
	gcc -shared -o $@ -s $(OBJS) $(LIBCDBPICA)

net_shisashi_tinycdb_TinyCDBJNI.h: bin/net/shisashi/tinycdb/TinyCDBJNI.class
	$(JAVAH) -o $@ -classpath bin -jni net.shisashi.tinycdb.TinyCDBJNI

jcdb.o: src/main/c/jcdb.c net_shisashi_tinycdb_TinyCDBJNI.h
	$(CC) $(CFLAGS) $(CC_INCLUDE) -c src/main/c/jcdb.c -o $@

clean:
	rm -f $(OBJS) libjcdb.so net.shisashi.tinycdb.TinyCDBJNI
