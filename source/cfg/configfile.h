/*
 * configfile.h
 *
 *  Created on: 07.09.2018
 *      Author: bsa
 */

#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cfgbase.h"

//typedef unsigned char byte;
//typedef unsigned long ulong;
//typedef unsigned short ushort;
//typedef unsigned short word;
//typedef unsigned long dword;
//typedef int boolean;

const unsigned int
  RecVoid =   0,
  RecByte =   1,
  RecInt  =   4,
  RecStr  =  10,    // ����� 8 ��������
  RecNStr =  11;    // ������ ��������� strnew, � ��� ��������� �� char*

inline char *strnew ( const char * s )
{
  char *a = new char[ strlen( s ) +1 ];

  if( a ) strcpy( a, s );
  else;

  return a;
}
/** ���� ����� - ������ ��������������� �������� �� ������ � ���� ��������������� ������.
  * �� �����, ����� ��������������� � �������� ��������� ������� config-�����
  * � ����� ����� ���� �������� � ���������� ���.
  */
class ConfigRec {

  char *Str;
  void *Data;
  int RecType;
  ConfigRec *Next;

public:

  ConfigRec( char *aStr, void *aData, int aType, ConfigRec *aNext = 0 );
  virtual ~ConfigRec();

/** ����� �������� ���� ������ ���������� � <Parm> -
          ������������� � ��������� <Val> � <Data>, �������� 1.
  * ����� ��� - �������� ��������� ������, �����, � ��� ����������.
  * ���� �� ������, �������� 0
  */
  int Parse( char *Parm, char *Val );

/** ����� <Parm> �� 0 � �������� ���� ������ ��������� � <Parm> -
  * �������� � ���� ������� <Str>=<*Data>, ������ Data, ������� 1.
  * ���� ��� �� ������� - �������� �� ������� ������.
  * ���� <Parm> == 0 - �������� �� ��������� ���, ����� ������ Data
  * � ���������� �������� ������. ���� �� ������, �������� 0.
  * '\n' �� �������
  */
  int Write( FILE * f, char *Parm = 0 );

/** ����� ������ - ��������� */
  ConfigRec * NextOne() const   { return Next; }
/** ��������� ��������� ������ */
  void NextOne( ConfigRec * n ) { Next = n; }
};

/** ��� ���� ����� - ������ ����� ����� ������-���� � ������ �� ����
  * ��������� ��� ���������.
  *
  * ������:
  * int TCPPort;
  * ConfigFile *CF = new ConfigFile( "msgr.cfg" );
  * CF->Read( new ConfigRec( "tcpipPort", &TCPPort, RecInt, NULL ) );
  * delete CF;
  */

#include <string>

class ConfigFile {

private:
  std::string FileName;

public:

  ConfigFile ( const char *aName );
  virtual ~ConfigFile();

/** ���������� ���� ����� � ������ ����, ��������� ������ ��� ��������
  * ConfigRec'��, ������� �������� �� ����� ������ ����. ���� ������ DelRecs,
  * �� ConfigRec'� ���� ������� ����������� ��� ��� � �� ������ �� ���� �������.
  * ����� ������� ������������ �� ���������.
  */
  int Read( ConfigRec *Recs, int DelRecs = 1 );

/** ���� ����� ���������� �������� � ������ ����
  * */
  int Write( ConfigRec *Recs, int DelRecs = 1 );

/** ���� ����� ������������ ����������� ����� ��������
  * ����������������� �����, �� �������� ����������� ����
  * ��������� � ��������� - ��� ���� ������ ���������
  * ����������� �����.
  * ���������� 0xFFFFFFFF ���� �� ����� ������� ����.*/
  //unsigned long CheckSum(const char *aCheckSumField);

};

#endif /* CONFIGFILE_H_ */
