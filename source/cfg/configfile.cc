/*
 * configfile.cc
 *
 *  Created on: 07.09.2018
 *      Author: bsa
 */

#include "configfile.h"


ConfigRec::ConfigRec( char *aStr, void *aData,  int aType, ConfigRec *aNext )
{
    Str = strnew (aStr);
    Data = aData;
    RecType = aType;
    Next = aNext;
  }

ConfigRec::~ConfigRec()
{
    delete [] Str;
    if ( Next ) delete Next;
  }

/* � ��� ��� ����� ������� �������� ����� � ������ ������ ��������...*/
int ConfigRec::Parse( char *Parm, char *Val )
{
    if( strcasecmp( Parm, Str ) == 0 )
    {
        switch( RecType )
        {
             case RecByte: * (unsigned char*) Data = atoi( Val ); break;
             case RecInt:  * (int*)  Data = atoi( Val ); break;
             case RecStr:  * strcpy( (char*)Data, Val ); break;
             case RecNStr: * (char**) Data = strnew( Val ); break;
        }

        return 1;
    }
    else if( Next )
              return Next->Parse( Parm, Val );
         else return 0;
}

int ConfigRec::Write ( FILE * f, char *Parm )
{
// <Parm> == 0 means unconditional writing of those records which
// doesn't exist in the original file
  if( !Parm || (strcasecmp( Parm, Str ) == 0) )
  {
     switch( RecType )
     {
        case RecVoid:
              // Comment out the duplicating string
              if( Parm )
              {
                fprintf ( f, "# " );
                return 0; // Returning 0 means "write the string back, no changes"
              }
              return Next ? Next->Write( f, Parm ) : 1;

        case RecByte:  fprintf( f, "%s=%d", Str, (int)(*(unsigned char*)Data)  );     break;
        case RecInt:   fprintf( f, "%s=%d", Str, *(int*)Data );              break;
        case RecStr:   fprintf( f, "%s=%s", Str, (char*)Data );              break;
        case RecNStr:  fprintf( f, "%s=%s", Str, *(char**)Data );            break;
     }
     RecType = RecVoid; // We've done it, never again...
     if( Parm )
         return 1;
     else;

     fputc( '\n', f );

     return Next ? Next->Write( f, Parm ) : 1;
  }

  // Not me, but maybe next one
  return Next ? Next->Write( f, Parm ) : 0;
}

// ��� ���� ����� - ������ ����� ����� ������-���� � ������ �� ����
// ��������� ��� ���������.
ConfigFile::ConfigFile( const char *aName )
{
    FileName = aName;
}

ConfigFile::~ConfigFile()
{
    //delete [] FileName;
    FileName.clear ();
}

// ������ ���� � ���� ������ ���� 'Parm = Value'
int ConfigFile::Read ( ConfigRec *Recs, int DelRecs )
{

    if( !Recs ) return 2;

    const unsigned int SBufLen = 1024;
    char sbuf[ SBufLen ],*s,*t,*v;

    FILE *f = NULL;
    f = fopen (FileName.c_str (), "r" );
    if( f ) {

      while( !feof( f ) ) {
        fgets( sbuf, SBufLen, f );
        // ���������� �����������
        s = strchr( sbuf, '#' );
        if( s ) *s = 0;
        t = strchr( sbuf, '=' );
        if( t ) {
          for( s=t-1; (s >= sbuf) && ( (*s == ' ') || (*s == '\t') ); s-- )
            *s = 0;
          s = sbuf;
          while( ( *s == ' ' ) || ( *s == '\t' ) )
            *s++ = 0;
          *t++ = 0;
          while( ( *t == ' ' ) || ( *t == '\t' ) )
          *t++ = 0;
          v = t + strlen( t ) -1;
          while( (v >= t) &&
                 ( (*v == ' ') || (*v == '\t') || (*v == '\n') || (*v == '\r') ) )
            *v-- = 0;
          // ������ ������� ����� - � s ����� ���, � � t - ��������
          Recs->Parse( s, t );
        }

      }
      fclose( f );
      if( DelRecs ) delete Recs;
      return 0;
    }
    if( DelRecs ) delete Recs;
    return 1;
  }

  // ������ ���� � ���� ������ ���� 'Parm = Value'
  // � ������������ ��� � ������ ���� � ������ ����������.
int ConfigFile::Write( ConfigRec *Recs, int DelRecs )
{
/*
    if( !Recs ) return 2;

    const unsigned int SBufLen = 1024;
    char sbuf[ SBufLen+1 ], sbuf2[ SBufLen+1 ], *cmt, *s,*t,*v, tempfname[SBufLen+1];
    char drive[_MAX_DRIVE+1],
     dir[_MAX_DIR+1],
     fname[_MAX_FNAME+1],
     ext[_MAX_EXT+1];

    char  tdrive[_MAX_DRIVE+1],
      tdir[_MAX_DIR+1],
      tfname[_MAX_FNAME+1],
      text[_MAX_EXT+1];
    size_t l;

    char *FullName = _fullpath( NULL, FileName, 0 );
    _splitpath_s (FullName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

    char *tmpfname = _tempnam(NULL,"CFG");
    _splitpath (tmpfname, tdrive, tdir, _MAX_DIR, tfname, _MAX_FNAME, text, _MAX_EXT);
    _makepath (tempfname, SBufLen, drive, dir, tfname, text);
    // printf("tempfile: %s",tempfname);
    free( tmpfname );
    free( FullName );

  FILE *ft;
  fopen (&ft, tempfname, "w+" );
  FILE *f;
  fopen (&f, FileName, "r" );
    if( ft ) // ���� ���� ���� ������
    {
      if( f ) // ... � �� ��� �����, ������ ������...
      {
         while( fgets( sbuf, SBufLen, f ) ) // ������ ���������
         {
         // �������� '\n', ���� �� ������� � �����
           l = strlen( sbuf );
           if( l && sbuf[ l-1 ] == '\n' )
             sbuf[ l-1 ] = '\0';
         // ������� ������ ����� ������� - �����, ���� ��������� ������
         // ���������� ������� � �������� ����
           strcpy_s ( sbuf2, SBufLen, sbuf );

         // ���������� �����������
           cmt = s = strchr( sbuf, '#' );
           if( s )
             // ������ <cmt> ���������� �� ������ ������ ����������� ����� #
             *cmt++ = 0;

           t = strchr( sbuf, '=' );
           if( t )
           {
             for( s=t-1; (s >= sbuf) && ( (*s == ' ') || (*s == '\t') ); s-- )
               *s = 0;
             s = sbuf;
             while( ( *s == ' ' ) || ( *s == '\t' ) )
               *s++ = 0;
             *t++ = 0;
             while( ( *t == ' ' ) || ( *t == '\t' ) )
             *t++ = 0;
             v = t + strlen( t ) -1;
             while( (v >= t) &&
                    ( (*v == ' ') || (*v == '\t') || (*v == '\n') ) )
               *v-- = 0;
           // ������ ������� ����� - � s ����� ���, � � t - ��������
             if( Recs->Write( ft, s ) ) // ���� �������� ��������
             {
               // �������� �����������, ���� ���
               if( cmt )
                 fprintf( ft, " # %s\n", cmt );
               else
                 fputc( '\n', ft );
             }
             else  // ������ �������� �� ���� ������� - ����� ������� ��� ����
             {
               fputs( sbuf2, ft );
               fputc( '\n', ft );
             }
           }  // if( t )
           else // ������� = �� ������� - ����� ������� ��� ����
           {
             fputs( sbuf2, ft );
             fputc( '\n', ft );
           }

         } // while
      } // if( f )

    // ��������� �� ���������, ������� �� ���� ������� � �������� �����
    // ���� ��������� ����� �� ����, �� ����� ������� � ����
      Recs->Write( ft );

      fclose( f );
      fclose( ft );
      remove( FileName );
      rename( tempfname, FileName);
      if( DelRecs ) delete Recs;
      return 0;
    } // if( ft )
    if (f)
      fclose( f );
    if (ft)
      fclose( ft );
    if( DelRecs ) delete Recs;
*/
    return 1;
  }

/*
unsigned long ConfigFile::CheckSum(const char *aCheckSumField)
{
    unsigned long CheckSum = 0xFFFFFFFF;

    const unsigned int SBufLen = 1024;
    char sbuf[ SBufLen ],*s,*t,*v;

    FILE *f;
  fopen_s (&f, FileName, "r" );
    if( f ) {

      while( !feof( f ) ) {
        fgets( sbuf, SBufLen, f );
        // ���������� �����������
        s = strchr( sbuf, '#' ); if( s ) *s = 0;
        t = strchr( sbuf, '=' );
        if( t ) {

          for( s=t-1; (s >= sbuf) && ( (*s == ' ') || (*s == '\t') ); s-- )
            *s = 0;
          s = sbuf; while( ( *s == ' ' ) || ( *s == '\t' ) ) *s++ = 0;
          *t++ = 0;
          while( ( *t == ' ' ) || ( *t == '\t' ) ) *t++ = 0;
          v = t + strlen( t ) -1;
          while( (v >= t) &&
                 ( (*v == ' ') || (*v == '\t') || (*v == '\n') ) ) *v-- = 0;
          // ������ ������� ����� - � s ����� ���, � � t - ��������
          if (_strcmpi( s, aCheckSumField ) != 0) {
            while (*t != 0) {
               CheckSum += *t++;
               CheckSum += *t << 16;
            }
          }
        }

      }
      fclose( f );
    }

    return CheckSum;
  }
*/


