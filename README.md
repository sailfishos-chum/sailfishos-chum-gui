# SailfishOS:Chum GUI application
SailfishOS:Chum GUI is a graphical client application for managing software packages from the SailfishOS:Chum community repository.

### Feature set
The SailfishOS:Chum GUI application deploys the same repository configuration as either of the [SailfishOS:Chum repository configuration RPMs](https://github.com/sailfishos-chum/sailfishos-chum-repo-config#readme) do, hence one can alternatively use command line tools as `pkcon` or `zypper` at any time.

The main page of the SailfishOS:Chum GUI app provides:
- In the pulley menu
  - **About** page
  - **Settings** page
    - Preset to show either applications or all packages for each package list by default
    - Switch between the regular SailfishOS:Chum and the SailfishOS:Chum:Testing repository
    - The ability to overrride the automatic SailfishOS:Chum repository selection based on the installed Sailfish&nbsp;OS release
  - **Refresh repository** trigger
- **Updates** page
  - Lists all installed packages, for which an update is available
- **Categories** page
  - Lists all categories
    - Each category lists all packages assigned to it
- **Applications** / **Packages** page
  - Package list of all applications or packages
- **Installed packages** page
  - Package list of all installed packages or applications
- All package lists
  - Searchable
  - Switchable between "applications only" and "all packages"
  - Display for each package
    - Package title
    - Categories the package is assigned to
    - The name if its develeoper
    - A package icon, if provided
    - The number of *stars* its source code repository has aquired (only for packages hosted at GitHub or GitLab.com)
    - Its install status (checkmark when installed) or if an update for an installed package is available (by a down-arrow)
- Every package page
  - All aforementioned items which are displayed for each package in a package list
  - If the package is a Sailfish application or a console application
  - The number of *forks* of its source code repository (gear in curly brackets; only for packages hosted at GitHub or GitLab.com)
  - A detailed description
  - Installed and available version, if applicable
  - Package name, download size and license (if indicated)
  - A link to its homepage
  - **Releases** sub-page (only for packages hosted at GitHub or GitLab.com)
    - Release versions and dates
    - On each release's sub-sub-page:
      - Release version and date
      - Release notes (if provided)
  - **Issues** sub-page (only for packages hosted at GitHub or GitLab.com)
    - Issue title, number and creator
    - Creation date and last updated date
    - Number of comments to the original issue report
    - On each issue's sub-sub-page:
      - Whole issue thread with all aforementioned items displayed in the thread header
      - Creator, creation date and last updated date for each message

### How to install the SailfishOS:Chum GUI application
See [sailfishos-chum/main#how-to-install-the-sailfishoschum-gui-application](https://github.com/sailfishos-chum/main#how-to-install-the-sailfishoschum-gui-application).

### Important notes
* If you experience issues when installing, removing or updating packages after a SailfishOS upgrade, try running `devel-su pkcon refresh` in a terminal app.
* Before software can be build for a SailfishOS release at the SailfishOS-OBS, Jolla must create a [corresponding "download on demand (DoD)" OBS-repository](https://build.merproject.org/project/subprojects/sailfishos).  It may take a little time after a new SailfishOS release is published before the corresponding "DoD" repository is being made available, during which installing the SailfishOS:Chum GUI application by the SailfishOS:Chum GUI Installer or updating the SailfishOS:Chum GUI application by itself on a device with the new SailfishOS release already installed does not work, because the SailfishOS:Chum GUI application cannot be compiled for this new SailfishOS release at the Sailfish-OBS, yet; consequently this is always the case for "closed beta (cBeta)" releases of SailfishOS.  In such a situation one has to manually download the SailfishOS:Chum GUI application built for the last prior SailfishOS "general availability (GA)" release (e.g., from [the SailfishOS:Chum repository](https://build.merproject.org/project/show/sailfishos:chum/sailfishos-chum-gui)), then install or update the SailfishOS:Chum GUI application via `pkcon install-local <downloaded RPM file>`, and hope that there is no change in the new SailfishOS release which breaks the SailfishOS:Chum GUI application; if there is, please report that soon at [SailfishOS:Chum GUI application's issue tracker](https://github.com/sailfishos-chum/sailfishos-chum-gui/issues).
* Disclaimer: The SailfishOS:Chum GUI application and its installer may still have flaws, kill your kittens or break your SailfishOS installation!  Although this is very unlikely after years of testing by many users, new flaws may be introduced in any release (as for any software).  Mind that the license you implicitly accept by using the SailfishOS:Chum GUI application or the SailfishOS:Chum GUI Installer excludes any liability.

### Translating (i18n / l10n) the SailfishOS:Chum GUI application
If you want to translate the SailfishOS:Chum GUI application to a language it does not support yet or enhance an existing translation, please read the [translation README](https://github.com/sailfishos-chum/sailfishos-chum-gui/tree/main/translations#readme).

<sup>**Translators**: [@atlochowski](https://github.com/atlochowski) (`pl`), [@black-sheep-dev](https://github.com/black-sheep-dev) (`de`), [@carmenfdezb](https://github.com/carmenfdezb) (`es`), [@dikonov](https://github.com/dikonov) (`ru`), [@eson57](https://github.com/eson57) (`sv`), [@jaimeMF](https://github.com/jaimeMF) (`es`), [@martonmiklos](https://github.com/martonmiklos) (`hu`), [@Olf0](https://github.com/Olf0) (`de` etc.), [@okruhliak](https://github.com/okruhliak) (`sk`), [@pherjung](https://github.com/pherjung) (`fr`), [@phklrz](https://github.com/phklrz) (`fr`), [@spodermenpls](https://github.com/spodermenpls) (`de`)</sup>

### Screenshots of the SailfishOS:Chum GUI application
|     |     |     |
| --- | --- | --- |
| ![RetrievingRepoRefreshed](https://github.com/sailfishos-chum/sailfishos-chum-gui/raw/main/.screenshots/sailfishos-chum-gui_retrieving-refreshed.png) | ![MainPage](https://github.com/sailfishos-chum/sailfishos-chum-gui/raw/main/.screenshots/sailfishos-chum-gui_main-page.png) | ![Settings](https://github.com/sailfishos-chum/sailfishos-chum-gui/raw/main/.screenshots/sailfishos-chum-gui_settings.png) |
| ![Applications](https://github.com/sailfishos-chum/sailfishos-chum-gui/raw/main/.screenshots/sailfishos-chum-gui_applications.png) | ![Categories](https://github.com/sailfishos-chum/sailfishos-chum-gui/raw/main/.screenshots/sailfishos-chum-gui_categories.png) | ![InstalledPackages](https://github.com/sailfishos-chum/sailfishos-chum-gui/raw/main/.screenshots/sailfishos-chum-gui_installed-packages.png) |

### Developers
- [Rinigus](https://github.com/rinigus)
- [olf (Olf0)](https://github.com/Olf0)
- [Adam Pigg (piggz)](https://github.com/piggz)
- [Petr Tsymbarovich (mentaljam / osetr)](https://github.com/mentaljam)

### Credits
- [SailfishOS:Chum OBS repository](https://build.merproject.org/project/show/sailfishos:chum) hosted at the [SailfishOS-OBS](https://build.merproject.org/), which is maintained by [Jolla Ltd.](https://jolla.com/)
- [SailfishOS:Chum community repository (source code)](https://github.com/orgs/sailfishos-chum/repositories?type=all) hosted at [GitHub](https://github.com/about)
- [SailfishOS:Chum (GUI) icon(s)](https://github.com/sailfishos-chum/sailfishos-chum-gui-installer/tree/main/.icons#readme) by [olf (Olf0)](https://github.com/Olf0)
