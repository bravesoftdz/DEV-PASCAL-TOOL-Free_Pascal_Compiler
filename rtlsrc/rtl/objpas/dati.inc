{
    *********************************************************************
    $Id: dati.inc,v 1.1.2.4 2003/01/18 23:44:37 michael Exp $
    Copyright (C) 1997, 1998 Gertjan Schouten

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
    *********************************************************************

    System Utilities For Free Pascal
}

{==============================================================================}
{   internal functions                                                         }
{==============================================================================}


Function DoEncodeDate(Year, Month, Day: Word): longint;

Var 
  D : TDateTime;

begin
  If TryEncodeDate(Year,Month,Day,D) then
    Result:=Trunc(D)
  else 
    Result:=0;   
end;

function DoEncodeTime(Hour, Minute, Second, MilliSecond: word): longint;

Var
  T : TDateTime;

begin
  If TryEncodeTime(Hour,Minute,Second,MilliSecond,T) then
    Result:=trunc(T*MSecsPerDay)
  else
    Result:=0;  
end;


{==============================================================================}
{   Public functions                                                           }
{==============================================================================}

{   DateTimeToTimeStamp converts DateTime to a TTimeStamp   }

function DateTimeToTimeStamp(DateTime: TDateTime): TTimeStamp;
begin
  result.Time := Trunc(Frac(DateTime) * MSecsPerDay);
  result.Date := 1 + DateDelta + Trunc(System.Int(DateTime));
end ;

{   TimeStampToDateTime converts TimeStamp to a TDateTime value   }

function TimeStampToDateTime(const TimeStamp: TTimeStamp): TDateTime;
begin
  result := (TimeStamp.Date - DateDelta - 1) + (TimeStamp.Time / MSecsPerDay);
end ;

{   MSecsToTimeStamp   }

function MSecsToTimeStamp(MSecs: comp): TTimeStamp;
begin
  result.Date := Round(msecs / msecsperday);
  msecs:= comp(msecs-result.date*msecsperday);
  result.Time := Round(MSecs);
end ;

{   TimeStampToMSecs   }

function TimeStampToMSecs(const TimeStamp: TTimeStamp): comp;
begin
  result := TimeStamp.Time + timestamp.date*msecsperday;
end ;

{   EncodeDate packs three variables Year, Month and Day into a
    TDateTime value the result is the number of days since 12/30/1899   }
    
const
   DayTable: array[Boolean, 1..12] of longint =
      ((0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334),
       (0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335));

Function TryEncodeDate(Year,Month,Day : Word; Var Date : TDateTime) : Boolean;

var
  c, ya: cardinal;
begin
  Result:=(Year>0) and (Year<10000) and
          (Month in [1..12]) and
          (Day>0) and (Day<=MonthDays[IsleapYear(Year),Month]);
 If Result then
   begin
     if month > 2 then
      Dec(Month,3)
     else
      begin
        Inc(Month,9);
        Dec(Year);
      end;
     c:= Year DIV 100;
     ya:= Year - 100*c;
     Date := (146097*c) SHR 2 + (1461*ya) SHR 2 + (153*cardinal(Month)+2) DIV 5 + cardinal(Day) - 693900;
   end
end;

function EncodeDate(Year, Month, Day: word): TDateTime;

begin
  If Not TryEncodeDate(Year,Month,Day,Result) then
    Raise Exception.CreateFmt('%d-%d-%d is not a valid date specification',
                              [Year,Month,Day]);
end;

{   EncodeTime packs four variables Hour, Minute, Second and MilliSecond into
    a TDateTime value     }

function EncodeTime(Hour, Minute, Second, MilliSecond:word):TDateTime;

begin
  If not TryEncodeTime(Hour,Minute,Second,MilliSecond,Result) then
    Raise Exception.CreateFmt('%d:%d:%d.%d is not a valid time specification',
                              [Hour,Minute,Second,MilliSecond]);
end;

function TryEncodeTime(Hour, Min, Sec, MSec:word; Var Time : TDateTime) : boolean;

begin
  Result:=(Hour<24) and (Min<60) and (Sec<60) and (MSec<1000);
  If Result then
    Time:=(Hour*3600000+Min*60000+Sec*1000+MSec)/MSecsPerDay;
end;

{   DecodeDate unpacks the value Date into three values:
    Year, Month and Day   }

procedure DecodeDate(Date: TDateTime; var Year, Month, Day: word);
var
  j : cardinal;
begin
  j := pred((Trunc(System.Int(Date)) + 693900) SHL 2);
  Year:= j DIV 146097;
  j:= j - 146097 * cardinal(Year);
  Day := j SHR 2;
  j:=(Day SHL 2 + 3) DIV 1461;
  Day:= (cardinal(Day) SHL 2 + 7 - 1461*j) SHR 2;
  Month:=(5 * Day-3) DIV 153;
  Day:= (5 * Day +2 - 153*Month) DIV 5;
  Year:= 100 * cardinal(Year) + j;
  if Month < 10 then
   inc(Month,3)
  else
    begin
      dec(Month,9);
      inc(Year);
    end;
end;

{   DecodeTime unpacks Time into four values:
    Hour, Minute, Second and MilliSecond    }

procedure DecodeTime(Time: TDateTime; var Hour, Minute, Second, MilliSecond: word);
Var
  l : cardinal;
begin
 l := Round(Frac(time) * MSecsPerDay);
 Hour   := l div 3600000;
 l := l mod 3600000;
 Minute := l div 60000;
 l := l mod 60000;
 Second := l div 1000;
 l := l mod 1000;
 MilliSecond := l;
end;

{   DateTimeToSystemTime converts DateTime value to SystemTime   }

procedure DateTimeToSystemTime(DateTime: TDateTime; var SystemTime: TSystemTime);
begin
  DecodeDate(DateTime, SystemTime.Year, SystemTime.Month, SystemTime.Day);
  DecodeTime(DateTime, SystemTime.Hour, SystemTime.Minute, SystemTime.Second, SystemTime.MilliSecond);
end ;

{   SystemTimeToDateTime converts SystemTime to a TDateTime value   }

function SystemTimeToDateTime(const SystemTime: TSystemTime): TDateTime;
begin
  result := DoEncodeDate(SystemTime.Year, SystemTime.Month, SystemTime.Day) +
            DoEncodeTime(SystemTime.Hour, SystemTime.Minute, SystemTime.Second, SystemTime.MilliSecond) / MSecsPerDay;
end ;

{   DayOfWeek returns the Day of the week (sunday is day 1)  }

function DayOfWeek(DateTime: TDateTime): integer;
begin
  Result := 1 + (Abs(Trunc(DateTime) - 1) mod 7);
end ;

{   Date returns the current Date   }

function Date: TDateTime;
var
  SystemTime: TSystemTime;
begin
  GetLocalTime(SystemTime);
  result := DoEncodeDate(SystemTime.Year, SystemTime.Month, SystemTime.Day);
end ;

{   Time returns the current Time   }

function Time: TDateTime;
var
  SystemTime: TSystemTime;
begin
  GetLocalTime(SystemTime);
  Result := DoEncodeTime(SystemTime.Hour,SystemTime.Minute,SystemTime.Second,SystemTime.MilliSecond) / MSecsPerDay;
end ;

{   Now returns the current Date and Time    }

function Now: TDateTime;
var
  SystemTime: TSystemTime;
begin
  GetLocalTime(SystemTime);
  result := DoEncodeDate(SystemTime.Year,SystemTime.Month,SystemTime.Day) +
            DoEncodeTime(SystemTime.Hour,SystemTime.Minute,SystemTime.Second,SystemTime.MilliSecond) / MSecsPerDay;
end ;

{   IncMonth increments DateTime with NumberOfMonths months,
    NumberOfMonths can be less than zero   }

function IncMonth(const DateTime: TDateTime; NumberOfMonths: integer): TDateTime;
var
  Year, Month, Day: word;
  S : Integer;
begin
  If NumberOfMonths>=0 then
    s:=1
  else
    s:=-1;
  DecodeDate(DateTime, Year, Month, Day);
  inc(Year,(NumberOfMonths div 12));
  inc(Month,(NumberOfMonths mod 12)-1); // Mod result always positive
  if Month>11 then
   begin
     Dec(Month, S*12);
     Inc(Year, S);
   end;
  Inc(Month);                            {   Months from 1 to 12   }
  if (Month = 2) and (IsLeapYear(Year)) and (Day > 28) then
   Day := 28;
  result := Frac(DateTime) + DoEncodeDate(Year, Month, Day);
end ;

{  IsLeapYear returns true if Year is a leap year   }

function IsLeapYear(Year: Word): boolean;
begin
  Result := (Year mod 4 = 0) and ((Year mod 100 <> 0) or (Year mod 400 = 0));
end;

{  DateToStr returns a string representation of Date using ShortDateFormat   }

function DateToStr(Date: TDateTime): string;
begin
  result := FormatDateTime('ddddd', Date);
end ;

{  TimeToStr returns a string representation of Time using ShortTimeFormat   }

function TimeToStr(Time: TDateTime): string;
begin
  result := FormatDateTime('t', Time);
end ;

{   DateTimeToStr returns a string representation of DateTime using ShortDateTimeFormat   }

function DateTimeToStr(DateTime: TDateTime): string;
begin
  result := FormatDateTime('c', DateTime);
end ;

{   StrToDate converts the string S to a TDateTime value
    if S does not represent a valid date value
    an EConvertError will be raised   }

function StrToDate(const S: string): TDateTime;
var
   df:string;
   d,m,y:word;
   n,i:longint;
   c:word;
   dp,mp,yp,which : Byte;
   s1:string[4];
   values:array[1..3] of longint;
   LocalTime:tsystemtime;
begin
  df := UpperCase(ShortDateFormat);
  { Determine order of D,M,Y }
  yp:=0;
  mp:=0;
  dp:=0;
  Which:=0;
  i:=0;
  while (i<Length(df)) and (Which<3) do
   begin
     inc(i);
     Case df[i] of
       'Y' :
         if yp=0 then
          begin
            Inc(Which);
            yp:=which;
          end;
       'M' :
         if mp=0 then
          begin
            Inc(Which);
            mp:=which;
          end;
       'D' :
         if dp=0 then
          begin
            Inc(Which);
            dp:=which;
          end;
     end;
   end;
  if Which<>3 then
   Raise EConvertError.Create('Illegal format string');
{ Get actual values }
  for i := 1 to 3 do
    values[i] := 0;
  s1 := '';
  n := 0;
  for i := 1 to length(s) do
   begin
     if (s[i] in ['0'..'9']) then
      s1 := s1 + s[i];
     if (s[i] in [dateseparator,' ']) or (i = length(s)) then
      begin
        inc(n);
        if n>3 then
         Raise EConvertError.Create('Invalid date format');
        val(s1, values[n], c);
        if c<>0 then
         Raise EConvertError.Create('Invalid date format');
        s1 := '';
      end ;
   end ;
  // Fill in values.
  If N=3 then
   begin
     y:=values[yp];
     m:=values[mp];
     d:=values[dp];
   end
  Else
  begin
    getLocalTime(LocalTime);
    y := LocalTime.Year;
    If n<2 then
     begin
       d:=values[1];
       m := LocalTime.Month;
     end
    else
     If dp<mp then
      begin
        d:=values[1];
        m:=values[2];
      end
    else
      begin
        d:=values[2];
        m:=values[1];
      end;
  end;
  if (y >= 0) and (y < 100) then
   inc(y,1900);
  Result := DoEncodeDate(y, m, d);
end ;


{   StrToTime converts the string S to a TDateTime value
    if S does not represent a valid time value an
    EConvertError will be raised   }

function StrToTime(const s: string): TDateTime;
var
   Len, Current: integer; PM: boolean;

   function GetElement: integer;
   var
     j: integer; c: word;
   begin
   result := -1;
   Inc(Current);
   while (result = -1) and (Current < Len) do begin
      if S[Current] in ['0'..'9'] then begin
         j := Current;
         while (Current < Len) and (s[Current + 1] in ['0'..'9']) do
            Inc(Current);
         val(copy(S, j, 1 + Current - j), result, c);
         end
      else if (S[Current] = TimeAMString[1]) or (S[Current] in ['a', 'A']) then begin
         Current := 1 + Len;
         end
      else if (S[Current] = TimePMString[1]) or (S[Current] in ['p', 'P']) then begin
         Current := 1 + Len;
         PM := True;
         end
      else if (S[Current] = TimeSeparator) or (S[Current] = ' ') then
         Inc(Current)
      else
        raise EConvertError.Create('Invalid Time format');
      end ;
   end ;

var
   i: integer;
   TimeValues: array[0..4] of integer;

begin
Current := 0;
Len := length(s);
PM := False;
for i:=0 to 4 do
  timevalues[i]:=0;
i := 0;
TimeValues[i] := GetElement;
while (i < 5) and (TimeValues[i] <> -1) do begin
   i := i + 1;
   TimeValues[i] := GetElement;
   end ;
If (i<5) and (TimeValues[I]=-1) then
  TimeValues[I]:=0;
if PM then Inc(TimeValues[0], 12);
result := EncodeTime(TimeValues[0], TimeValues[1], TimeValues[2], TimeValues[3]);
end ;

{   StrToDateTime converts the string S to a TDateTime value
    if S does not represent a valid date and time value
    an EConvertError will be raised   }

function StrToDateTime(const s: string): TDateTime;
var i: integer;
begin
i := pos(' ', s);
if i > 0 then result := StrToDate(Copy(S, 1, i - 1)) + StrToTime(Copy(S, i + 1, length(S)))
else result := StrToDate(S);
end ;

{   FormatDateTime formats DateTime to the given format string FormatStr   }

function FormatDateTime(FormatStr: string; DateTime: TDateTime): string;
var
   ResultLen: integer;
   ResultBuffer: array[0..255] of char;
   ResultCurrent: pchar;

   procedure StoreStr(Str: pchar; Len: integer);
   begin
   if ResultLen + Len < SizeOf(ResultBuffer) then begin
      StrMove(ResultCurrent, Str, Len);
      ResultCurrent := ResultCurrent + Len;
      ResultLen := ResultLen + Len;
      end ;
   end ;

   procedure StoreString(const Str: string);
   var Len: integer;
   begin
   Len := Length(Str);
   if ResultLen + Len < SizeOf(ResultBuffer) then begin
      StrMove(ResultCurrent, pchar(Str), Len);
      ResultCurrent := ResultCurrent + Len;
      ResultLen := ResultLen + Len;
      end;
   end;

   procedure StoreInt(Value, Digits: integer);
   var S: string; Len: integer;
   begin
   S := IntToStr(Value);
   Len := Length(S);
   if Len < Digits then begin
      S := copy('0000', 1, Digits - Len) + S;
      Len := Digits;
      end ;
   StoreStr(pchar(@S[1]), Len);
   end ;

   Function TimeReFormat(Const S : string) : string;
   // Change m into n for time formatting.
   Var i : longint;

   begin
     Result:=S;
     For I:=1 to Length(Result) do
       If Result[i]='m' then
         result[i]:='n';
   end;

var
   Year, Month, Day, DayOfWeek, Hour, Minute, Second, MilliSecond: word;

   procedure StoreFormat(const FormatStr: string);
   var
      Token: char;
      FormatCurrent: pchar;
      FormatEnd: pchar;
      Count: integer;
      Clock12: boolean;
      P: pchar;

   begin
   FormatCurrent := Pchar(FormatStr);
   FormatEnd := FormatCurrent + Length(FormatStr);
   Clock12 := false;
   P := FormatCurrent;
   while P < FormatEnd do begin
      Token := UpCase(P^);
      if Token in ['"', ''''] then begin
         P := P + 1;
         while (P < FormatEnd) and (P^ <> Token) do
            P := P + 1;
         end
      else if Token = 'A' then begin
         if (StrLIComp(P, 'A/P', 3) = 0) or
            (StrLIComp(P, 'AMPM', 4) = 0) or
            (StrLIComp(P, 'AM/PM', 5) = 0) then begin
            Clock12 := true;
            break;
            end ;
         end ;
      P := P + 1;
      end ;
   while FormatCurrent < FormatEnd do begin
      Token := UpCase(FormatCurrent^);
      Count := 1;
      P := FormatCurrent + 1;
         case Token of
            '''', '"': begin
               while (P < FormatEnd) and (p^ <> Token) do
                  P := P + 1;
               P := P + 1;
               Count := P - FormatCurrent;
               StoreStr(FormatCurrent + 1, Count - 2);
               end ;
            'A': begin
               if StrLIComp(FormatCurrent, 'AMPM', 4) = 0 then begin
                  Count := 4;
                  if Hour < 12 then StoreString(TimeAMString)
                  else StoreString(TimePMString);
                  end
               else if StrLIComp(FormatCurrent, 'AM/PM', 5) = 0 then begin
                  Count := 5;
                  if Hour < 12 then StoreStr('am', 2)
                  else StoreStr('pm', 2);
                  end
               else if StrLIComp(FormatCurrent, 'A/P', 3) = 0 then begin
                  Count := 3;
                  if Hour < 12 then StoreStr('a', 1)
                  else StoreStr('p', 1);
                  end
               else
                 Raise EConvertError.Create('Illegal character in format string');
               end ;
            '/': StoreStr(@DateSeparator, 1);
            ':': StoreStr(@TimeSeparator, 1);
            ' ', 'C', 'D', 'H', 'M', 'N', 'S', 'T', 'Y','Z' : begin
               while (P < FormatEnd) and (UpCase(P^) = Token) do
                  P := P + 1;
               Count := P - FormatCurrent;
                  case Token of
                     ' ': StoreStr(FormatCurrent, Count);
                     'Y': begin
                           case Count of
                              1: StoreInt(Year, 0);
                              2: StoreInt(Year mod 100, 2);
                              4: StoreInt(Year, 4);
                           end ;
                        end ;
                     'M': begin
                           case Count of
                              1: StoreInt(Month, 0);
                              2: StoreInt(Month, 2);
                              3: StoreString(ShortMonthNames[Month]);
                              4: StoreString(LongMonthNames[Month]);
                           end ;
                        end ;
                     'D': begin
                           case Count of
                              1: StoreInt(Day, 0);
                              2: StoreInt(Day, 2);
                              3: StoreString(ShortDayNames[DayOfWeek]);
                              4: StoreString(LongDayNames[DayOfWeek]);
                              5: StoreFormat(ShortDateFormat);
                              6: StoreFormat(LongDateFormat);
                           end ;
                        end ;
                     'H': begin
                        if Clock12 then begin
                           if Count = 1 then StoreInt(Hour mod 12, 0)
                           else StoreInt(Hour mod 12, 2);
                           end
                        else begin
                           if Count = 1 then StoreInt(Hour, 0)
                           else StoreInt(Hour, 2);
                           end ;
                        end ;
                     'N': begin
                        if Count = 1 then StoreInt(Minute, 0)
                        else StoreInt(Minute, 2);
                        end ;
                     'S': begin
                        if Count = 1 then StoreInt(Second, 0)
                        else StoreInt(Second, 2);
                        end ;
                     'Z': begin
                        if Count = 1 then StoreInt(MilliSecond, 0)
                        else StoreInt(MilliSecond, 3);
                        end ;
                     'T': begin
                        if Count = 1 then StoreFormat(timereformat(ShortTimeFormat))
                        else StoreFormat(TimeReformat(LongTimeFormat));
                        end ;
                     'C':
                       begin
                         StoreFormat(ShortDateFormat);
                         if (Hour<>0) or (Minute<>0) or (Second<>0) then
                          begin
                            StoreString(' ');
                            StoreFormat(TimeReformat(ShortTimeFormat));
                          end;
                       end;
                  end ;
               end ;
            else
              StoreStr(@Token, 1);
         end ;
      FormatCurrent := FormatCurrent + Count;
      end ;
   end ;

begin
  DecodeDate(DateTime, Year, Month, Day);
  DecodeTime(DateTime, Hour, Minute, Second, MilliSecond);
  DayOfWeek := SysUtils.DayOfWeek(DateTime);
  ResultLen := 0;
  ResultCurrent := @ResultBuffer;
  StoreFormat(FormatStr);
  ResultBuffer[ResultLen] := #0;
  result := StrPas(@ResultBuffer);
end ;

{   DateTimeToString formats DateTime to the given format in FormatStr   }

procedure DateTimeToString(var Result: string; const FormatStr: string; const DateTime: TDateTime);
begin
  Result := FormatDateTime(FormatStr, DateTime);
end ;


Function DateTimeToFileDate(DateTime : TDateTime) : Longint;

Var YY,MM,DD,H,m,s,msec : Word;

begin
  Decodedate (DateTime,YY,MM,DD);
  If (YY<1980) or (YY>2099) then
    Result:=0
  else
    begin
    DecodeTime (DateTime,h,m,s,msec);
    Result:=(s shr 1) or (m shl 5) or (h shl 11);
    Result:=Result or DD shl 16 or (MM shl 21) or ((YY-1980) shl 25);
    end;
end;


Function FileDateToDateTime (Filedate : Longint) : TDateTime;

Var Date,Time : Word;

begin
  Date:=FileDate shr 16;
  Time:=FileDate and $ffff;
  Result:=EncodeDate((Date shr 9) + 1980,(Date shr 5) and 15, Date and 31) +
          EncodeTime(Time shr 11, (Time shr 5) and 63, (Time and 31) shl 1,0);
end;

{
  $Log: dati.inc,v $
  Revision 1.1.2.4  2003/01/18 23:44:37  michael
  + Fixed EncodeDate/Time so they use TryEncodeDate/Time

  Revision 1.1.2.3  2002/09/07 20:08:11  carl
    * bugfix 1867

  Revision 1.1.2.2  2001/12/13 18:43:13  michael
    + Added Support for milliseconds to FormatDateTime

  Revision 1.1.2.1  2000/12/16 15:57:15  jonas
    * removed 64bit evaluations when range checking is on

  Revision 1.1  2000/07/13 06:30:58  michael
  + Initial import

  Revision 1.22  2000/06/18 18:02:54  peter
    * fixed decodetime which used trunc instead of round

  Revision 1.21  2000/02/27 14:41:25  peter
    * removed warnings/notes

  Revision 1.20  2000/02/09 16:59:32  peter
    * truncated log

  Revision 1.19  1999/11/29 16:59:27  pierre
   * fix for form bug 719

  Revision 1.18  1999/11/06 14:41:30  peter
    * truncated log

  Revision 1.17  1999/10/28 09:52:29  peter
    * fixed dayofweek

  Revision 1.16  1999/08/11 21:53:04  peter
    * fixed formatdatetime('c',...)
    * fixed strtodate
    * dateencode/decode is now delphi compatible

  Revision 1.15  1999/07/24 11:21:14  peter
    * fixed encode/decode date/time

}

