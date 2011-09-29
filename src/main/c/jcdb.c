#include "net_shisashi_tinycdb_TinyCDBJNI.h"
#include <cdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static void throwCDBException(JNIEnv* env, const char* message);

JNIEXPORT jlong JNICALL
    Java_net_shisashi_tinycdb_TinyCDBJNI_openReader(JNIEnv* env, jclass myclass, jstring name)
{
    struct cdb *cdb;
    int fd;

    if ((cdb = malloc(sizeof(struct cdb))) == NULL) {
        throwCDBException(env, "cannot allocate memory.");
        return -1;
    }

    {
        const char *tname;
        jboolean ic;

        tname = (*env)->GetStringUTFChars(env, name, &ic);
        fd = open(tname, O_RDONLY);
        if (ic == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, name, tname);

        if (fd == -1) {
            throwCDBException(env, "cannot open file.");
            return -1;
        }
    }

    if (cdb_init(cdb, fd) < 0) {
        throwCDBException(env, "cdb_init error");
        return -1;
    }

    return (jlong) cdb;
}

JNIEXPORT jbyteArray JNICALL
    Java_net_shisashi_tinycdb_TinyCDBJNI_find(JNIEnv* env, jclass myclass, jlong pointer, jbyteArray key, jint key_size)
{
    int find_result;
    struct cdb* cdb = (struct cdb*) pointer;

    {
        jboolean ic;
        jbyte *kbuf;

        kbuf = (*env)->GetByteArrayElements(env, key, &ic);
        find_result = cdb_find(cdb, (char*) kbuf, key_size);
        if (ic == JNI_TRUE) (*env)->ReleaseByteArrayElements(env, key, kbuf, JNI_ABORT);
    }

    if (find_result > 0) {
        const void *val;
        jbyteArray vbuf;
        unsigned int vlen, vpos;

        vpos = cdb_datapos(cdb);
        vlen = cdb_datalen(cdb);
        if ((val = cdb_get(cdb, vlen, vpos)) == NULL) {
            throwCDBException(env, "cdb_get error");
            return NULL;
        }

        vbuf = (*env)->NewByteArray(env, vlen);
        (*env)->SetByteArrayRegion(env, vbuf, 0, vlen, (jbyte*) val);
        return vbuf;
    }
    else {
        if (find_result < 0)
            throwCDBException(env, "cdb_find error");
        return NULL;
    }
}

JNIEXPORT jobjectArray JNICALL
    Java_net_shisashi_tinycdb_TinyCDBJNI_findAll(JNIEnv* env, jclass myclass, jlong pointer, jbyteArray key, jint key_size)
{
    struct cdb_find cdbf;
    int findinit_result;
    jboolean ic;
    jbyte *kbuf;
    struct cdb* cdbp;

    cdbp = (struct cdb*) pointer;

    kbuf = (*env)->GetByteArrayElements(env, key, &ic);
    findinit_result = cdb_findinit(&cdbf, cdbp, (const void*) kbuf, key_size);

    if (findinit_result < 0) {
        throwCDBException(env, "cdb_findinit error");
        if (ic == JNI_TRUE) (*env)->ReleaseByteArrayElements(env, key, kbuf, JNI_ABORT);
        return NULL;
    }

    {
        jbyteArray* array_array;
        size_t array_size = 1;
        size_t n = 0;

        if ((array_array = (jbyteArray*) malloc(sizeof(jbyteArray*) * array_size)) == NULL) {
            throwCDBException(env, "cannot allocate memory.");
            if (ic == JNI_TRUE) (*env)->ReleaseByteArrayElements(env, key, kbuf, JNI_ABORT);
            return NULL;
        }

        for (;;) {
            int findnext_result;

            findnext_result = cdb_findnext(&cdbf);
            if (findnext_result > 0) {
                const void *val;
                jbyteArray vbuf;
                unsigned int vlen, vpos;

                vpos = cdb_datapos(cdbp);
                vlen = cdb_datalen(cdbp);

                if ((val = cdb_get(cdbp, vlen, vpos)) == NULL) {
                    throwCDBException(env, "cdb_get error");
                    break;
                }

                vbuf = (*env)->NewByteArray(env, vlen);
                (*env)->SetByteArrayRegion(env, vbuf, 0, vlen, (jbyte*) val);
                array_array[n] = vbuf;
                n++;

                if (n >= array_size) {
                    jbyteArray* new_array_array;
                    int new_array_size = array_size * 2;

                    if ((new_array_array = (jbyteArray*) realloc(array_array, sizeof(jbyteArray*) * new_array_size)) == NULL) {
                        free(array_array);
                        throwCDBException(env, "cannot allocate memory.");
                        break;
                    }
                    //printf("%d, %d->%d\n", n, array_size, new_array_size);
                    array_array = new_array_array;
                    array_size = new_array_size;
                }
            }
            else if (findnext_result == 0) {
                jobjectArray result_array;
                jclass byte_array_class = (*env)->FindClass(env, "[B");
                int i;

                if ((result_array = (*env)->NewObjectArray(env, n, byte_array_class, NULL)) == NULL) {
                    throwCDBException(env, "cannot construct result byte array.");
                    break;
                }

                for (i = 0; i < n; i++) {
                    (*env)->SetObjectArrayElement(env, result_array, i, array_array[i]);
                }
                if (ic == JNI_TRUE) (*env)->ReleaseByteArrayElements(env, key, kbuf, JNI_ABORT);
                return result_array;
            }
            else {
                throwCDBException(env, "cdb_findnext error");
                break;
            }
        } // after for (;;)
        if (ic == JNI_TRUE) (*env)->ReleaseByteArrayElements(env, key, kbuf, JNI_ABORT);
        return NULL;
    }
}

JNIEXPORT void JNICALL
    Java_net_shisashi_tinycdb_TinyCDBJNI_close(JNIEnv* env, jclass myclass, jlong pointer)
{
    struct cdb *cdb;
    cdb = (struct cdb*) pointer;

    close(cdb_fileno(cdb));

    cdb_free(cdb);
    free(cdb);
}

static void throwCDBException(JNIEnv* env, const char* message) {
    (*env)->ThrowNew(env, (*env)->FindClass(env, "net/shisashi/tinycdb/CDBException"), message);
}
