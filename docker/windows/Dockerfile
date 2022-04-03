FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Install Chocolatey
RUN powershell -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command \
  "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" \
  && setx PATH "%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

# Install Chocolatey Packages
RUN setx PYTHONIOENCODING "UTF-8"
RUN choco install -y git --params "/GitAndUnixToolsOnPath /NoAutoCrlf" \
  && choco install -y python3 --version 3.10.0

RUN cd Python310 && python -m pip install aqtinstall \
  && aqt install-qt --outputdir c:/Qt windows desktop 6.2.4 win64_mingw \
  && aqt install-tool --outputdir c:/Qt windows desktop tools_cmake \
  && aqt install-tool --outputdir c:/Qt windows desktop tools_ninja \
  && aqt install-tool --outputdir c:/Qt windows desktop tools_mingw90 \
  && aqt install-tool --outputdir c:/Qt windows desktop tools_qtcreator \
  && setx PATH "C:\Qt\Tools\Ninja;%PATH%"
  # Somehow cmake is not able to detect ninja
