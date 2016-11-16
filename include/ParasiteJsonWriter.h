/**
 *
 *    @file  ParasiteJsonWriter.h
 *   @brief  Definitions for the class `ParasiteJsonWriter`
 *
 *    @date  01/08/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef PARASITE_JSON_WRITER_H_
#define PARASITE_JSON_WRITER_H_

// #include "rapidjson/document.h"
// #include "rapidjson/filewritestream.h"
// #include "rapidjson/prettywriter.h"
// #include "rapidjson/writer.h"
// #include "rapidjson/stringbuffer.h"
#include <cstdio>
#include <iostream>
#include <string>

#include "CallSiteProfile.h"
#include "ParasiteProfile.h"

class ParasiteJsonWriter {

 public:
	ParasiteJsonWriter(std::string _name_);
	~ParasiteJsonWriter();
	void writeCallSite(std::shared_ptr<CallSiteProfile> profile);
	void writeOverallProfile(parasite_profile_t prof);

	FILE* file_pointer;

	// document is the root of a json message
    // rapidjson::Document fromScratch;

    // rapidjson::Value callSiteArray;

    std::string name;

    char writeBuffer[65536];

 private:
	// prevent generated functions --------------------------------------------
	ParasiteJsonWriter(const ParasiteJsonWriter&);
	ParasiteJsonWriter& operator=(const ParasiteJsonWriter&);
};

#endif /* PARASITE_JSON_WRITER_H_ */





