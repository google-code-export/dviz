Summary: Modular Log Analyzer (modlogan)
Name: pxtools
Version: 0.0.20
Release: 1
Source: http://www.kneschke.de/projekte/pxtools/download/pxtools-%version.tar.gz
Packager: Jan Kneschke <jan@kneschke.de>
License: GPL
Group: Database/Converters
BuildRoot: /var/tmp/%{name}-root

%description
The PXtools are a collection of utilities to analyze and convert Paradox
database files to SQL, XML or CSV files. 

%prep

%setup

%build
./configure --prefix=/usr 

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%post 

%postun 

%changelog

* Thu Jun 22 2001 <jan@kneschke.de>
- initial version

%files
%defattr(-,root,root)
%doc AUTHORS ChangeLog INSTALL NEWS README
/usr/bin/pxinfo
/usr/bin/pxsqldump
/usr/bin/pxcsvdump
/usr/bin/pxxmldump
