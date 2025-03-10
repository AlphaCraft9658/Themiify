# Themiify
[![CI-Release](https://github.com/Themiify-hb/Themiify/actions/workflows/build-release.yml/badge.svg)](https://github.com/Themiify-hb/Themiify/actions/workflows/build-release.yml)  [![CI-Develop](https://github.com/Themiify-hb/Themiify/actions/workflows/build-dev.yml/badge.svg?branch=dev)](https://github.com/Themiify-hb/Themiify/actions/workflows/build-dev.yml)

A new WIP theme manager for the Wii U console

(This theme manager will be started soon and is in very early stages, so it will not work for a while at all)

## Idea
This theme manager will be available as a .wuhb file and utilize SDCafiine for applying themes. There is a planned patch to add a feature to SDCafiine to better auto-load modpacks or themes in this case.

## Clone the repository
In order to clone the dev branch of the repository for compiling or contributing you will need to run the following command:
```
git clone -b dev --recurse-submodules https://github.com/AlphaCraft9658/Themiify
```

## Get the latest test build
You can find the latest testing builds in the [GitHub Actions](https://github.com/Themiify-hb/Themiify/actions) at [CI-Release](https://github.com/Themiify-hb/Themiify/actions/workflows/build-release.yml) and [CI-Develop](https://github.com/Themiify-hb/Themiify/actions/workflows/build-dev.yml) of this repo or by clicking on the CI-Badges above. But keep in mind that those artifacts expire after 90 days! So until we publish stable builds that are meant for regular use you'll either need to use builds from the GitHub actions or build the repo yourself.

## Building this repo
While it is possible to manually install dependencies and build the repo with the Makefile, it's easier with the official, easily reproducible building container. So when you need to build it yourself when you want to build a specific branch for example, that has no GitHub workflow assigned, you can change your working directory to the root of the repo/branch and run the following command:

```
docker compose up -f build-compose.yaml up --abort-on-container-exit
```

*Optionally run this afterwards, to clean up any possible dangling containers:*

```
docker compose -f build-compose.yaml down
```

> **IMPORTANT NOTE:** If you want to use this method of building the project, you will need to make sure that you have both Docker and Docker Compose installed on your machine, or else it will not work. Docker normally just runs on *Linux* and *MacOS*, so if you want to use this specific building method on Windows, you will need to set up WSL first and then Docker and the repo inside of it. Though the process of setting up and using WSL is beyond the scope of this README.

### TODO
Add notes about reproducible Docker-based VSCode development environment
