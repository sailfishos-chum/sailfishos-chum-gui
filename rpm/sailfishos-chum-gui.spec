Name:           sailfishos-chum-gui
Summary:        GUI application for utilising the SailfishOS:Chum community repository
Version:        0.5.5
Release:        1
Group:          Applications/System
License:        MIT
URL:            https://github.com/sailfishos-chum/%{name}
Source0:        %{url}/archive/%{version}/%{name}-%{version}.tar.gz
Source1:        token-github.txt
Source2:        token-gitlab.txt
Source99:       %{name}.rpmlintrc
Requires:       sailfishsilica-qt5 >= 0.10.9
Requires:       ssu
Requires(postun): ssu
Conflicts:      %{name}-installer
Obsoletes:      %{name}-installer 
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

# Bundle YAML library only for builds at OBS corresponding to older SFOS version targets < v4.0.0.00
%if 0%{?sailfishos_version} && 0%{?sailfishos_version} < 40000
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
 - Network
 - Settings
 - PackageManager
Custom:
  Repo: %{url}
Icon: %{url}/raw/main/icons/sailfishos-chum-gui.svg
Url:
  Homepage: https://openrepos.net/content/olf/sailfishoschum-gui-installer
  Help: %{url}/issues
  Bugtracker: %{url}/issues
%endif

%prep
%setup -q

%build
# SFOS RPM cmake macro disables RPATH
%cmake -DCHUMGUI_VERSION=%(echo %{version} | grep -Eo '^[0-9]+(\.[0-9]+)*')  \
       -DCMAKE_SKIP_RPATH:BOOL=OFF  \
       -DCMAKE_INSTALL_RPATH=%{_datadir}/%{name}/lib:  \
       -DGITHUB_TOKEN=%(cat %{SOURCE1})  \
       -DGITLAB_TOKEN=%(cat %{SOURCE2})  \
       .
cmake --build .

%install
%make_install
desktop-file-install --delete-original  \
                     --dir %{buildroot}%{_datadir}/applications  \
                     %{buildroot}%{_datadir}/applications/*.desktop

%if 0%{?bundle_yaml}
mkdir -p %{buildroot}%{_datadir}/%{name}/lib
cp -a %{_libdir}/libyaml-cpp.so.* %{buildroot}%{_datadir}/%{name}/lib
# Strip executable bit from all libraries
chmod -x %{buildroot}%{_datadir}/%{name}/lib/*.so*
%endif

# Rectify desktop file for old SailfishOS releases < v4.0.1.00
%if 0%{?sailfishos_version} && 0%{?sailfishos_version} < 40100
sed -i 's/silica-qt5/generic/' %{buildroot}%{_datadir}/applications/sailfishos-chum-gui.desktop
%endif

%postun
if [ "$1" = 0 ]  # Removal
then
  ssu rr sailfishos-chum
  ssu rr sailfishos-chum-testing
  rm -f /var/cache/ssu/features.ini
  ssu ur
  # Remove a %%{name}-installer log-file, if there.
  # This is deliberately solely done when removing %%{name}, because users tend
  # to try things until %%{name} is installed and subsequently report issues with
  # the %%{name}-installer, hence the log file shall be still there, then.
  rm -f %{_localstatedir}/log/%{name}-installer.log.txt
fi
# BTW, `ssu`, `rm -f`, `mkdir -p` etc. *always* return with "0" ("success"), hence
# no appended `|| true` needed to satisfy `set -e` for failing commands outside of
# flow control directives (if, while, until etc.).  Furthermore on Fedora Docs it
# is indicated that solely the final exit status of a whole scriptlet is crucial: 
# See https://docs.pagure.org/packaging-guidelines/Packaging%3AScriptlets.html
# or https://docs.fedoraproject.org/en-US/packaging-guidelines/Scriptlets/#_syntax
# committed on 18 February 2019 by tibbs ( https://pagure.io/user/tibbs ) in
# https://pagure.io/packaging-committee/c/8d0cec97aedc9b34658d004e3a28123f36404324
# Hence I have the impression, that only the main section of a spec file is
# interpreted in a shell called with the option `-e', but not the scriptlets
# (`%%pre*`, `%%post*`, `%%trigger*` and `%%file*`).
exit 0

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
