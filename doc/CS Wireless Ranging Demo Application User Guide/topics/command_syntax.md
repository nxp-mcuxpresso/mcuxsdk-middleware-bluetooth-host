# Command syntax

The command line interface \(CLI\) is text-based, that is, only printable characters are used. The user can interact with the board using some commands such as a regular command prompt. This process requires a serial terminal application such as Tera Term/Putty. Line processes commands, that is, all information needed to process a command is contained in one line. Lines are terminated with the "line-feed" character. A command consists of one or more space separated words where each word, except the first, can be a command, submenu, or value \(of any type; integer/hex/character/string\). The first word cannot be a value. All commands are constructed with menus, which consist of commands and/or submenus. Commands can have zero or more value arguments.


```{include} ../topics/implicit_commands.md
:heading-offset: 2
```

```{include} ../topics/command_response.md
:heading-offset: 2
```

```{include} ../topics/boot_message.md
:heading-offset: 2
```

**Parent topic:**[Embedded firmware command reference](../topics/embedded_firmware_command_reference.md)

