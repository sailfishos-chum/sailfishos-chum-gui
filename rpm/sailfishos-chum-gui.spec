Name:           sailfishos-chum-gui
# Note that the value of %%{name} must be the project name at GitHub.
Summary:        GUI application for utilising the SailfishOS:Chum community repository
# The Version field must adhere to semantic versioning, see https://semver.org/
Version:        0.6.11
# The Release field should comprise a natural number greater or equal to 1,
# which may be prefixed with one of {alpha,beta,rc,release} (e.g. "beta3").
# For details and reasons, see
# https://github.com/storeman-developers/harbour-storeman-installer/wiki/Git-tag-format
Release:        3
# The Group field should comprise one of the groups listed here:
# https://github.com/mer-tools/spectacle/blob/master/data/GROUPS
Group:          Software Management/Package Manager
License:        MIT
URL:            https://github.com/sailfishos-chum/%{name}
Vendor:         chum
# Note that the git-tag format for release versions must be `%%{release}/%%{version}`:
Source0:        %{url}/archive/%{release}/%{version}/%{name}-%{version}.tar.gz
Source1:        token-github.txt
Source2:        token-gitlab.txt
Source3:        token-forgejo.txt
# Note that the rpmlintrc file must be named so according to
# https://en.opensuse.org/openSUSE:Packaging_checks#Building_Packages_in_spite_of_errors
Source99:       %{name}.rpmlintrc
Requires:       sailfishsilica-qt5 >= 0.10.9
Requires:       ssu
Requires(postun): ssu
# The oldest SailfishOS release which SailfishOS:Chum supports, because it is the
# oldest useable DoD-repo at https://build.sailfishos.org/project/subprojects/sailfishos
Requires:       sailfish-version >= 3.1.0
# Provide (anti-)dependencies to sibling packages:
Conflicts:      %{name}-installer
Obsoletes:      %{name}-installer 
Conflicts:      sailfishos-chum
Obsoletes:      sailfishos-chum
Conflicts:      sailfishos-chum-testing
Obsoletes:      sailfishos-chum-testing
Conflicts:      sailfishos-chum-repo-config
Obsoletes:      sailfishos-chum-repo-config
Conflicts:      sailfishos-chum-repo-config-testing
Obsoletes:      sailfishos-chum-repo-config-testing
Provides:       sailfishos-chum-repository
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(yaml-cpp)
BuildRequires:  pkgconfig(packagekitqt5)
BuildRequires:  desktop-file-utils
BuildRequires:  cmake >= 3.11
BuildRequires:  sailfish-svg2png
BuildRequires:  qt5-qttools-linguist
BuildRequires:  sed

# Bundle YAML-C++ library (for OBS builds) only for older SFOS version targets < v4.0.0
%if %{defined sailfishos_version} && 0%{?sailfishos_version} < 40000
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
%if 0%{?_chum}
Title: SailfishOS:Chum GUI application
Type: desktop-application
Categories:
 - System
 - Utility
 - Network
 - Settings
 - PackageManager
Custom:
  Repo: %{url}
PackageIcon: %{url}/raw/main/icons/%{name}.svg
Screenshots:
 - %{url}/raw/main/.screenshots/%{name}_retrieving-refreshed.png
 - %{url}/raw/main/.screenshots/%{name}_main-page.png
 - %{url}/raw/main/.screenshots/%{name}_settings.png
 - %{url}/raw/main/.screenshots/%{name}_applications.png
 - %{url}/raw/main/.screenshots/%{name}_categories.png
 - %{url}/raw/main/.screenshots/%{name}_installed-packages.png
Links:
  Homepage: https://openrepos.net/content/olf/sailfishoschum-gui-installer
  Help: %{url}/issues
  Bugtracker: %{url}/issues
%endif

# Define (S)RPM compression sensibly, also taking compatibility into account, see
# https://github.com/sailfishos-patches/patchmanager/pull/417#issuecomment-1429068156
%define _source_payload w6.gzdio
%define _binary_payload w2.xzdio

%prep
%setup -q

%build
# SFOS RPM cmake macro disables RPATH
%cmake -DCHUMGUI_VERSION=%(echo %{version} | grep -Eo '^[0-9]+\.[0-9]+\.[0-9]+')  \
       -DSAILFISHOS_TARGET_VERSION=%{!?sailfishos_version:0}%{?sailfishos_version}  \
       -DCMAKE_SKIP_RPATH:BOOL=OFF  \
       -DCMAKE_INSTALL_RPATH=%{_datadir}/%{name}/lib:  \
       -DGITHUB_TOKEN=%(cat %{SOURCE1})  \
       -DGITLAB_TOKEN=%(cat %{SOURCE2})  \
       -DFORGEJO_TOKEN=%(cat %{SOURCE3})  \
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

# Rectify desktop file for older SFOS version targets < v4.1.0
%if %{defined sailfishos_version} && 0%{?sailfishos_version} < 40100
sed -i 's/silica-qt5/generic/' %{buildroot}%{_datadir}/applications/sailfishos-chum-gui.desktop
%endif

%postun
# Removal:
if [ "$1" = 0 ]
then
  ssu rr sailfishos-chum
  ssu rr sailfishos-chum-testing
  ssu ur
  # Remove a %%{name}-installer log-file, if there.
  # This is deliberately solely done when removing %%{name}, because users tend
  # to try things until %%{name} is installed and subsequently report issues with
  # the %%{name}-installer, hence the log-file shall be still there, then.
  rm -f %{_localstatedir}/log/%{name}-installer.log.txt
fi
# BTW, `ssu`, `rm -f`, `mkdir -p` etc. *always* return with "0" ("success"), hence
# no appended `|| true` needed to satisfy `set -e` for failing commands outside of
# flow control directives (if, while, until etc.).  Furthermore Fedora Docs etc.
# state that solely the final exit status of a whole scriptlet is crucial: 
# See https://docs.pagure.org/packaging-guidelines/Packaging%3AScriptlets.html
# or https://docs.fedoraproject.org/en-US/packaging-guidelines/Scriptlets/#_syntax
# committed on 18 February 2019 by tibbs ( https://pagure.io/user/tibbs ) in
# https://pagure.io/packaging-committee/c/8d0cec97aedc9b34658d004e3a28123f36404324
# Hence only the main section of a spec file and likely also `%%(<shell-script>)`
# statements are executed in a shell invoked with the option `-e', but not the
# scriptlets: `%%pre*`, `%%post*`, `%%trigger*` and `%%file*`
exit 0

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/mapplauncherd/privileges.d/%{name}
