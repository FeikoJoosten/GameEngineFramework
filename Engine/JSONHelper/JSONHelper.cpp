#include "JSONHelper.hpp"

#include "../FileSystem/file_system.hpp"
#include "../engine.hpp"
#include "rapidjson/prettywriter.h"
#include <iostream>

namespace Common
{
rapidjson::Document JsonHelper::OpenJSON(std::string const& filePath)
{
	FileSystems::FileSystem& fs = Engine::GetFileSystem();
	FileSystems::File& jsonFile = fs.LoadFile(filePath);

	std::vector<u8> data(static_cast<size_t>(jsonFile.Size()) + 1);
	jsonFile.ReadData(data, jsonFile.Size());

	rapidjson::StringStream stream(reinterpret_cast<char const *const>(data.data()));
	rapidjson::Document document;
	document.ParseStream(stream);

	jsonFile.Close();

	return document;
}

rapidjson::Document JsonHelper::WriteToJson(std::string const filePath, rapidjson::Document& document,
	bool destroyOldJson)
{
	rapidjson::Document oldDocument = OpenJSON(filePath);

	if(destroyOldJson)
	{
		oldDocument.RemoveAllMembers();
	}

	MergeDocuments(oldDocument, document);

	FileSystems::FileSystem& fs = Engine::GetFileSystem();
	FileSystems::File& jsonFile = fs.LoadFile(filePath, FileSystems::File::OpenMode::write);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	oldDocument.Accept(writer);

	jsonFile.WriteString(buffer.GetString());

	return oldDocument;
}

void JsonHelper::MergeDocuments(rapidjson::Document& originalDocument,
	rapidjson::Document& documentToAdd, bool overwriteValues)
{
	rapidjson::Document::AllocatorType& allocator = originalDocument.GetAllocator();

	if(!originalDocument.ObjectEmpty())
	{
		for(rapidjson::Value::MemberIterator it = originalDocument.MemberBegin(), end = originalDocument.MemberEnd(); it != end; ++it)
		{
			rapidjson::Value first(it->name.GetString(), allocator);
			rapidjson::Value second;
			second.CopyFrom(it->value, allocator);

			if(overwriteValues)
			{
				if(!documentToAdd[first].IsNull())
				{
					second.CopyFrom(documentToAdd[first], allocator);
				}
			}

			documentToAdd.EraseMember(first);

			originalDocument[first].Swap(second);
		}
	}

	for(rapidjson::Document::MemberIterator it = documentToAdd.MemberBegin(), end = documentToAdd.MemberEnd(); it != end; ++it)
	{
		originalDocument.AddMember(it->name, it->value, allocator);
	}
}
} // namespace Common