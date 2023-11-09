# Keyboard Cleaner

A cross-platform system tray tool for disabling your keyboard.

> [!WARNING]
> MacOS support is experimental as the current implementation is not user friendly.<br>
> For some reason the functionality that blocks keyboard input has to be on a different process
> than the one running the UI, resulting in requiring 2 programs to be open at the same time.

## Requirements

### Linux

-   A system tray
-   `libevdev-dev` package (on Ubuntu, may have a different name on other distros)
-   `qt6-base-dev` package (on Ubuntu, may have a different name on other distros)

### MacOS

-   `qt` package from homebrew

## Usage

### Linux

1. Add your user to the `input` group - `sudo usermod -aG input $USER`. This allows to block input without root permissions
2. Run the `keyboard-cleaner` binary
3. Click on the icon in the system tray
4. Select your keyboard from the menu
5. Press `Disable keyboard`

### Windows

1. Run `keyboard-cleaner.exe`
2. Click on the icon in the system tray
3. Press `Disable keyboard`

> [!NOTE]
> Ctrl-Alt-Del still works as it is impossible to override it on Windows

### MacOS

1. Run the `keyboard-cleaner-service` binary (a terminal window will pop up, you can minimize it)
2. Run the `keyboard-cleaner` app bundle
3. Click on the icon in the system tray
4. Press `Disable keyboard`
