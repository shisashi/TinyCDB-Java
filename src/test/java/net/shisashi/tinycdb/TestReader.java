package net.shisashi.tinycdb;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.fail;

import org.junit.Test;

public class TestReader {

    @Test
    public void testStaticInitializer() throws Exception {
        try {

            String libpath = System.getProperty("java.library.path"); 
            System.out.println("java.library.path is " + libpath); 

            Class<?> clz = Class.forName("net.shisashi.tinycdb.TinyCDBJNI");
            assertEquals(TinyCDBJNI.class, clz);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Test
    public void testOpenAndRead() throws Exception {
        TinyCDB cdb = new TinyCDB("src/test/resources/net/shisashi/tinycdb/1.cdb");

        String[] keys = { "a", "b" };
        String[] values = { "b", "abc" };

        for (int i = 0; i < keys.length; i++) {
            String key = keys[i];
            String expectedValue = values[i];
            byte[] expectedsBytes = expectedValue.getBytes();

            assertArrayEquals(expectedsBytes, cdb.find(key.getBytes()));
        }

        assertNull(cdb.find("not_exist_key".getBytes()));
        assertNull(cdb.find("no_key".getBytes()));

        cdb.close();
    }

    @Test
    public void testFileDoesntExist() throws Exception {
        try {
            new TinyCDB("never_existing_filename");
            fail("Doesn't occur exception.");
        }
        catch (CDBException e) {
            // success
            assertEquals("cannot open file.", e.getMessage());
        }
    }

    @Test
    public void testOpenAndReadAll() throws Exception {
        TinyCDB cdb1 = new TinyCDB("src/test/resources/net/shisashi/tinycdb/1.cdb");
        TinyCDB cdb2 = new TinyCDB("src/test/resources/net/shisashi/tinycdb/600.cdb");

        // cdb1
        {
            assertEquals(0, cdb1.findAll("not_exist_key".getBytes()).length);
            assertEquals(0, cdb1.findAll("no_key".getBytes()).length);

            byte[][] values = cdb1.findAll("one".getBytes());

            assertEquals(2, values.length);

            assertArrayEquals("here".getBytes(), values[0]);
            assertArrayEquals("also".getBytes(), values[1]);
        }
        {
            // cdb2
            assertArrayEquals("other".getBytes(), cdb2.find("b".getBytes()));

            byte[][] values = cdb2.findAll("a".getBytes());
            assertEquals(600, values.length);
            
            for (int i=0; i<600; i++) {
                assertArrayEquals(String.format("%03d", i).getBytes(), values[i]);
            }
        }

        cdb2.close();
        cdb1.close();
    }
}
