package net.shisashi.tinycdb;

class TinyCDBJNI {
    static {
        System.loadLibrary("jcdb");
    }

    protected static final native long openReader(String filename) throws CDBException;

    protected static final native byte[] find(long pointer, byte[] key, int keySize) throws CDBException;

    protected static final native byte[][] findAll(long pointer, byte[] key, int keySize) throws CDBException;

    protected static final native void close(long pointer);
}
