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
  RecStr  =  10,    // Буфер 8 килобайт
  RecNStr =  11;    // Строка создается strnew, и это указатель на char*

inline char *strnew ( const char * s )
{
  char *a = new char[ strlen( s ) +1 ];

  if( a ) strcpy( a, s );
  else;

  return a;
}
/** Идея такая - каждый конфигурируемый параметр мы задаем в виде ниженаписанного класса.
  * Он умеет, кроме конструирования и умирания разбирать строчку config-файла
  * с целью найти свой параметр и обработать его.
  */
class ConfigRec {

  char *Str;
  void *Data;
  int RecType;
  ConfigRec *Next;

public:

  ConfigRec( char *aStr, void *aData, int aType, ConfigRec *aNext = 0 );
  virtual ~ConfigRec();

/** Ежели название этой записи совпадаетс с <Parm> -
          преобразовать и сохранить <Val> в <Data>, вернется 1.
  * Ежели нет - передать следующей записи, может, у нее получиться.
  * Если не судьба, вернется 0
  */
  int Parse( char *Parm, char *Val );

/** Ежели <Parm> не 0 и название этой записи совпадает с <Parm> -
  * записать в файл строчку <Str>=<*Data>, забыть Data, вернуть 1.
  * Если имя не совпало - передать по цепочке дальше.
  * Если <Parm> == 0 - записать не сравнивая имя, также забыть Data
  * и безусловно передать дальше. Если не судьба, вернется 0.
  * '\n' не пишется
  */
  int Write( FILE * f, char *Parm = 0 );

/** Какая запись - следующая */
  ConfigRec * NextOne() const   { return Next; }
/** Назначить следующую запись */
  void NextOne( ConfigRec * n ) { Next = n; }
};

/** Тут идея такая - данный класс берет конфиг-файл и читает из него
  * указанные ему параметры.
  *
  * Пример:
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

/** Собственно этот метод и читает файл, передавая строки для разборки
  * ConfigRec'ам, которые выбирают из файла нужные поля. Если указан DelRecs,
  * то ConfigRec'и были созданы персонально для нас и по выходу их надо удалить.
  * Такой вариант используется по умолчанию.
  */
  int Read( ConfigRec *Recs, int DelRecs = 1 );

/** Этот метод записывает парамтры в конфиг файл
  * */
  int Write( ConfigRec *Recs, int DelRecs = 1 );

/** Этот метод подсчитывает контрольную сумму значений
  * конфигурационного файла, из подсчета исключается поле
  * указанное в параметре - это поле должно содержать
  * контрольную сумму.
  * Возвращает 0xFFFFFFFF если не может открыть файл.*/
  //unsigned long CheckSum(const char *aCheckSumField);

};

#endif /* CONFIGFILE_H_ */
