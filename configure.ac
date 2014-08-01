AC_INIT([fastrest], [0.1], [gilles@gagniard.org])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

# configure for C++11 support

AC_LANG([C++])
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_SUBST([AM_CXXFLAGS], [-std=c++11])
AC_PROG_RANLIB
AC_C_INLINE

# check for boost

BOOST_REQUIRE([1.54])

BOOST_ASIO
BOOST_LOG
BOOST_LOG_SETUP
BOOST_THREADS

# generate makefiles

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    include/Makefile
    samples/Makefile
    samples/helloworld/Makefile
])


AC_OUTPUT
