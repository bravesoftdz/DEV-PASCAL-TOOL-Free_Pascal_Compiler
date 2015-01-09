{
    $Id: keyboard.pp,v 1.2.2.3 2002/03/03 21:10:18 hajny Exp $
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by Florian Klaempfl
    member of the Free Pascal development team

    Keyboard unit for OS/2

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}
unit Keyboard;
interface

{$i keybrdh.inc}

implementation

uses
 KbdCalls, DosCalls;

{$i keyboard.inc}

const
 DefaultKeyboard = 0;
 Handle: word = DefaultKeyboard;

procedure SysInitKeyboard;
var
 K: TKbdInfo;
begin
 if KbdGetFocus (IO_Wait, DefaultKeyboard) = No_Error then
 begin
  if KbdOpen (Handle) <> No_Error then Handle := DefaultKeyboard;
  KbdFlushBuffer (Handle);
  KbdFreeFocus (DefaultKeyboard);
  KbdGetFocus (IO_Wait, Handle);
  K.cb := 10;
  KbdGetStatus (K, Handle);
  K.fsMask := $14;
  KbdSetStatus (K, Handle);
 end;
end;

procedure SysDoneKeyboard;
begin
 KbdFreeFocus (Handle);
 if KbdGetFocus (IO_Wait, DefaultKeyboard) = No_Error then
 begin
  KbdClose (Handle);
  Handle := DefaultKeyboard;
  KbdFreeFocus (DefaultKeyboard);
 end;
end;

function SysGetKeyEvent: TKeyEvent;
var
 K: TKbdKeyInfo;
begin
  KbdGetFocus (IO_Wait, Handle);
  while (KbdCharIn (K, IO_Wait, Handle) <> No_Error)
        or (K.fbStatus and $40 = 0) do 
    DosSleep (5);
  with K do
    begin
    if (byte (chChar) = $E0) and (fbStatus and 2 <> 0) then chChar := #0;
    SysGetKeyEvent := cardinal ($0300 or fsState and $F) shl 16 or
                      cardinal (byte (chScan)) shl 8 or byte (chChar);
    end;
end;

function SysPollKeyEvent: TKeyEvent;
var
 K: TKbdKeyInfo;
 Key : TKeyEvent;
 
begin
  Key:=0;
  KbdGetFocus (IO_NoWait, Handle);
  if (KbdPeek (K, Handle) <> No_Error) or
     (K.fbStatus and $40 = 0) then 
    FillChar (K, SizeOf (K), 0) 
  else
    with K do
      begin
      if (byte (chChar) = $E0) and (fbStatus and 2 <> 0) then 
        chChar := #0;
      Key:= cardinal ($0300 or fsState and $F) shl 16 or
            cardinal (byte (chScan)) shl 8 or byte (chChar);
      end;
  if (Key and $FFFF)=0 then 
   Key := 0;
  SysPollKeyEvent:=Key;
end;

function SysGetShiftState: Byte;

var
 K: TKbdInfo;
begin
 KbdGetFocus (IO_NoWait, Handle);
 KbdGetStatus (K, Handle);
 SysGetShiftState:=(K.fsState and $F);
end;

Const
  SysKeyboardDriver : TKeyboardDriver = (
    InitDriver : @SysInitKeyBoard;
    DoneDriver : @SysDoneKeyBoard;
    GetKeyevent : @SysGetKeyEvent;
    PollKeyEvent : @SysPollKeyEvent;
    GetShiftState : @SysGetShiftState;
    TranslateKeyEvent : Nil;
    TranslateKeyEventUnicode : Nil; 
  );


begin 
  SetKeyBoardDriver(SysKeyBoardDriver);
end.
{
  $Log: keyboard.pp,v $
  Revision 1.2.2.3  2002/03/03 21:10:18  hajny
    * SysPollKeyEvent fixed

  Revision 1.2.2.2  2001/09/21 21:20:43  michael
  + Added support for keyboard driver.
  + Added DefaultTranslateKeyEvent,DefaultTranslateKeyEventUnicode
  + PendingKeyEvent variable no longer public. Handling of this variable is
    now done entirely by global functions. System dependent code should not
    need it, it is set automatically.
  + InitVideo DoneVideo will check whether the keyboard is initialized or not.

  Revision 1.2.2.1  2001/01/30 21:52:02  peter
    * moved api utils to rtl

  Revision 1.2  2001/01/13 12:01:07  hajny
    * ErrorHandler correction

  Revision 1.1  2001/01/13 11:03:58  peter
    * API 2 RTL commit

}
