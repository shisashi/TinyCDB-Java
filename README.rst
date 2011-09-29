############
TinyCDB-Java
############

What's this?
============
TinyCDB_ 's java binding.

.. _TinyCDB: http://www.corpit.ru/mjt/tinycdb.html

Requirements
============
* TinyCDB 0.77 or later
* Java SE 6 or later

Restrictions
============
Now this library only implements Query mode.

How to build
============

1. Edit Makefile to adjust some macros to build environment.
2. build by following.

::

  % make
   or
  % make -f Makefile.cygwin

build for android
-----------------

1. setup Android NDK
2. Copy source codes of tinycdb into android/jni/
3. copy src/main/c/jcdb.c android/jni/
4. generate net_shisashi_tinycdb_TinyCDBJNI by following.

::

  javah -o android/jni/net_shisashi_tinycdb_TinyCDBJNI \
   -classpath bin -jni net.shisashi.tinycdb.TinyCDBJNI

5. cd android
6. <NDK>/ndk-build
7. Build tool generate a shared library to under android/libs directory.
8. Copy android/libs to <your project>/libs.
9. Enjoy.

Creator
=======
S.Hisashi

* http://www.shisashi.net/
* http://twitter.com/shisashi
* shisashi[at]gmail.com

License
=======
Apache License, Version 2.0
