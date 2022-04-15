# OXI-Desktop

OXI-Desktop is a free software to update the firmware of your [OXI One](https://oxiinstruments.com/oxi-one/) and to transfer projects between the the computer and your OXI One. It runs on Mac, Windows and Linux.

The project is in very early development stage and it has limitations and known bugs.

![Screenshot](doc/img/screenshot.png)

## Usage

### Firmware Update
To update the firmware of you OXI select the **FW UPDATE** tab and choose the `.syx` file when chosing to update either the OXI Firmware, Bluetooth or Split.
The latest stable firmware can be found on the [OXI One Update files](https://oxiinstruments.com/oxi-one/update) section.

### Syncing Projects
You can sync your projects between the OXI One and your computer. Select the project number and click **GET PROJECT**. The project and the patterns it contains will be downloaded from the OXI One onto your computer. The desktop directory `OXI Files` now contains the downloaded data.

To send any project back to any project slot of your OXI One, select the desired slot and press **SEND PROJECT**. The file selection dialog lets you choose the `.oxipro` project file to upload.

## Community

For questions, suggestions and bug reports, please reach out to our community.
* https://oxiinstruments.com/forum/


## Source Code

The source code for this project is hosted on GitLab.
* https://gitlab.com/manuwind5/Oxi-Desktop-App

The _downstream_ repository on GitHub is used for publishing the builds.
* https://github.com/OXI-Instruments/OXI-Desktop/releases


## License

OXI-Desktop is published under the [GNU GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) license.
