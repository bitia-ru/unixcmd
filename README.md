Cross-platform double-panel file manager
===================

Runs on Windows, Linux, and macOS.

Inspired by Total Commander.

Currently is in development — not recommended for use before the first release.

![UnixCMD screenshot](unixcmd.png)

## Command Line Access (macOS)

After installing UnixCMD to `/Applications`, you can set up command line access to launch it 
quickly from Terminal using the `uc` command.

### Option 1: Create Symbolic Link (Recommended)

Create a symbolic link in `/usr/local/bin` (requires administrator privileges):

```bash
sudo ln -s /Applications/unixcmd.app/Contents/MacOS/unixcmd /usr/local/bin/uc
```

### Option 2: Shell Alias (User-specific)

Add an alias to your shell profile. For zsh (default on macOS):

```bash
echo 'alias uc="/Applications/unixcmd.app/Contents/MacOS/unixcmd"' >> ~/.zshrc
source ~/.zshrc
```

For bash:

```bash
echo 'alias uc="/Applications/unixcmd.app/Contents/MacOS/unixcmd"' >> ~/.bash_profile
source ~/.bash_profile
```

**Note:** When launched from command line, UnixCMD will start in the current working directory. 
When launched from GUI (double-clicking the app), it will start in your home directory.
