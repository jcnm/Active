#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "XMLGeneration.h"
#include "AnyFile.h"
#include "AQuery.h"

using namespace std;

string XMLGeneration::MIToBI(ANotifyEvent ane) {
  ostringstream oss;
  string fileName;
  struct stat fileInfo;
  oss << EN_TETE << "<INDEXATION>";
  if (ane.isRennomage()) {
    oss << "<RENOMMAGES id=" << id << "><FICHIERRENOMME>";
    /*
     * TODO : ajouter le nom du oldpath
     * oss << oldpath;
     */
    oss << "<PATH>" << "oldpath" << "</PATH>";
    /* 
     * TODO : ajouter le nom du newpath
     * oss << newpath;
     */
    oss << "<NEWPATH>" << "new_path" << "</NEWPATH>";
    oss << "</FICHIERRENOMME></RENOMMAGES>";
    id++;
  }
  else if (ane.isModification()) {
    fileName = ane.getName();
    if (stat(fileName, &fileInfo) == 0) {
      oss << "<MODIFICATIONS id=" << id << "><FICHIERMOFIFIE>";
      oss << "<PATH>" << ane.getName() << "</PATH>";
      oss << "<DATEMODIFICATION>" << ctime(&fileInfo.st_mtime) << "</DATEMODIFICATION>";
      oss << "<TAILLE>" << fileInfo.st_size << "</TAILLE>";
      oss << "<PROPRIETAIRE>" << fileInfo.st_uid << "</PROPRIETAIRE>";
      oss << "<GROUPE>" << fileInfo.st_gid << "</GROUPE>";
      oss << "<PERMISSIONS>" << fileInfo.st_mode << "</PERMISSIONS>";
      /*
       * TODO : ajouter les mots d'indexages par appel de fonction
       *        qui doit renvoyer un string
       */
      oss << "<INDEXAGE>" << "mots_a_indexer(fonction)" <<"</INDEXAGE>";
      oss << "<NEWPATH>" << ane.getName() << "</NEWPATH>";
      oss << "</FICHIERMODIFIE></MODIFICATIONS>";
      id++;
    }
    else {
      // Le fichier n'existe pas
      return "";
    }
  }
  else if (ane.isSuppression()) {
    oss << "<SUPPRESSIONS id=" << id << "><FICHIERSUPPRIME>";
    oss << "<FICHIERSUPPRIME>" << ane.getName() << "</FICHIERSUPPRIME>";
    oss << "</FICHIERSUPPRIME></SUPPRESSIONS>";
    id++;
  }
  else if (ane.isCreation()) {
    fileName = ane.getName();
    if (stat(fileName, &fileInfo) == 0) {
      oss << "<CREATIONS id=" << id << "><FICHIERCREE>";
      oss << "<PATH>" << fileName << "</PATH>";
      /*
       * TODO : recuperation du format :
       *        - soit sur ane par ane.getFormat()
       *        - soit sur le nom du fichier
       */
      oss << "<format>" << ane.getFormat() << "</format>";
      /*
       * Liberte : la date de creation d'un fichier est identique a sa date de
       * derniere modification au moment de sa creation
       */
      oss << "<DATECREATION>" << ctime(&fileInfo.st_mtime) << "</DATECREATION>";
      oss << "<TAILLE>" << fileInfo.st_size << "</TAILLE>";
      oss << "<PROPRIETAIRE>" << fileInfo.st_uid << "</PROPRIETAIRE>";
      oss << "<GROUPE>" << fileInfo.st_gid << "</GROUPE>";
      oss << "<PERMISSIONS>" << fileInfo.st_mode << "</PERMISSIONS>";
      /*
       * TODO : ajouter les mots d'indexages par appel de fonction
       *        qui doit renvoyer un string
       */
      oss << "<INDEXAGE>" << "mots_a_indexer(fonction)" << "</INDEXAGE>";
      oss << "</FICHIERCREE></CREATIONS>";
      id++;
    }
    else {
      // Le fichier n'existe pas
      return "";
    }
  }
  else {
    // ces cas ne sont pas pris en compte par la dtd
    return "";
  }
  oss << "</INDEXATION>";
  return oss.str();
}

// comme explique dans le code des ANotify la convention i/o (input/output) p/r(pointer/ref) nom_variable 
// permet de vite analyser la le fonctionnement de la fonction. ipquery voulait dire input pointer query ici aq devrait etre nomme
// iraq par exemple...
string XMLGeneration::BIToMR(AQuery& aq, int searchID) {
  ostringstream oss;
  if(ipquery == NULL || !aq.hasNewResult()) return oss.str();
  vector<AnyFile>* AQResult = aq.results();
  AnyFile::iterator tmpIt = AQResult->begin();
  oss << "<RESULT id=" << searchID << ">";
  if(AQResult.hasNewResult()) {
    for (int i(0); i < AQResult.size(); ++i) {
      oss << "<FILE>";
      oss << "<NAME>" << aq.AQResult[i].getName() << "</NAME>";
      oss << "<PATH>" << aq.AQResult[i].getPath() << "</PATH>";
      oss << "<PERM>" << aq.AQResult[i].getMime() << "</PERM">;
      oss << "<SIZE>" << aq.AQResult[i].getDiskSize() << "</SIZE>";
      if (aq.AQResult[i].getLastModif() != NULL) {
	oss << "<LASTMODIF>" << aq.AQResult[i].getLastModif() << "</LASTMODIF>";
      }
      oss << "<PROPRIO>" << aq.AQResult[i].getUserID() << "</PROPRIO>";
      oss << "</FILE>";
    }
  }
  oss << "</RESULT>";
  return oss.str();
}

string XMLGeneration::MRToBI(MR mr, int searchID) {
  ostringstream oss;
  oss << "<SEARCH id=" << searchID << ">";
  oss << "<WORD>" << mr.getWord() << "</WORD>";
  oss << "<CONTENT>" << mr.getContent() << "</CONTENT>";
  if (mr.getPathDir() != NULL) {
    oss << "<PATHDIR>" << mr.getPathDir() << "</PATHDIR>";
  }
  if (mr.getPerm() != NULL) {
    oss << "<PERM>" << mr.getPerm() << "</PERM>";
  }
  if (mr.getExtension() != NULL) {
    oss << "<EXTENSION>" << mr.getExtension() << "</EXTENSION>";
  }
  if (mr.timeSlot()) {
    oss << "<TIMESLOT><BEGIN>";
    oss << "<DAY>" << mr.getBeginDay() << "</DAY>";
    oss << "<MONTH>" << mr.getBeginMonth() << "</MONTH>";
    oss << "<YEAR>" << mr.getBeginYear() << "</YEAR>";
    oss << "</BEGIN><END>";
    oss << "<DAY>" << mr.getEndDay() << "</DAY>";
    oss << "<MONTH>" << mr.getEndMonth() << "</MONTH>";
    oss << "<YEAR>" << mr.getEndYear() << "</YEAR>";
    oss << "</END></TIMESLOT>";
  }
  oss << "</SEARCH>";
  return oss;
}
