#pragma once
#include<cstdio>
#include<cstring>
#include<unordered_map>
#include<vector>

#include"..\Minimal.h"
#include"global_tool_func.h"

namespace Ini
{
	struct StringData
	{
		std::string str;
		hash_t HashVal;
		void Set(const char* ext_str);
		void Rehash();
		StringData() = default;
		StringData(const char* ext_str);
		StringData& operator=(const StringData& rhs);
	};

	struct IniLine
	{
		StringData Key;
		StringData Value;
		IniLine() = default;
		hash_t GetHash()
		{
			return Key.HashVal;
		}
	};

	typedef std::unordered_map<hash_t, IniLine>::iterator IniLineIter;

	struct IniSection
	{
	public:
		StringData SecName;
		std::unordered_map<hash_t, IniLine> SecStr;
		std::vector<hash_t>KeySequence;

		void SetName(const char* ext_str)
		{
			SecName.Set(ext_str);
		}
		void SetName(const StringData& ext_str)
		{
			SecName = ext_str;
		}
		IniSection() = default;
		IniSection(const char* ext_str)
		{
			SetName(ext_str);
		}
		IniSection(const StringData& ext_str)
		{
			SetName(ext_str);
		}
		hash_t GetNameHash()
		{
			return SecName.HashVal;
		}
		inline IniLineIter GetLine(hash_t KeyHash)
		{
			return SecStr.find(KeyHash);
		}
		inline bool HasKey(hash_t KeyHash)
		{
			return GetLine(KeyHash) == SecStr.end();
		}
		inline IniLineIter GetLine(const char* KeyHash)
		{
			return SecStr.find(StrHash(KeyHash));
		}
		inline bool HasKey(const char* KeyHash)
		{
			return GetLine(KeyHash) == SecStr.end();
		}
		inline IniLineIter GetLine(const std::string& KeyHash)
		{
			return SecStr.find(StrHash(KeyHash));
		}
		inline bool HasKey(const std::string& KeyHash)
		{
			return GetLine(KeyHash) == SecStr.end();
		}
		inline IniLineIter NotFound()
		{
			return SecStr.end();
		}
	};

	typedef std::unordered_map<hash_t, IniSection>::iterator IniSectionIter;

	struct IniFile
	{
		StringData IniName;
		StringData IniLabel;
		std::unordered_map<hash_t, IniSection> FileSec;
		std::vector<hash_t>SectionSequence;
		bool Loaded{ false };
		IniFile() = default;
		bool Available()
		{
			return Loaded;
		}
		bool Load(const char* fname);
		inline IniSectionIter GetSection(hash_t SecHash)
		{
			return FileSec.find(SecHash);
		}
		inline bool HasSection(hash_t SecHash)
		{
			return GetSection(SecHash) == FileSec.end();
		}
		inline IniSectionIter GetSection(const char* SecHash)
		{
			return FileSec.find(StrHash(SecHash));
		}
		inline bool HasSection(const char* SecHash)
		{
			return GetSection(SecHash) == FileSec.end();
		}
		inline IniSectionIter GetSection(const std::string& SecHash)
		{
			return FileSec.find(StrHash(SecHash));
		}
		inline bool HasSection(const std::string& SecHash)
		{
			return GetSection(SecHash) == FileSec.end();
		}
		inline IniSectionIter NotFound()
		{
			return FileSec.end();
		}
        void Clear()
        {
            FileSec.clear();
            SectionSequence.clear();
            IniName = {};
            IniLabel = {};
            Loaded = false;
        }
	};
}

