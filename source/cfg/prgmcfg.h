/*
 * PrgmCfg.h
 *
 *  Created on: 07.09.2018
 *      Author: bsa
 */

#ifndef __PRGMCFG_H__
#define __PRGMCFG_H__

#include <stdlib.h>

#include <string>
using namespace std;

#include "configfile.h"
#include "cfgbase.h"

class ProgramConfig {

public:
  ProgramConfig (string aConfigFileName, string aAltConfigFileName);
/**/ 
  ~ProgramConfig ();
/**/
  string GetStringParam(string aParam, string aDefaultValue);
/**/
  int GetIntParam(string aParam, int aDefaultValue);
/**/
  int SetStringParam(string aParam, string aValue);
/**/
  int SetIntParam(string aParam, int aValue);

/**/
  int CheckValid( int aCFile);
/**/
  int CheckValid();
/**/
  //unsigned long CheckSum(int aCFile);
 
  //string GetBaseDir();

 
  string ConfigFileName();
 
  string AltConfigFileName();

protected:
/**/
  ConfigFile *mConfigFile;
/**/ 
  ConfigFile *mAltConfigFile;

private:
/**/
  std::string mConfigFileName;
/**/ 
  std::string mAltConfigFileName;
};

//#endif //_PROGRAMMCONFIG_HPP_

#endif /* __PRGMCFG_H__ */
