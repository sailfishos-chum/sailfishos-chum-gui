Name:           sailfishos-chum-gui
Summary:        Chum GUI
Version:        0.1
Release:        1
Group:          Applications/System
License:        MIT
URL:            https://github.com/sailfishos-chum/sailfishos-chum-gui
Source0:        %{name}-%{version}.tar.bz2
Requires:       sailfishsilica-qt5 >= 0.10.9
Requires:       sailfishos-chum
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils
BuildRequires:  cmake >= 3.11
BuildRequires:  sailfish-svg2png
BuildRequires:  PackageKit-Qt5-devel
BuildRequires:  qt5-qttools-linguist

%description
A client app for the Chum repositories

%prep
%setup -q -n %{name}-%{version}

%build
%cmake -DCHUMGUI_VERSION=%(echo %{version} | grep -Eo '^[0-9]+(\.[0-9]+)*') .
cmake --build .

%install
rm -rf %{buildroot}
%make_install
desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
