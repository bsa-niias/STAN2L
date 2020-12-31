#include "prgmcfg.h"

#include <string.h>



ProgramConfig::ProgramConfig (string aConfigFileName, string aAltConfigFileName)
{
  //char path_buffer[_MAX_PATH+1]="";
  //char drive[_MAX_DRIVE+1]="";
  //char dir[_MAX_DIR+1]="";
  //char fname[_MAX_FNAME+1]="";
  //char ext[_MAX_EXT+1]="";

  //strcpy (path_buffer, aConfigFileName.c_str());
  //_splitpath (path_buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

  //if (strlen(dir)==0 && strlen(drive)==0)
  //{
  //    mConfigFileName = GetBaseDir()+string("\\CFG\\")+aConfigFileName;
  //}
  //else
  mConfigFileName = aConfigFileName;

  //strcpy_s (path_buffer, _MAX_PATH, aAltConfigFileName.c_str());
  //_splitpath(path_buffer, drive, dir, fname, ext);
  //_splitpath_s (path_buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

  //if (strlen(dir)==0 && strlen(drive)==0)
  //{
  //    mAltConfigFileName = GetBaseDir()+string("\\CFG\\")+aAltConfigFileName;
  //}
  //else
  mAltConfigFileName = aAltConfigFileName;

  mConfigFile = new ConfigFile( mConfigFileName.c_str() );
  mAltConfigFile = new ConfigFile( mAltConfigFileName.c_str() );
}

 
ProgramConfig::~ProgramConfig()
{
  delete mConfigFile;
  delete mAltConfigFile;
}

 
string ProgramConfig::GetStringParam(string aParam, string aDefaultValue){
  char buf[255+1] = "";
  strcpy ((char *)&buf, aDefaultValue.c_str());
  ConfigRec *CR = new ConfigRec( (char *)aParam.c_str(), buf, RecStr, NULL );
  if (mConfigFile)
    mConfigFile->Read( CR, 0);
  if (mAltConfigFile)
    mAltConfigFile->Read( CR);
  return string( buf );
}

 
int ProgramConfig::GetIntParam(string aParam, int aDefaultValue){
  int buf = aDefaultValue;
  ConfigRec *CR = new ConfigRec( (char *)aParam.c_str(), &buf, RecInt, NULL );
  if (mConfigFile)
    mConfigFile->Read( CR, 0);
  if (mAltConfigFile)
    mAltConfigFile->Read( CR);
  return buf;
}

/* 
 * 
 */
int ProgramConfig::SetStringParam( string aParam, string aValue )
{
  char buf[255+1] = "";
  strncpy( buf, aValue.c_str(), sizeof( buf ) );//[ sizeof(buf)-1 ] = '\0';
  ConfigRec *CR = new ConfigRec( (char *)aParam.c_str(), buf, RecStr, NULL );
  return mConfigFile ? !mConfigFile->Write( CR ) : 0;
}

/* 
 * 
 */
int ProgramConfig::SetIntParam(string aParam, int aValue)
{
  int buf = aValue;
  ConfigRec *CR = new ConfigRec( (char *)aParam.c_str(), &buf, RecInt, NULL );
  return mConfigFile ? !mConfigFile->Write( CR ) : 0;
}

/* 
 * 
 * 
 */ 
int ProgramConfig::CheckValid( int aCFile)
{
/*
  unsigned long curConfigKey = 0 ;
 // 
  unsigned long configKey = 0;
  char * buf;
  char  skey[255+1]="";
  int rc=0;

  switch (aCFile) {
    case 0:
     if (mConfigFile)
       curConfigKey = mConfigFile->CheckSum("configKey");
       mConfigFile->Read( new ConfigRec( "configKey", skey, RecStr, NULL ) );
       configKey = strtoul(skey,&buf,16);
       rc = (configKey == curConfigKey);
     break;
    case 1:
     strcpy_s ( (char *)&skey,255, "FFFFFFFF");
     if (mAltConfigFile) {
       curConfigKey = mAltConfigFile->CheckSum("configKey");
       mAltConfigFile->Read( new ConfigRec( "configKey", skey, RecStr, NULL ) );
       configKey = strtoul(skey,&buf,16);
       rc = (configKey == curConfigKey);
     }
     break;
  }
  return rc;
*/
  return 0;
}

/* 
 * 
 */
int ProgramConfig::CheckValid()
{
  //int MainCheck = CheckValid(0);
  //int AltCheck = CheckValid(1);
  //return (MainCheck && AltCheck);
	return 0;
}

/* 
 * 
*/
/*
unsigned long ProgramConfig::CheckSum(int aCFile){
  unsigned long curConfigKey = 0 ;
  switch (aCFile) {
    case 0:
      if (mConfigFile)
        curConfigKey = mConfigFile->CheckSum("configKey");
      break;
    case 1:
      if (mAltConfigFile)
        curConfigKey = mAltConfigFile->CheckSum("configKey");
      break;
  }
  return curConfigKey;
}
*/

/* */
//string ProgramConfig::GetBaseDir()
//{
  //char * tmp = getenv ("MALS2_BASE");
  //return string(tmp ? tmp : "..");
//}

/* */
string ProgramConfig::ConfigFileName(){  return mConfigFileName;}
/* */
string ProgramConfig::AltConfigFileName(){  return mAltConfigFileName;}




