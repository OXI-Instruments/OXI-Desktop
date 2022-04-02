FROM mcr.microsoft.com/windows/servercore:ltsc2022-amd64

# Install Chocolatey
RUN powershell -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command \
  "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" \
  && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

# Install Chocolatey Packages
ENV PYTHONIOENCODING "UTF-8"
RUN choco install -y choco install -y git --params "/GitAndUnixToolsOnPath /NoAutoCrlf" \
  && choco install -y python3 --version 3.10.0 \
  && choco install -y pip --version 1.2.0 \
  && choco install -y cmake --version 3.21.3 --installargs "ADD_CMAKE_TO_PATH=System"
