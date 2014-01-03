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
*  @file    CmdServo.cpp
*
*  @brief   Implements a command line interface for the servo class.
*
****************************************************************************/

#include "CmdServo.h"
#include "Servo.h"

// In order to minimize memory requirements, we allocate the CmdServo objects
// dynamically, and only when required.

struct ServoObj
{
  uint8_t _pin;
  Servo _servo;
  ServoObj *_next;
};

static ServoObj *gServoObj; // list of servo objects

static uint8_t gPin;          // pin number for the current command
static ServoObj *gCurrServo;  // servo object for the current command

static void ServoAttach(int argc, char **argv)
{
  Cmd::Printf("ServoAttach\n");
  if (argc != 1 && argc != 3) {
    Cmd::Printf("Unexpected number of argumnents. Expecting 0 or 2, found %d", argc - 1);
    return;
  }
  if (!gCurrServo) {
    gCurrServo = new ServoObj;
    if (!gCurrServo) {
      Cmd::Printf("Unable to allocate a ServoObj object\n");
      return;
    }
    gCurrServo->_pin = gPin; 
    gCurrServo->_next = gServoObj;
    gServoObj = gCurrServo;
  }
  uint8_t servoIndex;
  if (argc == 1) {
    servoIndex = gCurrServo->_servo.attach(gCurrServo->_pin);
  }
  else {
    int minVal;
    int maxVal;
    if (!Cmd::ParseInt("min", argv[1], &minVal)
    ||  !Cmd::ParseInt("max", argv[2], &maxVal)) {
      return;
    }
    servoIndex = gCurrServo->_servo.attach(gCurrServo->_pin, minVal, maxVal);
  }
  Cmd::Printf("servoIndex = %d\n", servoIndex);
}

static void ServoDetach(int argc, char **argv)
{
  if (!gCurrServo) {
    Cmd::Printf("Servo %d not currently attached\n", gPin);
    return;
  }
  gCurrServo->_servo.detach();

  ServoObj *prevServo = nullptr;
  ServoObj *servo = gServoObj;
  while (servo && servo != gCurrServo) {
    prevServo = servo;
    servo = servo->_next;
  }
  if (servo) {
    if (prevServo) {
      prevServo->_next = servo->_next;
    }
    else {
      gServoObj = servo->_next;
    }
  }
  delete gCurrServo;
  gCurrServo = nullptr;
}

static void ServoRead(int argc, char **argv)
{
  if (!gCurrServo) {
    Cmd::Printf("Servo %d not currently attached\n", gPin);
    return;
  }
  int angle = gCurrServo->_servo.read();
  int usec = gCurrServo->_servo.readMicroseconds();
  Cmd::Printf("Angle: %d Microseconds: %d\n", angle, usec);
}

static void ServoWrite(int argc, char **argv)
{
  if (!gCurrServo) {
    Cmd::Printf("Servo %d not currently attached\n", gPin);
    return;
  }
  if (argc != 2) {
    Cmd::Printf("Expecting 1 argumnent. Found %d", argc - 1);
    return;
  }
  int val;
  if (!Cmd::ParseInt("val", argv[1], &val)) {
    return;
  }
  gCurrServo->_servo.write(val);
}

static ServoObj *FindServo(uint8_t pin)
{
  ServoObj *servo = gServoObj;
  while (servo && servo->_pin != pin) {
    servo = servo->_next;
  }
  return servo;
}

const Cmd::Entry gCmdServoEntry[] =
{
  { ServoAttach,   "attach",   "[min max]", "Attach the given pin to a servo" },
  { ServoDetach,   "detach",   "",          "Detaches the pin from a servo" },
  { ServoRead,     "read",     "",          "Reads the pulse width (as angle and microseconds)" },
  { ServoWrite,    "write",    "val",       "if val < 200 sets the angle, otherwise the pulse width in microseconds" },
  { nullptr }
};

void CmdServo(int argc, char **argv)
{
  if (argc < 3) {
    Cmd::Printf("Servo command needs at least 3 arguments\n");
    return;
  }
  if (!Cmd::ParseInt("pin", argv[1], &gPin)) {
    return;
  }
  gCurrServo = FindServo(gPin);
  Cmd::ProcessCommand(gCmdServoEntry, argc - 2, &argv[2]);
}
