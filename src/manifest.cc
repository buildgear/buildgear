/*
 * manifest.cc
 *
 *  Created on: May 7, 2013
 *      Author: Torsten Lund
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include "buildgear/config.h"
#include "buildgear/manifest.h"

extern CBuildFiles BuildFiles;

CManifest::CManifest() {
	mManifestData.documentFormat = FORMAT_NOT_SUPPORTED;
	mManifestData.footer = "";
	mManifestData.header = "";
	mManifestData.fileExtension = "";
}

void CManifest::generateManifest(CBuildFile *buildfile) {
	switch (mManifestData.documentFormat) {
	case FORMAT_PLAIN_TEXT:
		generatePlainText();
		break;
	case FORMAT_XML:
		if (!buildfile) {
			generateXMLFull();
		}
		break;
	case FORMAT_HTML:
		generateHTML();
		break;
	case FORMAT_NOT_SUPPORTED:
	default:
		cout << "Error: Document format not supported." << endl;
		break;
	}
}

void CManifest::generateXMLFull() {
	int ret = 0;
	list<CBuildFile*>::iterator it;
	string temp;
	string native = "native";
	string cross = "cross";

	/*
	 * XML defined format
	 *
	 * <?xml version="1.0" encoding="UTF-8"?>
	 * <manifest>
	 *   <project name="" />
	 *   <native>
	 *     <project name="" version="" release="" license="" url="" description="" />
	 *   </native>
	 *   <cross>
	 *     <project name="" version="" release="" license="" url="" description="" />
	 *   </cross>
	 * </manifest>
	 */

	open(MANIFEST_FILE + mManifestData.fileExtension);

	cout << "Generating manifest..           " << flush;

	mManifestData.file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	mManifestData.file << "<manifest>" << endl;

	if (mManifestData.header != "") {
		mManifestData.file << "  <!-- " << mManifestData.header << " -->" << endl;
	}

	//cout << "  <project name=\"\" />" << endl; TODO: At some point add this field

	mManifestData.file << "  <native>" << endl;

	for (it = BuildFiles.buildfiles.begin(); it != BuildFiles.buildfiles.end();
			it++) {
		if ("native" == (*it)->type) {
			generateXMLProjectProperties((CBuildFile*) (*it));
		}
	}

	mManifestData.file << "  </native>" << endl;
	mManifestData.file << "  <cross>" << endl;

	for (it = BuildFiles.buildfiles.begin(); it != BuildFiles.buildfiles.end();
			it++) {
		if ("cross" == (*it)->type) {
			generateXMLProjectProperties((CBuildFile*) (*it));
		}
	}

	mManifestData.file << "  </cross>" << endl;

	if (mManifestData.footer != "") {
		mManifestData.file << "  <!-- " << mManifestData.footer << " -->" << endl;
	}

	mManifestData.file << "</manifest>" << endl;

	mManifestData.file.close();

	cout << "Done (Saved as " << MANIFEST_FILE + mManifestData.fileExtension << ")"<< endl << endl;
}

void CManifest::generateXMLProject(CBuildFile* file) {
	debug("HelloWorld");
	cout << file->name << endl;
}

void CManifest::generateXMLProjectProperties(CBuildFile* file) {
	mManifestData.file  << "    <project name=\"" << file->short_name << "\"";

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

void CManifest::generatePlainText() {
}

void CManifest::generateHTML() {
}

void CManifest::setFooter(string footer) {
	mManifestData.footer = footer;
}

void CManifest::setHeader(string header) {
	mManifestData.header = header;
}

void CManifest::setFormat(DocumentFormat df) {
	mManifestData.documentFormat = df;
	switch(df) {
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

	if(!mManifestData.file.is_open()) {
		cout << "Error: Could not open '" << filename << "' for writing"<< endl;
		exit(EXIT_SUCCESS);
	}
}

void CManifest::close() {

	if(mManifestData.file.is_open()) {
		mManifestData.file.close();
	}
}

void CManifest::debug(string msg) {
	cout << msg << endl;
}
