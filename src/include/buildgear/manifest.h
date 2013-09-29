/*
 * This file is part of Build Gear.
 *
 * Copyright (C) 2013  Torsten Lund
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
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
	void generateManifest(list<CBuildFile*> *buildfiles);
	void setFormat(DocumentFormat df);
	void setHeader(string header);
	void setFooter(string footer);

private:
	void generateXML(list<CBuildFile*> *buildfiles);
	void generateXMLComponentProperties(CBuildFile* file);
	void generatePlainText(list<CBuildFile*> *buildfiles);
	void generatePlainTextComponentProperties(CBuildFile* file);
	void generateHTML(list<CBuildFile*> *buildfiles);
	void open(string filename);
	void close();
	void debug(string msg);
	ManifestData mManifestData;
};

extern CManifest Manifest;

#endif /* MANIFEST_H_ */
