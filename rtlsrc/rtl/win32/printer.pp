{
    $Id: printer.pp,v 1.1 2000/07/13 06:31:22 michael Exp $
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by Florian Klaempfl
    member of the Free Pascal development team

    Printer unit for BP7 compatible RTL

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}
unit printer;
interface

var
  lst : text;

implementation

var
  old_exit : pointer;

procedure printer_exit;
begin
  close(lst);
  exitproc:=old_exit;
end;


begin
  assign(lst,'PRN');
  rewrite(lst);
  old_exit:=exitproc;
  exitproc:=@printer_exit;
end.
{
  $Log: printer.pp,v $
  Revision 1.1  2000/07/13 06:31:22  michael
  + Initial import

  Revision 1.1  2000/04/06 08:27:33  florian
    + Initial revision of printer unit for win32, took that one of go32v2

}