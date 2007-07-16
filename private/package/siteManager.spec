Summary: Web Application Toolkit and library for PHP 4.x
Name: siteManager
Version: SM_VERSION
Release: 1
Copyright: QPL
Group: Development/Libraries
Source: http://www.roadsend.com/siteManager/download/siteManager-%{version}.tar.gz
URL: http://www.roadsend.com/siteManager
Vendor: Roadsend, Inc.
Buildarch: noarch

%define prefix /usr/local/lib/php/siteManager/
%define smdir siteManager-%{version}

%description
 SiteManager is an Open Source Web Application Toolkit for PHP Developers 
 that includes a framework for code modules and layout templates, database 
 connectivity, SmartForms, sessions, and some other tools. 
 
 All parts of the system are integrated through object-oriented libraries 
 which allow for easy expandability and maintenance.

%prep
# this will clean old version, and unpack new one
rm -rf $RPM_BUILD_DIR/%{smdir}
tar zxvf $RPM_SOURCE_DIR/siteManager-%{version}.tar.gz

# remove prefix dir
rm -rf %{prefix}

if [ -a /usr/bin/siteManager ]; then
    rm /usr/bin/siteManager
fi


%build
# nothing

%install
# copy all of the siteManager files to installation directory
cp -R $RPM_BUILD_DIR/%{smdir} %{prefix}
# move bin script to /usr/bin
mv %{prefix}bin/siteManager /usr/bin/siteManager
rmdir %{prefix}bin

%files
%config %{prefix}config/globalConfig.xsm
%doc %{prefix}doc/
/usr/bin/siteManager
%{prefix}siteManager.inc
%{prefix}cache/
%{prefix}lib/
%{prefix}modules/
%{prefix}skeletonSite/
%{prefix}smartForms/
%{prefix}tables/
%{prefix}templates/
%{prefix}testSite/

%changelog

