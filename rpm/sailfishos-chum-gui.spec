Name:           sailfishos-chum-gui
Summary:        GUI application for utilising the SailfishOS:Chum community repository
Version:        0.5.0
Release:        1
Group:          Applications/System
License:        MIT
URL:            https://github.com/sailfishos-chum/sailfishos-chum-gui
Source0:        %{name}-%{version}.tar.bz2
Source1:        token-github.txt
Source2:        token-gitlab.txt
Source101:      %{name}-rpmlintrc
Requires:       sailfishsilica-qt5 >= 0.10.9
Requires:       ssu
Conflicts:      sailfishos-chum
Obsoletes:      sailfishos-chum
Conflicts:      sailfishos-chum-testing
Obsoletes:      sailfishos-chum-testing
Provides:       sailfishos-chum-repository
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
BuildRequires:  sed

# Bundle YAML library only for builds at OBS corresponding to older SFOS version targets
%if 0%{?sailfishos_version} && 0%{?sailfishos_version}<40000
%define bundle_yaml 1
%endif

%if 0%{?bundle_yaml}
%define __provides_exclude_from ^%{_datadir}/.*$
%define __requires_exclude ^libyaml-cpp.*$
%endif

%description
SailfishOS:Chum GUI is a graphical application for managing software packages from the SailfishOS:Chum community repository.

# This description section includes the metadata for SailfishOS:Chum, see
# https://github.com/sailfishos-chum/main/blob/main/Metadata.md
%if "%{?vendor}" == "chum"
PackageName: SailfishOS:Chum GUI application
Type: desktop-application
Categories:
  - System
  - Utility
Custom:
  Repo: %{url}
Icon: %{url}/raw/main/icons/sailfishos-chum-gui.svg
%endif

%prep
%setup -q

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

%if 0%{?bundle_yaml}
mkdir -p %{buildroot}%{_datadir}/%{name}/lib
cp -a %{_libdir}/libyaml-cpp.so.* %{buildroot}%{_datadir}/%{name}/lib
# strip executable bit from all libraries
chmod -x %{buildroot}%{_datadir}/%{name}/lib/*.so*
%endif

%if 0%{?sailfishos_version} && 0%{?sailfishos_version}<40100
sed -i 's/silica-qt5/generic/' %{buildroot}%{_datadir}/applications/sailfishos-chum-gui.desktop
%endif

%postun
ssu rr sailfishos-chum || true
ssu rr sailfishos-chum-testing || true
rm -f /var/cache/ssu/features.ini || true
ssu ur || true

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
