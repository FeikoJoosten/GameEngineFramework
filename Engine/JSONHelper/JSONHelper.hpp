#ifndef f6d15177_9675_4c62_8588_b72848b55add_HPP
#define f6d15177_9675_4c62_8588_b72848b55add_HPP

#pragma once

#include <string>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include "../api.hpp"
#include <vector>

namespace Common
{
class RTS_COMMON_API JsonHelper
{
public:
	/// <summary> Enter a full path towards the json folder location, and it'll return you a document. </summary>
	static rapidjson::Document OpenJSON(std::string const &filePath);

	static rapidjson::Document WriteToJson(std::string const filePath, rapidjson::Document& document, bool destroyOldJson = false);

	static void MergeDocuments(rapidjson::Document& originalDocument, rapidjson::Document& documentToAdd, bool overwriteValues = true);

private:
	friend class FileSystem;
	friend class FlatFileSystem;
};
}

#endif // f6d15177_9675_4c62_8588_b72848b55add_HPP
