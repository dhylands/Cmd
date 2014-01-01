/****************************************************************************
*
*   Copyright (c) 2013 Dave Hylands     <dhylands@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the MIT License.
*
*   See LICENSE for more details.
*
****************************************************************************/
/**
*
*  @file    Cmd.h
*
*  @brief   Implementation of a simple comman processor.
* 
*  This command processor was inspired by the python Cmd module.
*
****************************************************************************/

#if !defined(CMD_H)
#define CMD_H

#include <stdarg.h>

#if !defined(LINE_LEN)
#define LINE_LEN  128
#endif

#if !defined(MAX_ARGS)
#define MAX_ARGS  8
#endif

class Cmd
{
public:
  typedef void (*Func)(int argc, char **argv);

  struct Entry
  {
    Func _func;         // Function executed
    const char *_name;  // Command name
    const char *_args;  // Describes arguments (used to dsplay help)
    const char *_descr; // Describes what the command does
  };

  Cmd(const Entry *entry);

  void Run();

  static void Printf(const char *fmt, ...);
  static void vPrintf(const char *fmt, va_list args);
  static int PrintFunc(void *param, int ch);
  static void Args(int argc, char **argv);
  static void Help(int argc, char **argv);

protected:
  virtual void PrintChar(char ch);
  virtual void Prompt();
  virtual bool IsCharAvailable();
  virtual char GetChar();

private:
  void Parse();
  const Entry *FindCommand(const char *cmd);

  static Cmd *sCmd;
  const Entry *_entry;

  const char *_promptPrefix;
  const char *_prompt;
  const char *_promptSuffix;

  char *_linePtr;
  char _line[LINE_LEN];
  char *_argv[MAX_ARGS];
  bool _isPromptDisplayed;
};


#endif // CMD_H
