FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Install Chocolatey
RUN powershell -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command \
  "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" \
  && setx PATH "%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

# Install Chocolatey Packages
RUN setx PYTHONIOENCODING "UTF-8"
RUN choco install -y git --params "/GitAndUnixToolsOnPath /NoAutoCrlf" \
  && choco install -y python3 --version 3.10.0

ARG QT=5.15.2
ARG QT_HOST=windows
ARG QT_TARGET=desktop
ARG QT_ARCH=win64_mingw81

RUN cd Python310 && python -m pip install aqtinstall \
  && aqt list-qt %QT_HOST% %QT_TARGET% \
  && aqt list-qt windows desktop --arch %QT% \
  && aqt install-qt --outputdir c:/Qt %QT_HOST% %QT_TARGET% %QT% %QT_ARCH% \
  && aqt install-tool --outputdir c:/Qt %QT_HOST% %QT_TARGET% tools_cmake \
  && aqt install-tool --outputdir c:/Qt %QT_HOST% %QT_TARGET% tools_ninja \
  && aqt install-tool --outputdir c:/Qt %QT_HOST% %QT_TARGET% tools_mingw qt.tools.win64_mingw810 \
  && aqt install-tool --outputdir c:/Qt %QT_HOST% %QT_TARGET% tools_qtcreator \
  && setx PATH "C:\Qt\Tools\mingw810_64\bin;C:\Qt\Tools\Ninja;%PATH%"
  # Somehow cmake is not able to detect ninja
