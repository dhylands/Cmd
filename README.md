Cmd
===

Simple command line processor (for Arduino)

To use, create an array of Cmd::Event objects, terminated by an entry of all
nullptrs.
```
const Cmd::Entry gCmdEntry[] =
{
  { Cmd::Args,  "args", "",           "Displays arguments passed on command line" },
  { Cmd::Help,  "help", "[command]",  "Displays help for a given command (or all commands)" },
  { nullptr, nullptr, nullptr, nullptr }
};
```
The functions should have a function prototype which looks like:
```
void SomeFunc(int argc, char **argv)
{
}
```
argv will be a null-terminated array with argc entries. The very first entry
will be the command that was entered.

Create a Cmd object (or an object derived from Cmd):

```
Cmd gCmd(gCmdEntry);
```

For now, add a call to:
```
  gCmd.Run();
```
in your loop function. This requirement will go away once Serial events can be
used.

To override which serial port is used you can either create a variable called
Serial in your main app, ala:
```
HardwareSerial2 Serial;
```
or you can use an object derived from Cmd which overrides the IsCharAvailable,
GetChar, and PrintChar methods.
