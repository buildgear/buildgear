/*
 * manifest.h
 *
 *  Created on: May 7, 2013
 *      Author: Torsten Lund
 */

#ifndef MANIFEST_H_
#define MANIFEST_H_

#include <list>
#include <string>
#include "buildgear/filesystem.h"

using namespace std;

typedef enum {
	FORMAT_PLAIN_TEXT,
	FORMAT_XML,
	FORMAT_HTML,
	FORMAT_NOT_SUPPORTED
} DocumentFormat;

typedef struct {
	string header;
	string footer;
	DocumentFormat documentFormat;
	string fileExtension;
	ofstream file;
} ManifestData;

class CManifest {

public:
	CManifest();
	void generateManifest(CBuildFile *buildfile);
	void setFormat(DocumentFormat df);
	void setHeader(string header);
	void setFooter(string footer);

private:
	void generateXMLFull();
	void generateXMLProject(CBuildFile* file);
	void generateXMLProjectProperties(CBuildFile* file);
	void generatePlainText();
	void generateHTML();
	void open(string filename);
	void close();
	void debug(string msg);
	ManifestData mManifestData;
};

extern CManifest Manifest;

#endif /* MANIFEST_H_ */
