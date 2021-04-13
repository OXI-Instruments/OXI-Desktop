# Oxi One Companion app

This app is supposed to help the user managing the Oxi One. It will be possible to install new firmware, make backups
of the data, manage projects and change settings.

At the moment, only firmware updates are implemented.

**If you're looking for the ready-to-use app, have a look at the [releases](/releases).**

## Technical overview

This application is implemented using the following technologies:
- Python3 for all business logic
- Qt6 for the frontend (QML) and some backend work (threading, useful library functions)
- Mido python library with the rtmidi (c++) backend for MIDI communication
- briefcase framework for packaging and distribution

# Building and running

## Run

It is **highly** recommend using a python venv for building and development. We will not provide any support for
different setups.

### Qt6 Framework

The Qt6 libraries must be accessible on your system. If you're on Linux, it's very likely that Qt6 is available in
the repositories of your distribution.

If you're on MacOS or Windows, please install the Qt6 framework using the [official installer](https://www.qt.io/download-qt-installer).

### Python 3

**This project requires Python 3! Nobody should use Python 2 for anything because it's officially deprecated.**

On MacOS you can have it just by installing Xcode. There are other ways as well,
but we will not cover it here.

Building on Windows was not tested yet. If you want to experiment, have a look at this guide: https://docs.python.org/3/using/windows.html

### Virtual Environment

After successful installation follow these steps:

Create a python venv:
```
$ python -m venv oxi-venv
```

Activate the venv:
```
$ source ./oxi-venv/bin/activate
```

Install all required packages:
```
$ pip3 install -r ./requirements.txt
```


### Finally running it

It is possible to run the application using briefcase which was installed into our virtual environment:
```
$ briefcase dev
```

Alternatively you can run the app with python directly:
```
$ python3 src/oxionecompanion/__main__.py
```

## Build

### MacOS

**To build a distributable app for MacOS you need an [Apple developer certificate](https://developer.apple.com/support/developer-id/).**

Create the app structure with briefcase: (only required once)
```
$ briefcase create
```

Build the app (the `-u` switch ensures that changes in the source are available in the app):
```
$ briefcase -u build
```

Test your build:
```
$ briefcase run
```

If the app doesn't start, it's very likely that you need code signing. From this point, an Apple
developer certificate is mandatory.

To sign the app, run the code sign command like this:
```
codesign --deep --strict -s <INSERT_YOUR_APPLE_ID_HERE> -f --timestamp ./macOS/app/Oxi\ One\ Companion/Oxi\ One\ Companion.app
```

Now try running the app with `briefcase run` again. If this still doesn't work, make sure that your code has no errors
and that your code signing is set up properly.

If this went fine, you can build a distributable `.dmg` image with this:

```
briefcase package -p dmg --no-sign
```

### Linux
*TBD*
### Windows
*TBD*