#include"ini.h"

#include"external_file.h"


namespace Ini
{
	void StringData::Set(const char* ext_str)
	{
		str = ext_str;
		HashVal = StrHash(ext_str);
	}
	void StringData::Rehash()
	{
		HashVal = StrHash(str);
	}
	StringData::StringData(const char* ext_str)
	{
		Set(ext_str);
	}
	StringData& StringData::operator=(const StringData& rhs)
	{
		str = rhs.str;
		HashVal = rhs.HashVal;
		return *this;
	}
	
	bool IniFile::Load(const char* fname)
	{
		using namespace std;
		IniName.Set(fname);
		ExtFileClass sf;
		sf.Open(fname, "r");
		if ( !sf.Available() || sf.Eof()) return Loaded = false;
		BufString buf0 = { 0 };
		char* buf = buf0;
		FileSec[StrHash("")] = IniSection("");
		hash_t cur_sec = StrHash("");
		char* cmt_start_pos = NULL;
		while (!sf.Eof())
		{
			bool has_cmt = true;
			memset(buf0, 0, sizeof(buf0));
			sf.GetStr(buf0, MAX_LINE_LENGTH);
			buf = buf0;
			buf = RemoveFrontSpace(buf0);//SECTION / KEY 均如此处理 
			RemoveBackSpace(buf);
			int buf_len = strlen(buf);
			if (buf_len < 2)continue;
			cmt_start_pos = strchr(buf, ';');
			if (cmt_start_pos == NULL)
			{
				cmt_start_pos = buf + buf_len;
				has_cmt = false;
			}
			*cmt_start_pos = 0;//暂时截断 
			if (cmt_start_pos == buf)//;COMMENT
			{
				if (has_cmt)*cmt_start_pos = ';';
				continue;
			}
			if (*buf == '[')//[SECTION]
			{
				char* ttp = strchr(buf, ']');
				if (ttp != NULL)
				{
					*ttp = 0;
					StringData tmp_label;
					tmp_label.Set(buf + 1);
					FileSec[tmp_label.HashVal] = IniSection(tmp_label);
					cur_sec = tmp_label.HashVal;
					if (has_cmt)*cmt_start_pos = ';';
					SectionSequence.push_back(tmp_label.HashVal);
					*ttp = ']';
					continue;
				}
			}
			if (strchr(buf, '=') == NULL)//dhsfjkhdsj;COMMENT  or  sdhfjdhsdjf
			{
				if (has_cmt)*cmt_start_pos = ';';
				continue;
			}
			{//a=b;COMMENT
				IniLine tmp_l;
				char* a = new BufString, * b = new BufString;
				sscanf(buf, " %[^=]= %[^;]", a, b);
				RemoveBackSpace(a);
				RemoveBackSpace(b);
				tmp_l.Key.Set(a); tmp_l.Value.Set(b);
				FileSec[cur_sec].SecStr[tmp_l.GetHash()] = tmp_l;
				hash_t htp = tmp_l.GetHash();
				FileSec[cur_sec].KeySequence.push_back(htp);
				if (has_cmt)*cmt_start_pos = ';';
				delete[] a; delete[] b;
				continue;
			}
		}
		sf.Close();
		return Loaded = true;
	}
}














