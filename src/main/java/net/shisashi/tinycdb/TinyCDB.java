package net.shisashi.tinycdb;

public final class TinyCDB {
    private static final long NULL = 0;
    /** Index of the native table for database handles. */
    private long pointer = NULL;

    /**
     * Get the database handle as a reader.
     * 
     * @param filename
     *            filename to read
     * @throws CDBException
     *             if an error occurs.
     */
    public TinyCDB(String filename) throws CDBException {
        this.pointer = TinyCDBJNI.openReader(filename);
    }

    /**
     * Retrieve a record.
     * 
     * @param key
     *            a byte array of a key.
     * @return a byte array of the value of the corresponding record. If no
     *         record corresponds, `null' is returned instead of exception.
     * @throws CDBException
     *             if an error occurs.
     */
    public final byte[] find(byte[] key) throws CDBException {
        if (pointer == NULL) {
            throw new CDBException("CDB already closed.");
        }
        return TinyCDBJNI.find(pointer, key, key.length);
    }

    public final byte[][] findAll(byte[] key) throws CDBException {
        if (pointer == NULL) {
            throw new CDBException("CDB already closed.");
        }
        return TinyCDBJNI.findAll(pointer, key, key.length);
    }

    public final void close() {
        TinyCDBJNI.close(pointer);
        this.pointer = NULL;
    }
}
