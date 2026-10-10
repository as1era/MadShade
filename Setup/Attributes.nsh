# Important
InstallDir "$LOCALAPPDATA\MadShade"

# Attributes
!define VERSION "0.0.2"
!define MANUFACTURER "bulieme"
!define NAME "MadShade"
!define ROBLOXREGLOC "SOFTWARE\ProjectX Corporation\ProjectX"
!define SELFREGLOC "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
!define ROBLOXCLIENT "RobloxPlayerBeta.exe"
!define LEGACYROBLOXCLIENT "ProjectXPlayerBeta.exe"
!define UninstallerExe "Uninstall ${NAME}.exe"
!define HELPLINK "https://google.com/" # supposed to be a discord server
!define ABOUTLINK "https://google.com/"
!define RENDERAPI "dxgi.dll"

# Directories
!define MADSHADEBASE "$LOCALAPPDATA\MadShade"
!define MADSHADECFG "${MADSHADEBASE}\config"
!define PRESETFOLDER "${MADSHADEBASE}\presets"
!define RESHADESOURCE "..\Files\Reshade"
!define PRESETSOURCE "..\Files\Preset"
!define PRESETTEMPFOLDER "$TEMP\Presets"
!define TEMPFOLDER "$TEMP\Peak"
!define LOGDIRECTORY "$TEMP\madshade"
!define MADSHADESOURCE "..\Files\MadShade"

# Files
;!define SPLASHICON "$PLUGINSDIR\SplashScreen.gif"
!define SHADERSINI "$PLUGINSDIR\Shaders.ini"
!define RESHADEINI "$PLUGINSDIR\Reshade.ini"
!define RESHADECONF "${MADSHADECFG}\Reshade.ini"
!define APPICON "$INSTDIR\AppIcon.ico"

Var Techniques
Var Repositories
Var ShaderDir
Var RobloxPath
Var PresetPriority # Determine which preset should be loaded. Lower = higher priority.

VIProductVersion "${VERSION}.0"
VIAddVersionKey "ProductName" "${NAME}"
VIAddVersionKey "CompanyName" "${MANUFACTURER}"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2025 @bulieme"
VIAddVersionKey "ProductVersion" "${VERSION}"
VIAddVersionKey "FileVersion" "${VERSION}"

Name "${NAME}"
Caption "$(^Name) Installation"
Outfile "..\${NAME}Setup.exe"
BrandingText "${MANUFACTURER}"
CRCCHECK force