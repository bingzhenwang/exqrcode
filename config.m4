dnl $Id$
dnl config.m4 for extension exqrcode

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(exqrcode, for exqrcode support,
dnl Make sure that the comment is aligned:
dnl [  --with-exqrcode             Include exqrcode support])

dnl Otherwise use enable:

 PHP_ARG_ENABLE(exqrcode, whether to enable exqrcode support,
dnl Make sure that the comment is aligned:
 [  --enable-exqrcode           Enable exqrcode support])

if test "$PHP_EXQRCODE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-exqrcode -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/exqrcode.h"  # you most likely want to change this
   if test -r $PHP_EXQRCODE/$SEARCH_FOR; then # path given as parameter
     EXQRCODE_DIR=$PHP_EXQRCODE
   else # search default path list
     AC_MSG_CHECKING([for exqrcode files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         EXQRCODE_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
   fi
  
   dnl if test -z "$EXQRCODE_DIR"; then
   dnl   AC_MSG_RESULT([not found])
   dnl   AC_MSG_ERROR([Please reinstall the exqrcode distribution])
   dnl fi

   # --with-exqrcode -> add include path
   PHP_ADD_INCLUDE($EXQRCODE_DIR/include)
	#PHP_ADD_LIBRARY_WITH_PATH(qrencode, /usr/lib, EXQRCODE_SHARED_LIBADD)

 #PHP_ADD_LIBRARY(qrencode, 1, EXQRCODE_SHARED_LIBADD)

  dnl # --with-exqrcode -> check for lib and symbol presence
  dnl   LIBNAME=qrencode # you may want to change this
  dnl   #LIBSYMBOL= QRcode_encodeString # you most likely want to change this 

   dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl  [
    dnl  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, /var/dev/qrencode/lib, EXQRCODE_SHARED_LIBADD)
   dnl   AC_DEFINE(HAVE_EXQRCODELIB,1,[ ])
  dnl  ],[
   dnl   AC_MSG_ERROR([wrong exqrcode lib version or lib not found])
  dnl  ],[
  dnl    -L$EXQRCODE_DIR/lib -lm
  dnl l ])
  
  dnl PHP_SUBST(EXQRCODE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(exqrcode, exqrcode.c, $ext_shared)
fi
#qrencode support
PHP_ARG_WITH(qrencode, for libqrencode support,
[  --with-qrencode[=DIR]        Include qrencode support])
  
if test "$PHP_QRENCODE" != "no"; then
  if test -r $PHP_QRENCODE/lib/libqrencode.la; then
    QREN_DIR=$PHP_QRENCODE
  else
    AC_MSG_CHECKING(for libqrencode in default path)
    for i in /usr/local /usr ; do
      if test -r $i/lib/libqrencode.la; then
        QREN_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$QREN_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libqrencode distribution - qrencode.h should 
                 be <qrencode-dir>/include and libqrencode.la should be in <qrencode-dir>/lib)
  fi
  PHP_ADD_INCLUDE($QREN_DIR/include)
  
  #PHP_SUBST(QREN_SHARED_LIBADD)

PHP_SUBST(EXQRCODE_SHARED_LIBADD)
  #PHP_ADD_LIBRARY_WITH_PATH(qrencode, $QREN_DIR/lib, QREN_SHARED_LIBADD)
PHP_ADD_LIBRARY_WITH_PATH(qrencode, $QREN_DIR/lib, EXQRCODE_SHARED_LIBADD)
  AC_DEFINE(HAVE_QRENCODE,1,[ ])
  
  PHP_EXTENSION(exqrcode, $ext_shared)
fi
#png

PHP_ARG_WITH(png, for libpng support,
[  --with-png[=DIR]        Include png support])
  
if test "$PHP_PNG" != "no"; then
  if test -r $PHP_PNG/lib/libpng.a; then
    QREN_DIR=$PHP_PNG
  else
    AC_MSG_CHECKING(for libpng in default path)
    for i in /usr/local /usr ; do
      if test -r $i/lib/libpng.a; then
        PNG_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$PNG_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(please install png )
  fi
  PHP_ADD_INCLUDE($PNG_DIR/include)

PHP_SUBST(EXQRCODE_SHARED_LIBADD)
PHP_ADD_LIBRARY_WITH_PATH(png, $PNG_DIR/lib, EXQRCODE_SHARED_LIBADD)
  AC_DEFINE(HAVE_PNG,1,[ ])
  
  PHP_EXTENSION(exqrcode, $ext_shared)
fi