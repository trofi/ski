%define RELEASE gplv2.iPDL.1
%define _prefix /usr/local

Summary:       The Ski simulator
Name:          ski
Version:       1.1.3
Release:       %{RELEASE}
Copyright:     Copyright Hewlett-Packard Development Company, L.P.
Vendor:        Hewlett-Packard
Group:         Applications/Simulator
Source:        ftp://golbat.cup.hp.com/pub/dists/ski/%{name}-%{version}.tar.gz
Url:           http://golbat.cup.hp.com/ski.html
BuildRoot:     %{_tmppath}/%{name}-%{PACKAGE_VERSION}-root
Prefix:        %{_prefix}
%ifos linux
Prereq:        /sbin/ldconfig
%endif

%description
The Ski simulator

%package devel
Summary: The Ski simulator support library.
Group: Development/Libraries
Requires: ski = %{version}

%description devel
The ski-devel package includes the static libraries and header files
for the support library for the Ski simulator.

Install ski-devel if you want to develop programs which will use Ski.

%prep
%setup -q

%build
%ifos linux
CONFIGURE_FLAGS="--with-bfd-includes=/usr/ia64-redhat-linux/include/i386 \
                 --with-bfd-libs=/usr/ia64-redhat-linux/lib/i386"
%endif

if test "x$CC" = xicc; then
    export CFLAGS="-O3 -tpp6 -axiM"
    RPM_BUILD_NCPUS=1
else
    export CFLAGS="-O3 -fno-strict-aliasing -fomit-frame-pointer \
                   `echo $RPM_OPT_FLAGS | sed -e 's|-O[0-9]*[ \t\r]*||g'`";
fi

%configure --enable-shared $CONFIGURE_FLAGS

# if RPM_BUILD_NCPUS unset, set it
if [ -z "$RPM_BUILD_NCPUS" ] ; then
%ifos linux
    if [ -x /usr/bin/getconf ] ; then
        RPM_BUILD_NCPUS=$(/usr/bin/getconf _NPROCESSORS_ONLN)
        if [ $RPM_BUILD_NCPUS -eq 0 ]; then
            RPM_BUILD_NCPUS=1
        else
            RPM_BUILD_NCPUS=`expr $RPM_BUILD_NCPUS \* 3 / 2`
        fi
    else
        RPM_BUILD_NCPUS=1
    fi
%else
    RPM_BUILD_NCPUS=1
%endif
fi

make -j$RPM_BUILD_NCPUS

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

strip $RPM_BUILD_ROOT%{_bindir}/ski
ln -f $RPM_BUILD_ROOT%{_bindir}/ski $RPM_BUILD_ROOT%{_bindir}/xski
ln -f $RPM_BUILD_ROOT%{_bindir}/ski $RPM_BUILD_ROOT%{_bindir}/bski
mkdir -p $RPM_BUILD_ROOT/usr/lib/X11/app-defaults
cp $RPM_BUILD_ROOT%{_libdir}/X11/app-defaults/XSki \
    $RPM_BUILD_ROOT/usr/lib/X11/app-defaults

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%ifos linux
%post
{
    grep "^%{prefix}/lib$" /etc/ld.so.conf &> /dev/null
    [ $? -ne 0 ] && echo "%{prefix}/lib" >> /etc/ld.so.conf
    /sbin/ldconfig
}

%postun
{
    if [ "$1" = "0" ]; then
        grep -v "^%{prefix}/lib$" /etc/ld.so.conf > /etc/ld.so.conf.new
        # preserve permissions
        cat /etc/ld.so.conf.new > /etc/ld.so.conf
        rm -f /etc/ld.so.conf.new
    fi
    /sbin/ldconfig
}
%endif

%files
%defattr(-, root, root)

%{_bindir}/ski
%{_bindir}/xski
%{_bindir}/bski
%{_libdir}/libski*.s?.*
/usr/lib/X11/app-defaults/XSki

%files devel
%defattr(-, root, root)

%{_bindir}/ski-config
%{_libdir}/lib*.s?
%{_libdir}/lib*.a
%{_includedir}/*

%changelog
* Tue May 14 2002 Laurent Morichetti <laurentm@cup.hp.com> (0.9.77 release)
- Initial
