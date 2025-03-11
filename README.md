# Themiify
[![CI-Release](https://github.com/Themiify-hb/Themiify/actions/workflows/build-release.yml/badge.svg)](https://github.com/Themiify-hb/Themiify/actions/workflows/build-release.yml)  [![CI-Develop](https://github.com/Themiify-hb/Themiify/actions/workflows/build-dev.yml/badge.svg?branch=dev)](https://github.com/Themiify-hb/Themiify/actions/workflows/build-dev.yml)

A new WIP theme manager for the Wii U console

## TODO
This list is comprised of a couple accomplishments and ideas for future updates, but is not final and contents may added or removed at any point in time.
- [x] Implement patching functionality
- [x] Implement a working UI for patching and managing local themes
  - [ ] Rework UI to use proper graphics libraries like SDL
    - [ ] Local theme manager
    - [ ] Settings page
    - [ ] Theme browser
    - [ ] Credits & About
    - [ ] Further UI elements ( to be extended )
- [ ] Final decision on whether to endorse SDCafiine or StyleMiiU
  - [ ] In case of SDCafiine: Get option for favorite modpack/theme selection to be implemented
- [ ] Implement online functionality
  - [ ] Browse online themes
  - [ ] Install online themes
  - [ ] Update online themes
  - [ ] Option to auto-update themes when launching Themiify
  - [ ] Option to select whether metadata and the theme list should be fetched from the theme repo automatically (e.g. when opening Themiify or the theme browser) or only manually
  - [ ] Version locking for themes (idea: if a theme that is available online is further modified by the user while retaining its metadata, they may want to keep Themiify from updating it)
- [ ] Add notes about reproducible Docker-based VSCode development environment

## Get the latest test build
You can find the latest test builds in the [GitHub Actions](https://github.com/Themiify-hb/Themiify/actions) at [CI-Release](https://github.com/Themiify-hb/Themiify/actions/workflows/build-release.yml) and [CI-Develop](https://github.com/Themiify-hb/Themiify/actions/workflows/build-dev.yml). But keep in mind that those artifacts expire after 90 days! So until we publish stable builds that are meant for regular use you'll either need to use builds from the GitHub Actions as long as they aren't expired or build the repo yourself.

## Creating themes
You can follow the guides over on the [Theme Café Discord Server](https://discord.gg/GsdQzbAejT) for creating your own theme.<br/>
In order to package your theme for use with Themiify you can use the official [utheme-tool](https://github.com/Themiify-hb/utheme-tool) with further instructions on its own repo and in the actual application.

> **NOTE:** [utheme-tool](https://github.com/Themiify-hb/utheme-tool) is still in development, so expect bugs and don't be afraid of reporting any issues you experience with it.

## Clone the repository

In order to clone the dev branch of the repository for compiling or contributing you will need to run the following command:
```
git clone -b dev https://github.com/AlphaCraft9658/Themiify
```

## Building Themiify
### Using make
#### Requirements
- [devkitpro/devkitppc](https://devkitpro.org/wiki/Getting_Started)
- [libmocha](https://github.com/wiiu-env/libmocha/tree/main)
#### Building
Run the following command on the root of the repo:
```
make
```
You can use wiiload to send the Themiify.wuhb file directly to your Wii U or you can manually place it on the SD-Card once you're done building.

### Using Docker
If you are at least somewhat familiar with Docker or have it installed on your system, this build method might be easier and quicker to use.<br/>
#### Requirements
- [Docker Engine](https://docs.docker.com/engine/install/) or [Docker Desktop](https://docs.docker.com/desktop/)
- [Docker compose](https://docs.docker.com/compose/install/) - ( If you installed [Docker Engine](https://docs.docker.com/engine/install/) )
- ( [WSL](https://learn.microsoft.com/en-us/windows/wsl/) or [Hyper-V](https://learn.microsoft.com/en-us/windows-server/virtualization/hyper-v/hyper-v-overview) if you are on Windows<br/>
*Note: [Docker Desktop](https://docs.docker.com/desktop/setup/install/windows-install/) integrates with [WSL](https://learn.microsoft.com/en-us/windows/wsl/) and [Hyper-V](https://learn.microsoft.com/en-us/windows-server/virtualization/hyper-v/hyper-v-overview),
but you can also run [Docker Engine](https://docs.docker.com/engine/install/) itself via either of those* )

#### Building
Run the following command on the root of the repo:
```
docker compose up -f build-compose.yaml up --abort-on-container-exit
```

Optionally run this afterwards, to clean up any possible dangling containers:
```
docker compose -f build-compose.yaml down
```