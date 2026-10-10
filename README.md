# MadShade

> [!IMPORTANT]
> ### MadShade IS NOT MADE FOR A NORMAL ROBLOX CLIENT
> MadShade is built for Pekora clients. They use a different folder layout from the standard Roblox client. Recent Pekora installs keep the client under:
> ```python
> %LOCALAPPDATA%/Pekora/clients/
> ├── 2021/
> │   ├── RobloxPlayerBeta.exe
> │   ├── AppSettings.xml
> │   ├── ClientSettings/
> │   └── ...
> └── ...
> ```
>
> Older ProjectX, Marine or Zekoro-style layouts are still supported for compatibility, but the installer now targets the Pekora client path by default.
>
>im not sure, but it might also work in other rivals like bubbablox or madoka, check for urself
>
> ReShade reads a small `Reshade.ini` bootstrap beside its injected DLL. That
> bootstrap's supported `[INSTALL] BasePath` setting redirects ReShade's active
> configuration and saves out of the Pekora client directory:
> ```python
> %LOCALAPPDATA%/MadShade/
> ├── config/
> │   └── Reshade.ini
> ├── presets/
> │   ├── RoShade High.ini
> │   └── ...
> └── ...
> ```
> The client-local bootstrap points `BasePath` to `config/`; the active config's
> `PresetPath` points to the selected preset in `presets/`. ReShade therefore
> saves config and preset changes in MadShade, not under `Pekora/clients/<version>`.

## Key features:
- [WIP] Automatically installs Reshade's DLL in Pekora clients.
- Allows you to select essential Reshade keybinds during installation.
- Uses a custom version of [Reshade v5.2.2](https://github.com/Not-Smelly-Garbage/Reshade-Unlocked/releases) to bypass network restrictions set by Reshade.
- Provides a description of system requirements for each component.
- Automatically installs the required shaders from GitHub.

## TODOs:
- [x] Keep ReShade's active config and presets outside the client folder using `INSTALL\BasePath`
- [x] Make support for all roblox clients
- [ ] Rebrand installer, change logo and icon stuff.
- [ ] Make MadShade keep the uninstaller when doing a non-clean uninstall.
- [ ] Make use of the fonts in the MadShade folder (edit Reshade.ini)

## Getting Started
To get started with MadShade, run `MadShadeSetup.exe` and follow the prompts. Once installation is complete, launch Pekora and use the included Reshade presets and shaders.

If you have any questions or issues with MadShade, open an issue in the repository you obtained it from.

## Building the source
The installer is written in [NSIS](https://nsis.sourceforge.io/Download "Download NSIS"), a popular open-source tool for creating Windows installers. To build the source, you'll need to have NSIS installed on your machine. I highly recommend reading through the [NSIS reference](https://nsis.sourceforge.io/Docs/Contents.html) before proceeding. In folder already has all the build plugins

### Libraries
Some dependencies may be installed to the NSIS directory, or to the repository's *Setup\Util* folder. For more information on installing plugins to the NSIS directory, click [here](https://nsis.sourceforge.io/How_can_I_install_a_plugin).
##### NSIS Plugins Directory:
- [LogEx](https://nsis.sourceforge.io/LogEx_plug-in)
- [NScurl](https://github.com/negrutiu/nsis-nscurl)
- [Nsisunz](https://github.com/past-due/nsisunz)
- [NsProcess](https://nsis.sourceforge.io/mediawiki/index.php?title=NsProcess_plugin&oldid=24277)
- [TitlebarProgress](https://nsis.sourceforge.io/TitlebarProgress_plug-in)
- [TaskbarProgress](https://nsis.sourceforge.io/TaskbarProgress_plug-in)
- [nsJSON](https://nsis.sourceforge.io/NsJSON_plug-in)
- [AccessControl](https://nsis.sourceforge.io/AccessControl_plug-in)
##### Setup->Util folder:
- [MoveFileFolder](https://nsis.sourceforge.io/MoveFileFolder)
- [GetSectionNames](https://nsis.sourceforge.io/Get_all_section_names_of_INI_file)
- [Explode](https://nsis.sourceforge.io/Explode)
- [StrContains](https://nsis.sourceforge.io/StrContains)
- [ConfigRead](https://nsis.sourceforge.io/ConfigRead)

### Command Line
You can build using the command line:
```
> makensis Setup.nsi
```
For production, it is recommended to use **lzma compression**, as it offers a higher compression ratio:
```
> makensis /X"SetCompressor /FINAL lzma" Setup.nsi
```
"# MadShade" 
"# MadShade" 
"# MadShade" 
"# MadShade" 
