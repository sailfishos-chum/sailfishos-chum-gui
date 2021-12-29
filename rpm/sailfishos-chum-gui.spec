Name:           sailfishos-chum-gui
Summary:        Chum GUI
Version:        0.2.0
Release:        1
Group:          Applications/System
License:        MIT
URL:            https://github.com/sailfishos-chum/sailfishos-chum-gui
Source0:        %{name}-%{version}.tar.bz2
Source1:        token-github.txt
Source2:        token-gitlab.txt
Requires:       sailfishsilica-qt5 >= 0.10.9
Requires:       ssu
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(yaml-cpp)
BuildRequires:  desktop-file-utils
BuildRequires:  cmake >= 3.11
BuildRequires:  sailfish-svg2png
BuildRequires:  PackageKit-Qt5-devel
BuildRequires:  qt5-qttools-linguist

%define __provides_exclude_from ^%{_datadir}/.*$
%define __requires_exclude ^libyaml-cpp.*$

%description
A client app for the Chum repositories.

PackageName: Chum GUI
Type: desktop-application
Categories:
  - System
  - Utility
Custom:
  Repo: https://github.com/sailfishos-chum/sailfishos-chum-gui
Icon: https://raw.githubusercontent.com/sailfishos-chum/sailfishos-chum-gui/main/icons/sailfishos-chum-gui.svg

%prep
%setup -q -n %{name}-%{version}

%build

# SFOS RPM cmake macro disables RPATH
%cmake -DCHUMGUI_VERSION=%(echo %{version} | grep -Eo '^[0-9]+(\.[0-9]+)*') \
      -DCMAKE_SKIP_RPATH:BOOL=OFF \
      -DCMAKE_INSTALL_RPATH=%{_datadir}/%{name}/lib: \
      -DGITHUB_TOKEN=%(cat %{SOURCE1})  \
      -DGITLAB_TOKEN=%(cat %{SOURCE2})  \
     .
cmake --build .

%install
rm -rf %{buildroot}
%make_install
desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications \
   %{buildroot}%{_datadir}/applications/*.desktop

mkdir -p %{buildroot}%{_datadir}/%{name}/lib
cp -a %{_libdir}/libyaml-cpp.so.* %{buildroot}%{_datadir}/%{name}/lib

%postun
ssu rr sailfishos-chum || true
ssu rr sailfishos-chum-testing || true
rm -f /var/cache/ssu/features.ini || true
ssu ur || true

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
