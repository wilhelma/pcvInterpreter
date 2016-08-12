/**
 *
 *    @file  ParasiteJsonwriter.cpp
 *   @brief  Implementation file for the class `ParasiteJsonWriter`
 *
 *    @date  01/08/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */


#include "ParasiteJsonWriter.h"

ParasiteJsonWriter::ParasiteJsonWriter() {
	file_pointer = fopen("parasite_output.json", "w"); // non-Windows use "w"
}

ParasiteJsonWriter::~ParasiteJsonWriter() {
	fclose(file_pointer);
}

void ParasiteJsonWriter::writeCallSite(std::shared_ptr<CallSiteProfile> profile) {

	char writeBuffer[65536];
	rapidjson::FileWriteStream output_stream(file_pointer, writeBuffer, sizeof(writeBuffer));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(output_stream);

    std::shared_ptr<call_site_profile_t> prof = profile->prof;
    writer.StartObject();
    writer.Key("call_site");
    writer.Int(static_cast<int>(prof->call_site));
    writer.Key("count");
    writer.Int(prof->count);
    writer.Key("function_signature");
    writer.String(static_cast<std::string>(prof->function_signature).c_str());
    writer.Key("parallelism");
    writer.Double(prof->parallelism);
    writer.Key("span");
    writer.Uint64(static_cast<uint64_t>(prof->span));
    writer.Key("work");
    writer.Uint64(static_cast<uint64_t>(prof->work));
    writer.EndObject();
}

void ParasiteJsonWriter::writeOverallProfile(std::shared_ptr<parasite_profile_t> 
															  prof) {

	char writeBuffer[65536];
	rapidjson::FileWriteStream output_stream(file_pointer, writeBuffer, sizeof(writeBuffer));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(output_stream);

    writer.StartObject();
    writer.Key("lock_wait_time");
    writer.Uint64(static_cast<uint64_t>(prof->lock_wait_time));
    writer.Key("parallelism");
    writer.Double(prof->parallelism);
    writer.Key("span");
    writer.Uint64(static_cast<uint64_t>(prof->span));
    writer.Key("work");
    writer.Uint64(static_cast<uint64_t>(prof->work));
    writer.EndObject();
}