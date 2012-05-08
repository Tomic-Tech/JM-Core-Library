LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jmcore
### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := cyo/CyoDecode.c \
				   cyo/CyoEncode.c \
				   sqlite/sqlite3.c \
				   jm/asio/serialport.cpp \
				   jm/diag/v1/boxversion.cpp \
				   jm/diag/boxfactory.cpp \
				   jm/diag/boxlink.cpp \
				   jm/diag/canbus.cpp \
				   jm/diag/commbox.cpp \
				   jm/diag/kwp1281.cpp \
				   jm/diag/kwp2000.cpp \
				   jm/diag/livedata.cpp \
				   jm/diag/livedatavector.cpp \
				   jm/diag/mikuni.cpp \
				   jm/diag/stream.cpp \
				   jm/diag/virtualstream.cpp \
				   jm/system/app.cpp \
				   jm/system/database.cpp \
				   jm/system/register.cpp \
				   jm/ui/livedataprepare.cpp \
				   jm/ui/livedataview.cpp \
				   jm/ui/menu.cpp \
				   jm/ui/message.cpp \
				   jm/ui/msgbox.cpp \
				   jm/ui/statusbox.cpp \
				   jm/ui/troublecode.cpp \
				   jm/vehicle/database.cpp \
				   jm/vehicle/loader.cpp
				   
				   
LOCAL_CFLAGS := -DANDROID \
				-DSQLITE_HAS_CODEC \
				-DSQLITE_NO_SYNC \
				-DJMCORE_BUILD
				
LOCAL_STATIC_LIBRARIES += boost_thread \
						  boost_system \
						  crypto

include $(BUILD_SHARED_LIBRARY)
$(call import-module, boost_1_47_0)
$(call import-module, openssl)