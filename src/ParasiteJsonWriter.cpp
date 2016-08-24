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

ParasiteJsonWriter::ParasiteJsonWriter() : callSiteArray(rapidjson::kArrayType) {
    fromScratch.SetObject();
}

ParasiteJsonWriter::~ParasiteJsonWriter() {

    rapidjson::Document::AllocatorType& allocator = fromScratch.GetAllocator();
    fromScratch.AddMember("call_site_array", callSiteArray, allocator);
    file_pointer = fopen("parasite_output.json", "w"); // non-Windows use "w"
    rapidjson::FileWriteStream output_stream(file_pointer,
                                             writeBuffer, 
                                             sizeof(writeBuffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(output_stream);
    fromScratch.Accept(writer);
	fclose(file_pointer);
}

void ParasiteJsonWriter::writeCallSite(std::shared_ptr<CallSiteProfile> profile) {

    rapidjson::Document::AllocatorType& allocator = fromScratch.GetAllocator();
    std::shared_ptr<call_site_profile_t> prof = profile->prof;
    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("call_site", static_cast<int>(prof->call_site), allocator);
    object.AddMember("count", prof->count, allocator);
    rapidjson::Value fun_sg((prof->function_signature).c_str(), allocator);
    object.AddMember("function_signature", fun_sg.Move(), allocator);
    object.AddMember("parallelism", prof->parallelism, allocator);
    object.AddMember("span", static_cast<uint64_t>(prof->span), allocator);
    object.AddMember("work", static_cast<uint64_t>(prof->work), allocator);
    object.AddMember("start", static_cast<uint64_t>(prof->start), allocator);
    object.AddMember("stop", static_cast<uint64_t>(prof->start +
                                                   prof->span), allocator);
    object.AddMember("lock_wait_time", 
                     static_cast<uint64_t>(prof->lock_wait_time), allocator);
    callSiteArray.PushBack(object, allocator);
}


void ParasiteJsonWriter::writeOverallProfile(parasite_profile_t prof) {

    rapidjson::Document::AllocatorType& allocator = fromScratch.GetAllocator();
    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("parallelism", prof.parallelism, allocator);
    object.AddMember("span", static_cast<uint64_t>(prof.span), allocator);
    object.AddMember("work", static_cast<uint64_t>(prof.work), allocator);
    object.AddMember("lock_wait_time", 
                     static_cast<uint64_t>(prof.lock_wait_time), allocator);
    fromScratch.AddMember("overall profile", object, allocator);
}