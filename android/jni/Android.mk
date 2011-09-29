LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := jcdb
LOCAL_SRC_FILES := cdb.c cdb_find.c cdb_findnext.c cdb_hash.c \
	cdb_init.c cdb_make.c cdb_make_add.c cdb_make_put.c \
	cdb_seek.c cdb_seq.c cdb_unpack.c jcdb.c

LOCAL_CFLAGS := -I$(LOCAL_PATH)

include $(BUILD_SHARED_LIBRARY)