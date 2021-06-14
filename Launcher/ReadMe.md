# Lode Runner 2020 - Launcher

This is folder contains the source for the launcher of the [Lode Runner 2020](https://github.com/Ma-Pe-Ma/Lode-Runner-2020) game.

For generic project-specific informations please checkout the root ReadMe.

This ReadMe only contains information about the launcher.

## Building

A QtCreator project file is included. After cloning the repo, simply open the project file and you should be able to build it with the IDE.

(If you have installed a Qt version and a compiler.)

## Developer notes

It was developed with Qt 5.15.2. It was deployed dynamically so it contains many linked libs. Because of this the size of it is unnecessary large.

This launcher simply parses the config.txt file and after launching it recreates it according to user input.

Then the game executable reads it for itself and reacts accordingly.

## Downloading built version

At the release section you can find different versions, the filename tells you if the launcher is included or not.

## Screenshot

![](../Screenshots/Launcher.png)

## Future plans

1. Statically link Qt with the app
2. Merge source into main LodeRunner.exe, so the launcher and the game should be in one common executable. 