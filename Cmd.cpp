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
*  @file    Cmd.cpp
*
*  @brief   Implementation of a simple comman processor.
* 
*  This command processor was inspired by the python Cmd module.
*
****************************************************************************/

#include "Cmd.h"
#include "StrPrintf.h"
#include "Logging.h"
#include "Arduino.h"

#define GREEN "\x1b[1;32m"
#define NO_COLOR "\x1b[0m"
#define DEFAULT_PROMPT  "Cmd> "

Cmd *Cmd::sCmd = nullptr;

Cmd::Cmd(const Cmd::Entry *entry)
  : _entry(entry),
    _promptPrefix(GREEN),
    _prompt(DEFAULT_PROMPT),
    _promptSuffix(NO_COLOR),
    _linePtr(&_line[0]),
    _isPromptDisplayed(false)
{
  sCmd = this;
}

//virtual
void Cmd::PrintChar(char ch)
{
  Serial.print(ch);
  Serial.flush();
}

//static
int Cmd::PrintFunc(void *param, int ch)
{
  if (ch == '\n') {
    sCmd->PrintChar('\r');
  }
  sCmd->PrintChar(ch); 
  return 1;
}

//static
void Cmd::Printf(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  sCmd->vPrintf(fmt, args);
  va_end(args);
}

//static
void Cmd::vPrintf(const char *fmt, va_list args)
{
  vStrXPrintf(PrintFunc, nullptr, fmt, args);
}

//virtual
bool Cmd::IsCharAvailable()
{
  return Serial.available();
}

//virtual
char Cmd::GetChar()
{
  return Serial.read();
}

//virtual
void Cmd::Prompt()
{
  Printf("%s%s%s", _promptPrefix, _prompt, _promptSuffix);
}

void Cmd::Run()
{
  if (!_isPromptDisplayed) {
    _isPromptDisplayed = true;
    Prompt();
  }
  if (!IsCharAvailable()) {
    return;
  }
  char ch = GetChar();

  switch (ch) {
    case '\n':
      Printf("\n");
      *_linePtr++ = '\0';
      _isPromptDisplayed = false;
      Parse();
      _linePtr = &_line[0];
      break;

    case '\b':
    case '\x7f':
      if (_linePtr > &_line[0]) {
        Printf("\b \b");
        _linePtr--;
      }
      else {
        Printf("\a");
      }
      break;

    default:
      if (_linePtr < &_line[LINE_LEN-1]) {
        *_linePtr++ = ch;
        Printf("%c", ch);
      }
      else {
        Printf("\a");
      }
      break;
  }
}

void Cmd::Parse()
{
  const char *delim = " \t\r\n";
  int argc = 0;
  char *saveptr;
  char *token = strtok_r(_line, delim, &saveptr);
  while (token != NULL && argc < MAX_ARGS - 1) {
    _argv[argc++] = token; 
    token = strtok_r(NULL, delim, &saveptr);
  }
  _argv[argc] = nullptr;

  if (argc == 0) {
    // Empty line - ignore
    return;
  }

  const Entry *entry = FindCommand(_argv[0]);
  if (entry) {
    entry->_func(argc, _argv);
  }
  else {
    Printf("Unrecognized command: '%s'\n", _argv[0]);
  }
}

const Cmd::Entry *Cmd::FindCommand(const char *cmd)
{
  const Entry *entry = _entry;
  while (entry->_func) {
    if (strcmp(entry->_name, cmd) == 0) {
      return entry;
    }
    entry++;
  }
  return nullptr;
}

void Cmd::Args(int argc, char **argv)
{
  Cmd::Printf("args: argc = %d\n", argc);
  for (int i = 0; i < argc; i++) {
    Cmd::Printf("  argv[%d] = '%s'\n", i, argv[i]);
  }
}

void Cmd::Help(int argc, char **argv)
{
  if (argc > 1) {
    // Give help for a specific command

    const Entry *entry = sCmd->FindCommand(argv[1]);
    if (entry) {
      Printf("%s %s - %s\n", entry->_name, entry->_args, entry->_descr);
    }
    else {
      Printf("Unrecognized command: '%s'\n", argv[1]);
    }
    return;
  }

  size_t wName = 0;
  size_t wArgs = 0;
  const Entry *entry = sCmd->_entry;
  while (entry->_func) {
    wName = max(wName, strlen(entry->_name));
    wArgs = max(wArgs, strlen(entry->_args));
    entry++;
  }
  entry = sCmd->_entry;
  while (entry->_func) {
    Printf("%-*s %-*s - %s\n", wName, entry->_name, 
            wArgs, entry->_args, entry->_descr);
    entry++;
  }
}
