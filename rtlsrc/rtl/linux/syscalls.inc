{
    $Id: syscalls.inc,v 1.1.2.6 2003/06/16 08:47:47 pierre Exp $
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by Michael Van Canneyt,
    member of the Free Pascal development team.

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}

{No debugging for syslinux include !}
{$IFDEF SYS_LINUX}
  {$UNDEF SYSCALL_DEBUG}
{$ENDIF SYS_LINUX}


{*****************************************************************************
                     --- Main:The System Call Self ---
*****************************************************************************}

{$ifdef FPC_PROFILE}
  {$define PROFILE_WAS_ACTIVE}
  {$profile off}
{$else}
  {$undef PROFILE_WAS_ACTIVE}
{$endif}


Procedure Do_SysCall( callnr:longint;var regs : SysCallregs );assembler;
{
  This function puts the registers in place, does the call, and then
  copies back the registers as they are after the SysCall.
}
{$ifdef i386}
{$ASMMODE ATT}
asm
{ load the registers... }
  movl 12(%ebp),%eax
  movl 4(%eax),%ebx
  movl 8(%eax),%ecx
  movl 12(%eax),%edx
  movl 16(%eax),%esi
  movl 20(%eax),%edi
{ set the call number }
  movl 8(%ebp),%eax
{ Go ! }
  int $0x80
{ Put back the registers... }
  pushl %eax
  movl 12(%ebp),%eax
  movl %edi,20(%eax)
  movl %esi,16(%eax)
  movl %edx,12(%eax)
  movl %ecx,8(%eax)
  movl %ebx,4(%eax)
  popl %ebx
  movl %ebx,(%eax)
end;
{$ASMMODE DEFAULT}
{$else}
{$ifdef m68k}
asm
{ load the registers... }
  move.l 12(a6),a0
  move.l 4(a0),d1
  move.l 8(a0),d2
  move.l 12(a0),d3
  move.l 16(a0),d4
  move.l 20(a0),d5
{ set the call number }
  move.l 8(a6),d0
{ Go ! }
  trap #0
{ Put back the registers... }
  move.l d0,-(sp)
  move.l 12(a6),a0
  move.l d5,20(a0)
  move.l d4,16(a0)
  move.l d3,12(a0)
  move.l d2,8(a0)
  move.l d1,4(a0)
  move.l (sp)+,d1
  move.l d1,(a0)
end;
{$else}
{$error Cannot decide which processor you have ! define i386 or m68k }
{$endif}
{$endif}

{$IFDEF SYSCALL_DEBUG}
Const
  DoSysCallDebug : Boolean = False;

var
  LastCnt,
  LastEax,
  LastCall : longint;
  DebugTxt : string[20];
{$ENDIF}
Function SysCall( callnr:longint;var regs : SysCallregs ):longint;
{
  This function serves as an interface to do_SysCall.
  If the SysCall returned a negative number, it returns -1, and puts the
  SysCall result in errno. Otherwise, it returns the SysCall return value
}
begin
  do_SysCall(callnr,regs);
  if (regs.reg1<0) and (regs.reg1>=-Sys_ERROR_MAX) then
   begin
{$IFDEF SYSCALL_DEBUG}
     If DoSysCallDebug then
       debugtxt:=' syscall error: ';
{$endif}
     ErrNo:=-regs.reg1;
     SysCall:=-1;
   end
  else
   begin
{$IFDEF SYSCALL_DEBUG}
  if DoSysCallDebug then
       debugtxt:=' syscall returned: ';
{$endif}
     SysCall:=regs.reg1;
     errno:=0
   end;
{$IFDEF SYSCALL_DEBUG}
  if DoSysCallDebug then
    begin
    inc(lastcnt);
    if (callnr<>lastcall) or (regs.reg1<>lasteax) then
      begin
      if lastcnt>1 then
        writeln(sys_nr_txt[lastcall],debugtxt,lasteax,' (',lastcnt,'x)');
      lastcall:=callnr;
      lasteax:=regs.reg1;
      lastcnt:=0;
      writeln(sys_nr_txt[lastcall],debugtxt,lasteax);
      end;
    end;
{$endif}
end;

{$ifdef PROFILE_WAS_ACTIVE}
  {$profile on}
  {$undef PROFILE_WAS_ACTIVE}
{$endif}


Function Sys_Time:longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=0;
  Sys_Time:=SysCall(SysCall_nr_time,regs);
end;


{*****************************************************************************
               --- File:File handling related calls ---
*****************************************************************************}


Function Sys_Open(f:pchar;flags:longint;mode:integer):longint;
var
  regs : SysCallregs;
Begin
  regs.reg2:=longint(f);
  regs.reg3:=flags;
  regs.reg4:=mode;
  Sys_Open:=SysCall(SysCall_nr_open,regs);
End;



Function Sys_Close(f:longint):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=f;
  Sys_Close:=SysCall(SysCall_nr_close,regs);
end;



Function Sys_Lseek(F:longint;Off:longint;Whence:longint):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=f;
  regs.reg3:=off;
  regs.reg4:=Whence;
  Sys_lseek:=SysCall(SysCall_nr_lseek,regs);
end;



Function Sys_Read(f:longint;buffer:pchar;count:longint):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=f;
  regs.reg3:=longint(buffer);
  regs.reg4:=count;
  Sys_Read:=SysCall(SysCall_nr_read,regs);
end;



Function Sys_Write(f:longint;buffer:pchar;count:longint):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=f;
  regs.reg3:=longint(buffer);
  regs.reg4:=count;
  Sys_Write:=SysCall(SysCall_nr_write,regs);
end;



Function Sys_Unlink(Filename:pchar):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(filename);
  Sys_Unlink:=SysCall(SysCall_nr_unlink,regs);
end;


Function Sys_fstat(fd : longint;var Info:stat):Longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=fd;
  regs.reg3:=longint(@Info);
  Sys_fStat:=SysCall(SysCall_nr_fstat,regs);
end;


Function Sys_Rename(Oldname,Newname:pchar):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(oldname);
  regs.reg3:=longint(newname);
  Sys_Rename:=SysCall(SysCall_nr_rename,regs);
end;



Function Sys_Stat(Filename:pchar;var Buffer: stat):longint;
{
   We need this for getcwd
}
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(filename);
  regs.reg3:=longint(@buffer);
  Sys_Stat:=SysCall(SysCall_nr_stat,regs);
end;


Function Sys_Symlink(oldname,newname:pchar):longint;
{
  We need this for erase
}
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(oldname);
  regs.reg3:=longint(newname);
  Sys_symlink:=SysCall(SysCall_nr_symlink,regs);
end;


Function Sys_ReadLink(name,linkname:pchar;maxlen:longint):longint;
var
  regs : SysCallRegs;
begin
  regs.reg2:=longint(name);
  regs.reg3:=longint(linkname);
  regs.reg4:=maxlen;
  Sys_ReadLink:=SysCall(Syscall_nr_readlink,regs);
end;


{*****************************************************************************
               --- Directory:Directory related calls ---
*****************************************************************************}


Function Sys_Chdir(Filename:pchar):longint;
var
  regs : SysCallregs;

begin
  regs.reg2:=longint(filename);
  Sys_ChDir:=SysCall(SysCall_nr_chdir,regs);
end;



Function Sys_Mkdir(Filename:pchar;mode:longint):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(filename);
  regs.reg3:=mode;
  Sys_MkDir:=SysCall(SysCall_nr_mkdir,regs);
end;



Function Sys_Rmdir(Filename:pchar):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(filename);
  Sys_Rmdir:=SysCall(SysCall_nr_rmdir,regs);
end;



{ we need this for getcwd }
Function OpenDir(f:pchar):pdir;
var
  fd:integer;
  st:stat;
  ptr:pdir;
begin
  opendir:=nil;
  if sys_stat(f,st)<0 then
   exit;
{ Is it a dir ? }
  if not((st.mode and $f000)=$4000)then
   begin
     errno:=sys_enotdir;
     exit
   end;
{ Open it}
  fd:=sys_open(f,OPEN_RDONLY,438);
  if fd<0 then
   exit;
  new(ptr);
  if ptr=nil then
   exit;
  new(ptr^.buf);
  if ptr^.buf=nil then
   exit;
  ptr^.fd:=fd;
  ptr^.loc:=0;
  ptr^.size:=0;
  ptr^.dd_max:=sizeof(ptr^.buf^);
  opendir:=ptr;
end;



function CloseDir(p:pdir):integer;
begin
  closedir:=sys_close(p^.fd);
  dispose(p^.buf);
  dispose(p);
end;



Function Sys_ReadDir(p:pdir):pdirent;
var
  regs :SysCallregs;
  dummy:longint;
begin
  regs.reg3:=longint(p^.buf);
  regs.reg2:=p^.fd;
  regs.reg4:=1;
  dummy:=SysCall(SysCall_nr_readdir,regs);
{ the readdir system call returns the number of bytes written }
  if dummy=0 then
   sys_readdir:=nil
  else
   sys_readdir:=p^.buf
end;


{*****************************************************************************
        --- Process:Process & program handling - related calls ---
*****************************************************************************}

Function Sys_GetPid:LongInt;
var
  regs : SysCallregs;
begin
  Sys_GetPid:=SysCall(SysCall_nr_getpid,regs);
end;


Procedure Sys_Exit(ExitCode:Integer);
var
  regs : SysCallregs;
begin
  regs.reg2:=exitcode;
  SysCall(SysCall_nr_exit,regs)
end;

Procedure SigAction(Signum:longint;Act,OldAct:PSigActionRec );
{
  Change action of process upon receipt of a signal.
  Signum specifies the signal (all except SigKill and SigStop).
  If Act is non-nil, it is used to specify the new action.
  If OldAct is non-nil the previous action is saved there.
}
Var
  sr : Syscallregs;
begin
  sr.reg2:=Signum;
  sr.reg3:=Longint(act);
  sr.reg4:=Longint(oldact);
  SysCall(Syscall_nr_sigaction,sr);
end;

function Sys_FTruncate(Handle,Pos:longint):longint;  //moved from sysunix.inc Do_Truncate
var
  sr : syscallregs;
begin
  sr.reg2:=Handle;
  sr.reg3:=Pos;
  Sys_FTruncate:=syscall(syscall_nr_ftruncate,sr);
end;

Function Sys_mmap(adr,len,prot,flags,fdes,off:longint):longint; // moved from sysunix.inc, used in sbrk
type
  tmmapargs=packed record
    address : longint;
    size    : longint;
    prot    : longint;
    flags   : longint;
    fd      : longint;
    offset  : longint;
  end;
var
  t     : syscallregs;
  mmapargs : tmmapargs;
begin
  mmapargs.address:=adr;
  mmapargs.size:=len;
  mmapargs.prot:=prot;
  mmapargs.flags:=flags;
  mmapargs.fd:=fdes;
  mmapargs.offset:=off;
  t.reg2:=longint(@mmapargs);
  do_syscall(syscall_nr_mmap,t);
  Sys_mmap:=t.reg1;
  if t.reg1=-1 then
    errno:=-1;
end;

{
  Interface to Unix ioctl call.
  Performs various operations on the filedescriptor Handle.
  Ndx describes the operation to perform.
  Data points to data needed for the Ndx function. The structure of this
  data is function-dependent.
}
Function Sys_IOCtl(Handle,Ndx: Longint;Data: Pointer):LongInt;  // This was missing here, instead hardcode in Do_IsDevice
var
  sr: SysCallRegs;
begin
  sr.reg2:=Handle;
  sr.reg3:=Ndx;
  sr.reg4:=Longint(Data);
  Sys_IOCtl:=SysCall(Syscall_nr_ioctl,sr);
end;


Function Sys_SigAltStack(ss, oss :psigaltstack):longint;
var
  regs : SysCallregs;
begin
  regs.reg2:=longint(ss);
  regs.reg3:=longint(oss);
  sys_sigaltstack:=SysCall(syscall_nr_sigaltstack,regs);
end;


{
  $Log: syscalls.inc,v $
  Revision 1.1.2.6  2003/06/16 08:47:47  pierre
   * fix problem with mmap

  Revision 1.1.2.5  2003/03/13 23:01:56  pierre
   + Use SigAltStack

  Revision 1.1.2.4  2002/09/09 06:29:32  pierre
   * disable profiling for syscall function to get infos on time used by each function

  Revision 1.1.2.3  2001/07/24 00:40:40  pierre
   * fix sys_mmap for m68k cpu

  Revision 1.1.2.2  2001/03/12 15:10:39  marco
   * [Solaris] patches. Some functions,const and types moved out sysunix.inc

  Revision 1.1.2.1  2000/09/10 16:12:40  marco
  The rearrangement to linux for

  Revision 1.1  2000/07/13 06:30:54  michael
  + Initial import

  Revision 1.11  2000/07/08 18:02:39  peter
    * do_open checks for directory, if directory then ioerror 2

  Revision 1.10  2000/02/09 16:59:32  peter
    * truncated log

  Revision 1.9  2000/02/08 11:47:09  peter
    * paramstr(0) support

  Revision 1.8  2000/01/07 16:41:41  daniel
    * copyright 2000

  Revision 1.7  2000/01/07 16:32:28  daniel
    * copyright 2000 added

  Revision 1.6  1999/07/28 17:37:06  michael
  * forgot ;

  Revision 1.5  1999/07/28 12:15:16  michael
  * Memory leak fixed in CloseDir, by Sebastian Guenther

  Revision 1.4  1999/07/28 12:14:37  michael
  * Memory leak fixed in CloseDir, by Sebastian Guenther

}
