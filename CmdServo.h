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
*  @file    CmdServo.h
*
*  @brief   Implements a command line interface for the servo class.
* 
****************************************************************************/

#if !defined(CMDSERVO_H)
#define CMDSERVO_H

#include "Cmd.h"

extern const Cmd::Entry gCmdServoEntry[];

void CmdServo(int argc, char **argv);

#endif // CMDSERVO_H

