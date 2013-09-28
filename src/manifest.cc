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

#include "config.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "buildgear/config.h"
#include "buildgear/manifest.h"

extern CBuildFiles BuildFiles;

int cmp_field_lenght = 34;
int ver_field_lenght = 20;
int lic_field_lenght = 20;
int dsc_field_lenght = 60;

CManifest::CManifest() {
	mManifestData.documentFormat = FORMAT_NOT_SUPPORTED;
	mManifestData.header = "Autogenerated by Build Gear v" VERSION " (http://www.buildgear.org)";
	mManifestData.footer = "";
	mManifestData.fileExtension = "";
}

void CManifest::generateManifest(list<CBuildFile*> *buildfiles) {
	switch (mManifestData.documentFormat) {
	case FORMAT_PLAIN_TEXT:
		generatePlainText(buildfiles);
		break;
	case FORMAT_XML:
      generateXML(buildfiles);
		break;
	case FORMAT_HTML:
		generateHTML(buildfiles);
		break;
	case FORMAT_NOT_SUPPORTED:
	default:
		cout << "Error: Document format not supported." << endl;
      exit(EXIT_FAILURE);
		break;
	}
}

void CManifest::generateXML(list<CBuildFile*> *buildfiles) {
	list<CBuildFile*>::iterator it;
	string temp;
	string native = "native";
	string cross = "cross";
   string filename = MANIFEST_FILE "." + Config.name_stripped + mManifestData.fileExtension;

	/*
	 * XML defined format
	 *
	 * <?xml version="1.0" encoding="UTF-8"?>
	 * <manifest>
	 *   <project name="" />
	 *   <native>
	 *     <component name="" version="" release="" license="" url="" description="" />
	 *   </native>
	 *   <cross>
	 *     <component name="" version="" release="" license="" url="" description="" />
	 *   </cross>
	 * </manifest>
	 */

	open(filename);

	cout << "Generating manifest..           " << flush;

	mManifestData.file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	mManifestData.file << "<manifest>" << endl;

   if (mManifestData.header != "")
      mManifestData.file << "  <!-- " << mManifestData.header << " -->" << endl;

	//cout << "  <project name=\"\" />" << endl; TODO: At some point add this field

	mManifestData.file << "  <native>" << endl;

   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
      if ("native" == (*it)->type)
         generateXMLProjectProperties((CBuildFile*) (*it));
   }

	mManifestData.file << "  </native>" << endl;
	mManifestData.file << "  <cross>" << endl;

   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
   {
		if ("cross" == (*it)->type)
			generateXMLProjectProperties((CBuildFile*) (*it));
	}

	mManifestData.file << "  </cross>" << endl;

	if (mManifestData.footer != "")
      mManifestData.file << "  <!-- " << mManifestData.footer << " -->" << endl;

	mManifestData.file << "</manifest>" << endl;

	mManifestData.file.close();

	cout << "Done" << endl << endl;
	cout << "Saved manifest to " << filename << endl;
}

void CManifest::generateXMLProject(CBuildFile* file) {
	cout << file->name << endl;
}

void CManifest::generateXMLProjectProperties(CBuildFile* file) {
	mManifestData.file << "    <component name=\"" << file->short_name << "\"";

	if (file->version.length() != 0) {
		mManifestData.file << " version=\"" << file->version << "\"";
	}

	if (file->release.length() != 0) {
		mManifestData.file << " release=\"" << file->release << "\"";
	}

	//mManifestData.file << " license=\""<< "N/A" << "\""; TODO

	if (file->source.length() != 0) {
		size_t found = file->source.find(" ");
		if (found != std::string::npos)
			file->source.erase(file->source.begin() + found,
					file->source.end());

		mManifestData.file << " url=\"" << file->source << "\"";
	}

	//mManifestData.file << " description=\""<< "N/A" << "\""; TODO

	mManifestData.file << " />" << endl;
}

void CManifest::generatePlainText(list<CBuildFile*> *buildfiles) {
	list<CBuildFile*>::iterator it;
	ostringstream headLine, seperator;
	string cmp_name = "Component name";
	string ver_name = "Version";
	string lic_name = "License";
	string dsc_name = "Description";
   string filename = MANIFEST_FILE "." + Config.name_stripped + mManifestData.fileExtension;

	/*
	 * Plain text defined format
	 *
	 * Component name      Version    License    Description
	 * ===================-==========-==========-=======================================================
	 *
	 */

	open(filename);

	cout << "Generating manifest..           " << flush;

	if (mManifestData.header != "") {
		mManifestData.file << mManifestData.header << endl << endl;
	}

	headLine << cmp_name;
	for(int i=0;i<(cmp_field_lenght - cmp_name.length())+2;i++)
		headLine << " ";

	headLine << ver_name;
	for(int i=0;i<ver_field_lenght-ver_name.length();i++)
		headLine << " ";

	headLine << lic_name;
	for(int i=0;i<lic_field_lenght-lic_name.length();i++)
		headLine << " ";

	headLine << dsc_name;
	for(int i=0;i<dsc_field_lenght-dsc_name.length();i++)
		headLine << " ";

	mManifestData.file  << headLine.str() << endl;

	for(int i=0; i<cmp_field_lenght+1;i++)
		seperator << "=";
	seperator << "-";
	for(int i=0; i<ver_field_lenght-1;i++)
		seperator << "=";
	seperator << "-";
	for(int i=0; i<lic_field_lenght-1;i++)
		seperator << "=";
	seperator << "-";
	for(int i=0; i<dsc_field_lenght;i++)
		seperator << "=";

	mManifestData.file << seperator.str() <<endl;

   for (it=buildfiles->begin(); it!=buildfiles->end(); it++)
		generatePlainTextProjectProperties((CBuildFile*) (*it));

	mManifestData.file.close();

	cout << "Done" << endl << endl;
	cout << "Saved manifest to " << filename << endl;
}

void CManifest::generatePlainTextProjectProperties(CBuildFile* file) {
	ostringstream line;

	if("cross" == file->type) {
		line << "c ";
	}

	if("native" == file->type) {
		line << "n ";
	}

	line << file->short_name;
	for(int i=0; i<cmp_field_lenght - file->short_name.length() ;i++) {
		line << " ";
	}

	line << file->version;
	for(int i=0; i<ver_field_lenght - file->version.length(); i++) {
		line << " ";
	}
	mManifestData.file << line.str() << endl;
}

void CManifest::generateHTML(list<CBuildFile*> *buildfiles) {
}

void CManifest::setFooter(string footer) {
	mManifestData.footer = footer;
}

void CManifest::setHeader(string header) {
	mManifestData.header = header;
}

void CManifest::setFormat(DocumentFormat df) {
	mManifestData.documentFormat = df;
	switch (df) {
	case FORMAT_XML:
		mManifestData.fileExtension = ".xml";
		break;
	case FORMAT_PLAIN_TEXT:
		mManifestData.fileExtension = ".txt";
		break;
	case FORMAT_HTML:
		mManifestData.fileExtension = ".html";
		break;
	}
}

void CManifest::open(string filename) {
	mManifestData.file.open(filename, ios::out | ios::trunc);

	if (!mManifestData.file.is_open()) {
		cout << "Error: Could not open '" << filename << "' for writing"
				<< endl;
		exit (EXIT_SUCCESS);
	}
}

void CManifest::close() {

	if (mManifestData.file.is_open()) {
		mManifestData.file.close();
	}
}

void CManifest::debug(string msg) {
	cout << msg << endl;
}
