Uses Dos;

{ Declared EXPLICITLY with Ansistring, so NO mistaking is possible }

Function Getenv (Var EnvVar  : AnsiString): AnsiString;

begin
  Result:=Dos.Getenv(EnvVar);
end;
{
  $Log: ezcgi.inc,v $
  Revision 1.2  2002/09/07 15:15:22  peter
    * old logs removed and tabs fixed

}
