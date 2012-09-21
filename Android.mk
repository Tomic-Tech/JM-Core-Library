LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := JMCore
### Add all source file names to be included in lib separated y a whitespace
LOCAL_SRC_FILES := eglib/garray.c \
				   eglib/gdate-unix.c \
				   eglib/gdir-unix.c \
				   eglib/gerror.c \
				   eglib/gfile-posix.c \
				   eglib/gfile-unix.c \
				   eglib/gfile.c \
				   eglib/ghashtable.c \
				   eglib/giconv.c \
				   eglib/glist.c \
				   eglib/gmarkup.c \
				   eglib/gmem.c \
				   eglib/gmisc-unix.c \
				   eglib/gmodule-unix.c \
				   eglib/goutput.c \
				   eglib/gpath.c \
				   eglib/gpattern.c \
				   eglib/gptrarray.c \
				   eglib/gqsort.c \
				   eglib/gqueue.c \
				   eglib/gshell.c \
				   eglib/gslist.c \
				   eglib/gspawn.c \
				   eglib/gstr.c \
				   eglib/gstring.c \
				   eglib/gtimer-unix.c \
				   eglib/gunicode.c \
				   eglib/gutf8.c \
				   eglib/vasprintf.c \
				   cyo/CyoDecode.c \
				   cyo/CyoEncode.c \
				   sqlite3/sqlite3secure.c \
				   jm/database.c \
				   jm/livedata.c \
				   jm/livedatavector.c \
				   jm/register.c \
				   jm/serial.c 

LOCAL_CFLAGS := -DSQLITE_HAS_CODEC
LOCAL_C_INCLUDES += eglib
LOCAL_SHARED_LIBRARIES += crypto
LOCAL_LDLIBS += -llog
TARGET_PLATFORM := android-7				   
include $(BUILD_SHARED_LIBRARY)
$(call import-module, openssl)
