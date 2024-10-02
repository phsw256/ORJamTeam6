#pragma once

#include "Include.h"
#include "ORTimer.h"
#include "Version.h"
#include "Global.h"


int ExitUninit();

namespace PreLink
{
    GLFWwindow* window;
    ImVec4 clear_color = ImVec4(0.67f, 0.33f, 0.0f, 1.00f);
    //HICON ProgIcon;
    HINSTANCE hInst;
    ImFont* font;

    void CleanUp();
}


namespace GlobalAnalyze
{
	typedef int (*cmddeal_t)(int, const std::vector<std::string>&);
	bool HasOutput{ false };

	std::vector<std::string>OutputText;
	std::mutex OTMutex;
	std::default_random_engine rnd;

	std::string CutSpace(const std::string& ss)//REPLACE ORIG
	{
		auto fp = ss.find_first_not_of(" \011\r\n\t"), bp = ss.find_last_not_of(" \011\r\n\t");
		return std::string(ss.begin() + (fp == ss.npos ? 0 : fp),
			ss.begin() + (bp == ss.npos ? 0 : bp + 1));
	}
	std::string CutComment(const std::string& ss)//ORIG
	{
		auto fp = ss.find_first_of(';');
		return std::string(ss.begin(), ss.begin() + (fp == ss.npos ? ss.length() : fp));
	}
	std::vector<std::string>LoadFileStr(const std::string& File)
	{
		std::vector<std::string>Ret;
        ExtFileClass GetFileStr;
		if (GetFileStr.Open(File.c_str(), "r"))
		{
			BufString str;
			while (!GetFileStr.Eof())
			{
                GetFileStr.GetStr(str, MAX_STRING_LENGTH);
				Ret.push_back(CutSpace(CutComment(str)));
			}
            GetFileStr.Close();
		}
		if (Ret.empty())return Ret;
		Ret.erase(Ret.begin());
		return Ret;
	}
	std::vector<std::string> SplitParam(const std::string& Text)//ORIG
	{
		if (Text.empty())return {};
		size_t cur = 0, crl;
		std::vector<std::string> ret;
		while ((crl = Text.find_first_of(',', cur)) != Text.npos)
		{
			ret.push_back(CutSpace(std::string(Text.begin() + cur, Text.begin() + crl)));
			cur = crl + 1;
		}
		ret.push_back(CutSpace(std::string(Text.begin() + cur, Text.end())));
		return ret;
	}

	void Warning(const char* Str)
	{
		OTMutex.lock();
		OutputText.push_back(Str);
		OTMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(FnInfoDelay));
		HasOutput = true;
	}
	std::string RandStr(int i)
	{
		static const char Pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
		std::string ss; ss.reserve(i + 1);
		for (int p = 0; p < i; p++)ss.push_back(Pool[rnd() % 62]);
		return ss;
	}

	std::vector<std::string>FileName;//文件结构 
	std::vector<int>FileCmdLOffset;
	std::vector<int>FileRealLOffset;
	std::vector<int>CmdLBelongFile;
	std::vector<int>RealLBelongFile;
	int GlobalCursor;
	std::vector<std::string>GlobalFileStr;




	int Begin(int, const std::vector<std::string>&);//函数列表 
	int End(int, const std::vector<std::string>&);
	int Debug(int, const std::vector<std::string>&);//#EXPR #DEFAULT
	int SetValue(int, const std::vector<std::string>&);
	int Label(int, const std::vector<std::string>&);
	int LabelJump(int, const std::vector<std::string>&);//#ALL #THIS
	int ImportFile(int, const std::vector<std::string>&);
	int LongCall(int, const std::vector<std::string>&);//#ALL #THIS
	int CmdLineJump(int, const std::vector<std::string>&);
	int Nothing(int, const std::vector<std::string>&);
	int PrintTo(int, const std::vector<std::string>&);//#EXPR 拆序列 
	int LPrintTo(int, const std::vector<std::string>&);//#EXPR 拆序列 + @CO
	int Run(int, const std::vector<std::string>&);//#EXPR 拆序列 
	int Expression(int, const std::vector<std::string>&);//#PAR<n> #EXPR (内)#PARAM__ 拆序列 
	int RunExpr(int, const std::vector<std::string>&);//#EXPR #EXPC #EXPCI #EXPCR #DEFAULT #MAKEEXPR #EXPREND
	int Bind(int, const std::vector<std::string>&);//#PAR<n> #SAME:复制Iter #FULL:不复制Iter #AFTER:[Cur,End) #BEFORE:[Begin,Cur) (内)#PARAM__
	int SetExprIter(int, const std::vector<std::string>&);
	int LabelJumpIf(int, const std::vector<std::string>&);//#ALL #THIS #CONTINUE
	int Link(int, const std::vector<std::string>&);//#ON #OFF 
	int Raw(int, const std::vector<std::string>&);

	std::unordered_map<std::string, cmddeal_t> CommandMap
	{
		{u8"Function",Begin},{u8"Return",End},{u8"Debug",Debug},
		{u8"SetValue",SetValue},{u8"Call",LongCall},{u8"SetExprIter",SetExprIter},
		{u8"LabelJump",LabelJump},{u8"CmdLineJump",CmdLineJump},{u8"Label",Label},
		{u8"Import",ImportFile},{u8"Nop",Nothing},{u8"Bind",Bind},
		{u8"PrintTo",PrintTo},{"LPrintTo",LPrintTo},{u8"Run",Run},
		{u8"Expression",Expression},{u8"RunExpr",RunExpr},{u8"LabelJumpIf",LabelJumpIf},
		{u8"Link",Link},
		//{"Delete",DeleteData}

		{u8"Expr",Expression},{u8"Ret",End},{u8"LblJmp",LabelJump},
		{u8"Func",Begin},{u8"SetV",SetValue},{u8"ExprIt",SetExprIter},
		{u8"CmdLJmp",CmdLineJump},{"PrTo",PrintTo},{u8"LPrTo",LPrintTo},
		{u8"BindExpr",Bind},{u8"LblJmpIf",LabelJumpIf},{u8"LblJIf",LabelJumpIf}
	};



	struct CmdLineInfo//标签、指令管理 
	{
		cmddeal_t Func;
		std::vector<std::string> Param;
		int RealLineNo;
		bool Same(const CmdLineInfo& r)const { return RealLineNo == r.RealLineNo && Param == r.Param && Func == r.Func; }
		bool EqualEffect(const CmdLineInfo& r)const { return Func == r.Func && Param == r.Param; }
		bool EqualFirst(const CmdLineInfo& r)const{ return Func == r.Func && ((Param.empty() && r.Param.empty()) || Param[0] == r.Param[0]); }
		bool EqualFunc(const CmdLineInfo& r)const { return Func == r.Func; }
		bool LineLess(const CmdLineInfo& r)const { return RealLineNo < r.RealLineNo; }
	};
	std::vector<CmdLineInfo>GlobalCommands;
	std::vector<CmdLineInfo>GetValidLines(const std::vector<std::string>& vec)
	{
		static char Fname[MAX_STRING_LENGTH], Param[MAX_STRING_LENGTH];
		std::vector<CmdLineInfo> ret;
		int lp = 1;
		for (auto& str : vec)
		{
			std::string ts = CutSpace(str);
			++lp;
			if (ts == "")continue;
			memset(Fname, 0, sizeof(Fname));
			memset(Param, 0, sizeof(Param));
			std::string Tmp1 = CutSpace(CutComment(ts));
			sscanf(Tmp1.c_str(), "%[^ (] (%[^)])%*s", Fname, Param);
			std::string FnN = CutSpace(Fname), Par = CutSpace(Param);
			if (FnN == "Raw")
			{
				auto fp = Tmp1.find_first_of('(');
				auto fl = Tmp1.find_last_of(')');
				auto fs = Tmp1.find_first_of(',',fp);
				if (fp == Tmp1.npos || fl == Tmp1.npos || fs == Tmp1.npos)continue;
				ret.push_back({ Raw,{
					CutSpace(std::string(Tmp1.begin() + fp + 1, Tmp1.begin() + fs)),
					CutSpace(std::string(Tmp1.begin() + fs + 1, Tmp1.begin() + fl))},lp });
				continue;
			}
			auto it = CommandMap.find(FnN);
			if (it != CommandMap.end())
			{
				ret.push_back({ it->second,SplitParam(Par),lp });
			}
			else
			{
				sprintf(Fname, u8"语法错误：第%d行 未知函数\"%s\"", lp, FnN.c_str());
				Warning(Fname);
			}
		}
		return ret;
	}
	int FindLabelPos(const CmdLineInfo& line)
	{
		auto it = find_if(GlobalCommands.begin(), GlobalCommands.end(),
			[&line](decltype(line)& fl) {return fl.EqualFirst(line); });
		if (it == GlobalCommands.end())return -1;
		return std::distance(GlobalCommands.begin(), it);
	}
	int FindLabelPos(int from, int to, const CmdLineInfo& line)
	{
		auto Final = ((to == -1) ? GlobalCommands.end() : GlobalCommands.begin() + to);
		auto it = find_if(GlobalCommands.begin() + from, Final,
			[&line](decltype(line)& fl) {return fl.EqualFirst(line); });
		if (it == Final)return -1;
		return std::distance(GlobalCommands.begin(), it);
	}
	int CmdLine()
	{
		return GlobalCursor - FileCmdLOffset[CmdLBelongFile[GlobalCursor]];
	}
	int RealLine()
	{
		return GlobalCommands[GlobalCursor].RealLineNo;
	}

	struct CallSubInfo//调用栈 
	{
		std::string CallName;
		std::unordered_map<std::string, std::string>ParamMap;
	};
	std::stack<CallSubInfo>CallStack;
	std::stack<int>RetStack;
	std::stack<std::vector<std::string> >LocalStack;
	std::vector<std::string> CallOrder;


	//变量列表 
	//最后加u8
    std::unordered_map < std::string, std::string > GlobalConst =
    {
        {u8"，",u8","},{u8"EM",u8""},{u8"CO",u8","},{u8"LN",u8"\n"},
        {u8"\\n",u8"\n"},{u8"AT",u8"@"},{u8"VERSION",VersionNStr},
        {u8"TRUE",u8"1"},{u8"FALSE",u8"0"},
    };
    std::unordered_map < std::string, std::string > GlobalEnviron;
	//@FUNCTION @FILE @CMDLINE @FILELINE @EM R@x:"@x" R~@x:"~@x" P@x引用形参 V@x:传值x L@x：局部变量x
	std::string GlobalDecryptString(const std::string& Key)
	{
		//printf("GlobalDecryptString Key=\"%s\"\n",Key.c_str());//OUT
		if (Key.empty())return "";
		if (Key == "@FILE")
		{
			return FileName[CmdLBelongFile[GlobalCursor]];
		}
		if (Key == "@CMDLINE")
		{
			return std::to_string(CmdLine());
		}
		if (Key == "@FILELINE")
		{
			return std::to_string(RealLine());
		}
		if (Key.size() > 1 && Key[0] == 'R' && Key[1] == '@')
		{
			return std::string(Key.begin() + 1, Key.end());
		}
		if (Key.size() > 2 && Key[0] == 'R' && Key[2] == '@')
		{
			return std::string(Key.begin() + 1, Key.end());
		}
		if (Key.size() > 1 && Key[0] == 'P' && Key[1] == '@')
		{
			auto it = CallStack.top().ParamMap.find(std::string(Key.begin() + 2, Key.end()));
			if (it == CallStack.top().ParamMap.end())return "";
			else return GlobalDecryptString(it->second);
		}
		if (Key.size() > 1 && Key[0] == 'L' && Key[1] == '@')
		{
			std::string ns = std::to_string(RetStack.top()) + CallStack.top().CallName + std::string(Key.begin() + 2, Key.end());
			if (GlobalEnviron.find(ns) == GlobalEnviron.end())LocalStack.top().push_back(ns);
			return GlobalDecryptString("@" + ns);
		}
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')su = GlobalDecryptString(su);
			}
			if (su == "")return"";

			auto It = GlobalEnviron.find(su);
			if (It != GlobalEnviron.end())return It->second;

            auto It2 = GlobalConst.find(su);
            if (It2 != GlobalConst.end())return It2->second;

			return GlobalEnviron[su];
		}
		return Key;
	}
	bool GlobalDecryptStringHasThis(const std::string& Key)
	{
		//printf("GlobalDecryptString Key=\"%s\"\n",Key.c_str());//OUT
		if (Key.empty())return false;
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')
				{
					if (!GlobalDecryptStringHasThis(su))return false;
					su = GlobalDecryptString(su);
				}
			}
			if (su.empty())return false;

			auto It = GlobalEnviron.find(su);
			if (It != GlobalEnviron.end())return true;

            auto It2 = GlobalConst.find(su);
            if (It2 != GlobalConst.end())return true;

			return false;
		}
		else return false;
	}
	std::string GlobalEncryptStringTarget(const std::string& Key)
	{
		if (Key.empty())return "";
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')su = GlobalDecryptString(su);
			}
			return su;
		}
		else if (Key.size() >= 2 && Key[1] == '@')
		{
			if (Key.size() > 1 && Key[0] == 'P' && Key[1] == '@')
			{
				auto it = CallStack.top().ParamMap.find(std::string(Key.begin() + 2, Key.end()));
				if (it != CallStack.top().ParamMap.end())GlobalEncryptStringTarget(it->second);
			}
			if (Key.size() > 1 && Key[0] == 'L' && Key[1] == '@')
			{
				std::string ns = std::to_string(RetStack.top()) + CallStack.top().CallName + std::string(Key.begin() + 2, Key.end());
				if (GlobalEnviron.find(ns) == GlobalEnviron.end())LocalStack.top().push_back(ns);
				GlobalEncryptStringTarget("@" + ns);
			}
		}
		return "";
	}
	void GlobalEncryptRawString(const std::string& Key, const std::string& Val)
	{
		std::string su = GlobalEncryptStringTarget(Key);
		if (su.empty())return;
		GlobalEnviron[su] = Val;
	}
	void GlobalEncryptString(const std::string& Key, const std::string& Val)
	{
		GlobalEncryptRawString(Key,GlobalDecryptString(Val));
	}
	
 
	bool IsTrueString(const std::string& s1)
	{
		return s1 == "true" || s1 == "yes" || s1 == "1" || s1 == "T" || s1 == "True" || s1 == "Yes" || s1 == "t" || s1 == "Y" || s1 == "y";
	}

	struct ExprInfo
	{
		std::vector<std::string> Expr;
		std::vector<int> Replace;
		int ExprIter;
		int MaxParam;
        int LineNo;
	};
	std::unordered_map<std::string, ExprInfo> ExprMap;
	bool HasExpr(const std::string& Name)
	{
		return ExprMap.find(Name) != ExprMap.end();
	}
	typedef std::vector<std::string>::const_iterator __vsit;
    const std::unordered_map<std::string, int> CalcNum
	{
		//零元 
		{"#T",0},{"#F",0},{"#FRI",0},
		//一元 
		{"#RI",1},{"#RS",1},{"#SP",1},
        {"#!",1},{"#NOT",1},
		{"#SB",1},{"#IB",1},{"#-I",1},{"#NI",1},
		{"#~I",1},{"#CI",1},
		{"#SL",1},{"#SR",1},{"#NOP",1},
		{"#ELEN",1},{"#EMAXP",1},{"#EIT",1},
        {"#STR",1},{"#CSFSTR",1},
        {"#INCUR",1},
		//二元 
		{"#S=",2},{"#SE",2},{"#I=",2},{"#IE",2},
		{"#S!=",2},{"#SNE",2},{"#I!=",2},{"#INE",2},
		{"#S>",2},{"#SG",2},{"#I>",2},{"#IG",2},
		{"#S>=",2},{"#SGE",2},{"#I>=",2},{"#IGE",2},
		{"#S<",2},{"#SL",2},{"#I<",2},{"#IL",2},
		{"#S<=",2},{"#SLE",2},{"#I<=",2},{"#ILE",2},
		{"#I+",2},{"#I-",2},{"#I*",2},{"#I/",2},{"#I%",2},
		{"#IA",2},{"#IS",2},{"#IM",2},{"#ID",2},{"#IU",2},
		{"#I&",2},{"#I|",2},{"#I^",2},
		{"#IN",2},{"#IO",2},{"#IX",2},
		{"#S+",2},{"#SA",2},{"#SCF",2},{"#SCB",2},
		{"#SRF",2},{"#SRP",2},
		{"#ININI",2},
        {"#&&",2},{"#||",2},{"#AND",2},{"#OR",2},

		//三元特判（就这一个） 
		{"#?:",3},{"#CT",3},
	};
	const std::unordered_map<std::string, std::function<std::string(const std::string&, const std::string&)> > CalcMap
	{
		//零元 
		{"#T",[](const std::string&,const std::string&)->std::string {return "1"; }},
		{"#F",[](const std::string&,const std::string&)->std::string {return "0"; }},
		{"#FRI",[](const std::string&,const std::string&)->std::string {return std::to_string(rnd()); }},

		//一元 
		{"#RI",[](const std::string& s1,const std::string&)->std::string
			{
				int md = atoi(s1.c_str());
				if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #RI：试图对0取模").c_str()); return "0"; }
				return std::to_string(rnd() % md);
			}},
        {"#RS",[](const std::string& s1,const std::string&)->std::string {return RandStr(atoi(s1.c_str())); }},
        {"#SP",[](const std::string& s1,const std::string&)->std::string {return std::string((int)atoi(s1.c_str()),' '); }},
		{"#NOT",[](const std::string& s1,const std::string&)->std::string {return std::to_string(!atoi(s1.c_str())); }},
		{"#!",[](const std::string& s1,const std::string&)->std::string {return std::to_string(!atoi(s1.c_str())); }},
		{"#SB",[](const std::string& s1,const std::string&)->std::string{return std::to_string((int)IsTrueString(s1)); }},
		{"#IB",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(atoi(s1.c_str()) != 0)); }},
		{"#NI",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(-atoi(s1.c_str()))); }},
		{"#-I",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(-atoi(s1.c_str()))); }},
		{"#CI",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(~atoi(s1.c_str()))); }},
		{"#~I",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(~atoi(s1.c_str()))); }},
		{"#SL",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(s1.length())); }},
		{"#SR",[](const std::string& s1,const std::string&)->std::string {auto s2 = s1; reverse(s2.begin(),s2.end()); return s2; }},
		{"#ELEN",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].Expr.size()) : "0"; }},
		{"#EEND",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].Expr.size() + 1) : "0"; }},
		{"#EMAXP",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].MaxParam) : "0"; }},
		{"#EIT",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].ExprIter + 1) : "0"; }},
		{"#NOP",[](const std::string& s1,const std::string&)->std::string {return s1; }},
                //二元 
        {"#S=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 == s2)); }},
        {"#SE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 == s2)); }},
        {"#I=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) == atoi(s2.c_str()))); }},
        {"#IE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) == atoi(s2.c_str()))); }},
        //
        {"#S!=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 != s2)); }},
        {"#SNE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 != s2)); }},
        {"#I!=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) != atoi(s2.c_str()))); }},
        {"#INE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) != atoi(s2.c_str()))); }},
        //
        {"#S>",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 > s2)); }},
        {"#SG",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 > s2)); }},
        {"#I>",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) > atoi(s2.c_str()))); }},
        {"#IG",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) > atoi(s2.c_str()))); }},
        //
        {"#S<",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 < s2)); }},
        {"#SL",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 < s2)); }},
        {"#I<",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) < atoi(s2.c_str()))); }},
        {"#IL",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) < atoi(s2.c_str()))); }},
        //
        {"#S>=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 >= s2)); }},
        {"#SGE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 >= s2)); }},
        {"#I>=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) >= atoi(s2.c_str()))); }},
        {"#IGE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) >= atoi(s2.c_str()))); }},
        //
        {"#S<=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 <= s2)); }},
        {"#SLE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 <= s2)); }},
        {"#I<=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) <= atoi(s2.c_str()))); }},
        {"#ILE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) <= atoi(s2.c_str()))); }},
        //
        {"#I+",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) + atoi(s2.c_str()))); }},
        {"#IA",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) + atoi(s2.c_str()))); }},
        {"#I-",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) - atoi(s2.c_str()))); }},
        {"#IS",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) - atoi(s2.c_str()))); }},
        {"#I*",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) * atoi(s2.c_str()))); }},
        {"#IM",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) * atoi(s2.c_str()))); }},
        {"#I/",[](const std::string& s1,const std::string& s2)->std::string
            {
                int md = atoi(s2.c_str());
                if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #I/：试图除以0").c_str()); return "0"; }
                return std::to_string((atoi(s1.c_str()) / md));
            }},
        {"#ID",[](const std::string& s1,const std::string& s2)->std::string
            {
                int md = atoi(s2.c_str());
                if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #ID：试图除以0").c_str()); return "0"; }
                return std::to_string((atoi(s1.c_str()) / md));
            }},
        {"#I%",[](const std::string& s1,const std::string& s2)->std::string
            {
                 int md = atoi(s2.c_str());
                 if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #I%：试图对0取模").c_str()); return "0"; }
                 return std::to_string((atoi(s1.c_str()) % md));
             }},
        {"#IU",[](const std::string& s1,const std::string& s2)->std::string
             {
                 int md = atoi(s2.c_str());
                 if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #IU：试图对0取模").c_str()); return "0"; }
                 return std::to_string((atoi(s1.c_str()) % md));
             }},
                 //
        {"#I&",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) & atoi(s2.c_str()))); }},
        {"#IN",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) & atoi(s2.c_str()))); }},
        {"#I|",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) | atoi(s2.c_str()))); }},
        {"#IO",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) | atoi(s2.c_str()))); }},
        {"#&&",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) & atoi(s2.c_str()))); } },
        {"#AND",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) & atoi(s2.c_str()))); } },
        {"#||",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) | atoi(s2.c_str()))); } },
        {"#OR",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) | atoi(s2.c_str()))); } },
        {"#I^",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) ^ atoi(s2.c_str()))); }},
        {"#IX",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) ^ atoi(s2.c_str()))); }},
        //
        {"#S+",[](const std::string& s1,const std::string& s2)->std::string {return s1 + s2; }},
        {"#SA",[](const std::string& s1,const std::string& s2)->std::string {return s1 + s2; }},
        {"#SCF",[](const std::string& s1,const std::string& s2)->std::string
        {
            int at = atoi(s2.c_str());
            if (at >= (int)s1.length()) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCF：字符串长度上溢").c_str()); return ""; }
            if (at < 0) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCF：字符串长度下溢").c_str()); return ""; }
            return std::string(s1.begin() + at,s1.end());
        }},
        {"#SCB",[](const std::string& s1,const std::string& s2)->std::string
        {
            int at = atoi(s2.c_str());
            if (at >= (int)s1.length()) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCB：字符串长度上溢").c_str()); return ""; }
            if (at < 0) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCB：字符串长度下溢").c_str()); return ""; }
            return std::string(s1.begin(),s1.begin() + s1.length() - at);
        }},
        {"#SRF",[](const std::string& s1,const std::string& s2)->std::string
            {
                int res;
                //printf("Regex Match: Module=\"%s\" std::string=\"%s\"\n",s2.c_str(),s1.c_str());
                try { res = (int)std::regex_match(s1, std::regex(s2)); }
                catch (std::exception& e) { (void)e; Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SRF：正则表达式非法").c_str()); return "0"; }
                return std::to_string(res);
            }},
        {"#SRP",[](const std::string& s1,const std::string& s2)->std::string
            {
                int res;
                //printf("Regex Search: Module=\"%s\" std::string=\"%s\"\n",s2.c_str(),s1.c_str());
                try { res = (int)std::regex_search(s1, std::regex(s2)); }
                catch (std::exception& e) { (void)e; Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SRP：正则表达式非法").c_str()); return "0"; }
                return std::to_string(res);
            } },

        {"#?:",[](const std::string& s1,const std::string&)->std::string {return s1; }},//三元特判
        {"#CT",[](const std::string& s1,const std::string&)->std::string {return s1; }},//三元特判
	};

	std::string CallCalculateExpr(const std::vector<std::string>& Param);
	std::pair<std::string, __vsit> CalculateForward(__vsit Cur, __vsit End)
	{
		if (Cur == End)return { "",End };
		std::string opr = *Cur;
		Cur++;
		if (opr == "#EXPC" || opr == "#EXPCI" || opr == "#EXPCR")
		{
			auto af = CalculateForward(Cur, End);
			Cur = af.second;
			std::string ExprName = af.first;
			af = CalculateForward(Cur, End);
			Cur = af.second;
			int Rx;
			if (af.first == "#DEFAULT")Rx = HasExpr(ExprName) ? ExprMap[ExprName].MaxParam : 0;
			else Rx = atoi(af.first.c_str());

			if (HasExpr(ExprName))
			{
				auto& info = ExprMap[ExprName];
				if (opr == "#EXPCR")info.ExprIter = 0;
				else if (opr == "#EXPCI")
				{
					auto ag = CalculateForward(Cur, End);
					Cur = ag.second;
					info.ExprIter = atoi(ag.first.c_str()) - 1;
				}
			}

			std::vector<std::string> Param{ "",ExprName };
			for (int i = 0; i < Rx; i++)
			{
				auto ap = CalculateForward(Cur, End);
				Cur = ap.second;
				Param.push_back(ap.first);
			}
			return { CallCalculateExpr(Param),Cur };
		}
		if (opr == "#MAKEEXPR")
		{
			std::string Name = RandStr(16);
			std::vector<std::string> Par{ Name };
			--Cur;
			while(1)
			{
				++Cur;
				if (Cur == End)break;
				//Warning(("AP.FIRST : \"" + *Cur + "\"").c_str());
				//ImGui::TextWrapped(("AP.FIRST : \"" + *Cur + "\"").c_str());
				if (*Cur != "#EXPREND")
				{
					//for (auto& ss : SplitParam(*Cur))Par.push_back(ss);
					Par.push_back(*Cur);
				}
				else break;
			}
			//*
			for (size_t i = 0; i < Par.size(); i++)
			{
				//ImGui::TextWrapped(("##PAR THIS : " + std::to_string(i) + " \"" + Par[i] + "\"").c_str());
			}//*/
			Expression(0, Par);
			return { Name,Cur };
		}
		std::string p1, p2;
        //ImGui::Text("OPR=%s", opr.c_str());
		auto nit = CalcNum.find(opr);
		auto ncm = CalcMap.find(opr);
		if (nit == CalcNum.end())//数据
		{
            //ImGui::Text("IS DATA");
            if (opr.size() >= 2 && opr[0] == 'T' && opr[1] == '@')opr = GlobalDecryptString(opr);
			return { opr,Cur };
		}
		//printf("IS OPR PARAM N=%d\n",nit->second);
		if (nit->second == 0)//零元 
		{
			std::string al = ncm->second("","");
            //ImGui::Text("RESULT=%s",al.c_str());
			return { al,Cur };
		}
		if (nit->second == 1)//一元 
		{
			auto as = CalculateForward(Cur, End);
            //ImGui::Text("PARAM 1=%s",as.first.c_str());
			std::string al = ncm->second(as.first, "");
			//ImGui::Text("RESULT=%s",al.c_str());
			return { al,as.second };
		}
		if (nit->second == 2)//二元 
		{
			auto af = CalculateForward(Cur, End);
			Cur = af.second;
			auto as = CalculateForward(Cur, End);
			//ImGui::Text("PARAM 1=%s PARAM 2=%s",af.first.c_str(),as.first.c_str());
			std::string al = ncm->second(af.first, as.first);
			//ImGui::Text("RESULT=%s",al.c_str());
			return { al,as.second };
		}
		if (nit->second == 3)//三元特判 
		{
			auto af = CalculateForward(Cur, End);
			Cur = af.second;
			auto as = CalculateForward(Cur, End);
			Cur = as.second;
			auto au = CalculateForward(Cur, End);
			//ImGui::Text("PARAM 1=%s PARAM 2=%s PARAM 3=%s",af.first.c_str(),as.first.c_str(),au.first.c_str());
			std::string al = ncm->second((atoi(af.first.c_str()) ? as.first : au.first), "");
			//ImGui::Text("RESULT=%s",al.c_str());
			return { al,au.second };
		}
		//算完，后面忽略 
		return { "",End };
	}
	std::string CalculateExpr(int& Cursor, const std::vector<std::string>& Expr)
	{
		if (Cursor < 0)Cursor = 0;
		if (Cursor >= (int)Expr.size())Cursor = 0;
		auto It = Expr.begin() + Cursor;
		auto pr = CalculateForward(It, Expr.end());
		Cursor = std::distance(Expr.begin(), pr.second);
		return pr.first;
	}
	std::string CallCalculateExpr(const std::vector<std::string>& Param)
	{
        std::string st = GlobalDecryptString(Param.at(1));
		auto& info = ExprMap[st];
		std::vector<std::string> DParam = info.Expr;
		for (size_t i = 0; i < DParam.size(); i++)
		{
			if (DParam.at(i) == "#PARAM__")
			{
				int id = info.Replace.at(i);
                if (id + 1 < (int)Param.size())
                {
                    DParam.at(i) = GlobalDecryptString(Param.at(id + 1));
                }
                   
				else DParam.at(i) = "";
			}
		}
		return CalculateExpr(info.ExprIter, DParam);
	}
	

	struct LinkInfo// cond expr(lim 1) from to hint
	{
		bool On;
		std::string From, Req, ToINI, ToSec, Button, Hint;
	};
	std::vector<LinkInfo> Links;
	std::unordered_map < std::string, int > LinkName;

	int Debug(int Cursor, const std::vector<std::string>& Param)
	{
		std::string Oup = u8"调试数据 : { ";
		bool isexpr = false;
		for (auto s : Param)
		{
			std::string sr = GlobalDecryptString(s);
			if (sr == "#EXPR")
			{
				isexpr = true;
			}
			else if (isexpr)
			{
				Oup += ("\"" + s + "\"=\"" + sr + "\"( ");
				if (HasExpr(sr))
				{
					auto& info = ExprMap[sr];
					for (size_t j = 0; j < info.Expr.size(); j++)
					{
						if ((int)j == info.ExprIter)Oup += u8"**";
						if (info.Expr[j] == u8"#PARAM__")
						{
							Oup += (u8"参数(" + std::to_string(info.Replace[j]) + ") ");
						}
						else Oup += ("\"" + info.Expr[j] + "\" ");
					}
					Oup += u8"运行位置=" + ((int)info.Expr.size() == info.ExprIter ? u8"结尾" : std::to_string(info.ExprIter + 1)) + " ";
				}
				else Oup += u8"找不到 ) ";
				isexpr = false;
			}
			else
			{
				auto p = s.find_first_of('@');
				if (s == sr && p != 0 && p != 1)Oup += ("\"" + s + "\" ");
				else Oup += ("\"" + s + "\"=\"" + sr + "\" ");
			}
		}
		Oup.push_back('}');

		OTMutex.lock();
		OutputText.push_back(Oup);
		OTMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(FnInfoDelay));
		HasOutput = true;

		return Cursor + 1;
	}
	int Begin(int Cursor, const std::vector<std::string>& Param)
	{
		//printf("Function");//OUT
		//PrintVec(Param);//OUT
		//cout<<endl;//OUT
		CallStack.push({ (Param.empty() ? "" : GlobalDecryptString(Param.at(0))),{} });
		auto& Par = CallStack.top().ParamMap;
		int LoadPar = std::min(Param.size() - 1, CallOrder.size());
		for (int i = 0; i < LoadPar; i++)
		{
			Par[Param.at(i + 1)] = CallOrder[i];
		}
		GlobalEnviron["FUNCTION"] = Param.at(0);
		return Cursor + 1;
	}
	int End(int Cursor, const std::vector<std::string>& Param)
	{
		//printf("Return");//OUT 
		//PrintVec(Param);//OUT
		//cout<<endl;//OUT
		int ToCur = RetStack.top();
		for (const auto& ss : LocalStack.top())
		{
			GlobalEnviron.erase(ss);
		}
		RetStack.pop();
		CallStack.pop();
		LocalStack.pop();
		GlobalEnviron["FUNCTION"] = CallStack.top().CallName;
		return ToCur + 1;
	}
	int SetValue(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		GlobalEncryptString(Param.at(0), Param.at(1));
		return Cursor + 1;
	}
	int Raw(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		GlobalEncryptRawString(Param.at(0), Param.at(1));
		return Cursor + 1;
	}
	int LongCall(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fname[MAX_STRING_LENGTH];
		if (Param.size() < 2)return Cursor + 1;
		int JmpTo;
		if (Param.at(1) == "#ALL")
		{
			JmpTo = FindLabelPos({ Begin,{Param.at(0)},0 });
		}
		else if (Param.at(1) == "#THIS")
		{
			int CurFile = CmdLBelongFile[GlobalCursor];
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Begin,{Param.at(0)},0 });
		}
		else
		{
			auto it = std::find_if(FileName.begin(), FileName.end(), [&Param](const std::string& op) {return op == Param.at(1); });
			if (it == FileName.end())
			{
				sprintf(Fname, u8"未载入名为\"%s\"的文件", Param.at(1).c_str());
				Warning(Fname);
				return Cursor + 1;
			}
			int CurFile = std::distance(FileName.begin(), it);
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Begin,{Param.at(0)},0 });
		}

		//cout<<"LongJmpTo:"<<JmpTo<<endl;//OUT
		if (JmpTo == -1)
		{
			sprintf(Fname, u8"找不到名为\"%s\"的调用点", Param.at(0).c_str());
			Warning(Fname);
			return Cursor + 1;
		}
		CallOrder.clear();
		CallOrder.insert(CallOrder.end(), Param.begin() + 2, Param.end());
		LocalStack.push({});
		for (auto& it : CallOrder)
		{
			if (it.length() >= 2 && it[0] == 'V' && it[1] == '@')
			{
				std::string ns = std::to_string(rand()) + std::to_string(rand()) + std::string(it.begin() + 2, it.end());
				GlobalEnviron[ns] = GlobalDecryptString(std::string(it.begin() + 1, it.end()));
				LocalStack.top().push_back(ns);
				it = "@" + ns;
			}
		}
		//cout<<"CallOrder:";//OUT
		//PrintVec(CallOrder);//OUT
		//cout<<endl;//OUT
		RetStack.push(Cursor);
		return JmpTo;
	}
	int Label(int Cursor, const std::vector<std::string>& Param)
	{
		return Cursor + 1;
	}
	int Nothing(int Cursor, const std::vector<std::string>& Param)
	{
		return Cursor + 1;
	}
	int ImportFile(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		auto str = LoadFileStr(Param.at(0));
		auto cmd = GetValidLines(str);
		FileName.push_back(Param.at(0));
		FileCmdLOffset.push_back(GlobalCommands.size());
		FileRealLOffset.push_back(GlobalFileStr.size());
		GlobalFileStr.insert(GlobalFileStr.end(), str.begin(), str.end());
		GlobalCommands.insert(GlobalCommands.end(), cmd.begin(), cmd.end());
		CmdLBelongFile.insert(CmdLBelongFile.end(), cmd.size(), FileName.size() - 1);
		RealLBelongFile.insert(RealLBelongFile.end(), str.size(), FileName.size() - 1);
		return Cursor + 1;
	}
	int LabelJump(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fname[MAX_STRING_LENGTH];
		if (Param.size() < 2)return Cursor + 1;
		int JmpTo;
		if (Param.at(1) == "#ALL")
		{
			JmpTo = FindLabelPos({ Label,{Param.at(0)},0 });
		}
		else if (Param.at(1) == "#THIS")
		{
			int CurFile = CmdLBelongFile[GlobalCursor];
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{Param.at(0)},0 });
		}
		else
		{
			auto it = std::find_if(FileName.begin(), FileName.end(), [&Param](const std::string& op) {return op == Param.at(1); });
			if (it == FileName.end())
			{
				sprintf(Fname, u8"未载入名为\"%s\"的文件", Param.at(1).c_str());
				Warning(Fname);
				return Cursor + 1;
			}
			int CurFile = std::distance(FileName.begin(), it);
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{Param.at(0)},0 });
		}
		//cout<<"JmpTo:"<<JmpTo<<endl;//OUT
		if (JmpTo == -1)
		{
			sprintf(Fname, u8"找不到名为\"%s\"的标签", Param.at(0).c_str());
			Warning(Fname);
			return Cursor + 1;
		}
		return JmpTo + 1;
	}
	int LabelJumpIf(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fname[MAX_STRING_LENGTH];
		if (Param.size() < 2)return Cursor + 1;
		if (!HasExpr(GlobalDecryptString(Param.at(1))))return Cursor + 1;
		std::string ans = CallCalculateExpr(Param);
		if (ans == "#CONTINUE")return Cursor + 1;
		int JmpTo;
		if (Param.at(0) == "#ALL")
		{
			JmpTo = FindLabelPos({ Label,{ans},0 });
		}
		else if (Param.at(0) == "#THIS")
		{
			int CurFile = CmdLBelongFile[GlobalCursor];
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{ans},0 });
		}
		else
		{
			auto it = std::find_if(FileName.begin(), FileName.end(), [&Param](const std::string& op) {return op == Param.at(0); });
			if (it == FileName.end())
			{
				sprintf(Fname, u8"未载入名为\"%s\"的文件", Param.at(0).c_str());
				Warning(Fname);
				return Cursor + 1;
			}
			int CurFile = std::distance(FileName.begin(), it);
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{ans},0 });
		}
		//cout<<"JmpTo:"<<JmpTo<<endl;//OUT
		if (JmpTo == -1)
		{
			sprintf(Fname, u8"找不到名为\"%s\"的标签", ans.c_str());
			Warning(Fname);
			return Cursor + 1;
		}
		return JmpTo + 1;
	}
	int CmdLineJump(int Cursor, const std::vector<std::string>& Param)
	{
		int T;
		if (Param.empty())T = 0;
		else T = atoi(GlobalDecryptString(Param.at(0)).c_str());
		return Cursor + (T ? T : 1);
	}
	int PrintTo(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::string sp;
		std::vector<std::string> TParam;
		bool isExpr = false;
		for (auto st = Param.begin() + 1; st != Param.end(); ++st)
			for (const auto& ss : SplitParam(GlobalDecryptString(*st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
							TParam.push_back("\n");
						}
						TParam.pop_back();
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}
		//PrintVec(TParam);//OUT
		//cout << endl;//OUT
		if (TParam.empty())return Cursor + 1;
		for (auto si : TParam) { sp += GlobalDecryptString(si); sp.push_back('\n'); }
		GlobalEncryptString(TParam.at(0), sp);
		return Cursor + 1;
	}
	int LPrintTo(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		std::string sp;
		std::vector<std::string> TParam;
		bool isExpr = false;
		for (auto st = Param.begin() + 1; st != Param.end(); ++st)
			for (const auto& ss : SplitParam(GlobalDecryptString(*st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
							TParam.push_back(",");
						}
						TParam.pop_back();
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}
		//PrintVec(TParam);//OUT
		//cout<<endl;//OUT
		if (TParam.empty())return Cursor + 1;
		for (auto si : TParam) { sp += GlobalDecryptString(si); }
		GlobalEncryptString(Param.at(0), sp);
		return Cursor + 1;
	}
	int Run(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fcs[MAX_STRING_LENGTH];
		if (Param.empty())return Cursor + 1;
		std::vector<std::string> TParam;
		bool isExpr = false;
		for (const auto& st : Param)
			for (const auto& ss : SplitParam(GlobalDecryptString(st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
						}
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}

		if (TParam.empty())return Cursor + 1;
		auto FnS = GlobalDecryptString(TParam.at(0));
		auto FnIter = CommandMap.find(FnS);
		TParam.erase(TParam.begin());
		if (FnIter != CommandMap.end())
		{
			//std::string tot;
			//for (const auto& ss : TParam)tot += ('\"' + ss + '\"');
			//sprintf(Fcs,"Run() 解析内容： Fn=\"%s\",Param={%s}", FnS.c_str(), tot.c_str());
			//Warning(Fcs);
			return FnIter->second(Cursor, TParam);
		}
		else
		{
			std::string tot;
			for (const auto& ss : TParam)tot += ('\"' + ss + '\"');
			sprintf(Fcs, u8"Run() 解析错误： Fn=\"%s\",Param={%s}", FnS.c_str(), tot.c_str());
			Warning(Fcs);
			return Cursor + 1;
		}

	}
	int Expression(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::vector<std::string> TParam;
		bool isExpr = false;
		/*
		for (size_t i = 0; i < Param.size(); i++)
		{
			Warning(("##PARAM : " + std::to_string(i) + " \"" + Param[i] + "\"").c_str());
		}*/
		for (const auto& st : Param)
			for (const auto& ss : SplitParam(GlobalDecryptString(st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
						}
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}
		/*v
		for (size_t i = 0; i < TParam.size(); i++)
		{
			Warning(("##TPARAM : " + std::to_string(i) + " \"" + TParam[i] + "\"").c_str());
		}*/
		if (TParam.empty())return Cursor + 1;
		std::string st = GlobalDecryptString(TParam.at(0));
		auto& info = ExprMap[st];
		//Warning(("EXPR NAME = " + st).c_str());
		if (TParam.size() == 1)return Cursor + 1;
		//Warning(("EXPR NAME = " + st).c_str());
        info.LineNo = Cursor;
		info.Expr.clear();
		info.Replace.resize(TParam.size() - 1);
		info.ExprIter = 0;

		for (size_t i = 1; i < TParam.size(); i++)
		{
			std::string sr = GlobalDecryptString(TParam.at(i));
			//Warning(("TPARAM : " + std::to_string(i) + " \"" + sr + "\"").c_str());
			if (sr.length() > 5 && std::string(sr.begin(), sr.begin() + 5) == "#PAR<")
			{
				int Rx;
				sscanf(sr.c_str(), "#PAR<%d>", &Rx);
				sr = "#PARAM__";
				info.Replace[i - 1] = Rx;
				info.MaxParam = std::max(Rx, info.MaxParam);
			}
			else if (sr.length() > 6 && std::string(sr.begin(), sr.begin() + 5) == "R#PAR<")
			{
				info.Expr.push_back(std::string(sr.begin() + 1, sr.end()));
			}
			info.Expr.push_back(sr);
		}
		return Cursor + 1;
	}
	int RunExpr(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		if (!HasExpr(GlobalDecryptString(Param.at(1))))return Cursor + 1;
		GlobalEncryptString(Param.at(0), CallCalculateExpr(Param));
		return Cursor + 1;
	}
	int Bind(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 3)return Cursor + 1;
		std::string sf = GlobalDecryptString(Param.at(1));
		if (!HasExpr(sf))return Cursor + 1;
		auto& infof = ExprMap[sf];
		auto& infot = ExprMap[GlobalDecryptString(Param.at(0))];
		if (Param.at(2) == "#SAME")
		{
			infot = infof;
		}
		else if (Param.at(2) == "#FULL")
		{
			infot = infof;
			infot.ExprIter = 0;
		}
		else if (Param.at(2) == "#BEFORE")
		{
			infot = ExprInfo{};
			infot.Expr.insert(infot.Expr.end(), infof.Expr.begin(), infof.Expr.begin() + infof.ExprIter);
			infot.Replace.insert(infot.Replace.end(), infof.Replace.begin(), infof.Replace.begin() + infof.ExprIter);
			infot.ExprIter = 0;
			infot.MaxParam = 0;
            infot.LineNo = Cursor;
			for (int p : infot.Replace)infot.MaxParam = std::max(infot.MaxParam, p);
		}
		else if (Param.at(2) == "#AFTER")
		{
			infot = ExprInfo{};
			infot.Expr.insert(infot.Expr.end(), infof.Expr.begin() + infof.ExprIter, infof.Expr.end());
			infot.Replace.insert(infot.Replace.end(), infof.Replace.begin() + infof.ExprIter, infof.Replace.end());
			infot.ExprIter = 0;
			infot.MaxParam = 0;
            infot.LineNo = Cursor;
			for (int p : infot.Replace)infot.MaxParam = std::max(infot.MaxParam, p);
		}
		else
		{
			ExprMap.erase(GlobalDecryptString(Param.at(0)));
			return Cursor + 1;
		}
		if (Param.size() == 3)return Cursor + 1;
		for (size_t i = 0; i < infot.Expr.size(); i++)
		{
			if (infot.Expr.at(i) == "#PARAM__")
			{
				int id = infot.Replace.at(i);
				if (id + 2 < (int)Param.size())
				{
					sf = GlobalDecryptString(Param.at(id + 2));
					if (sf.length() > 5 && std::string(sf.begin(), sf.begin() + 5) == "#PAR<")
					{
						int Rx;
						sscanf(sf.c_str(), "#PAR<%d>", &Rx);
						infot.Expr.at(i) = "#PARAM__";
						infot.Replace[i] = Rx;
						infot.MaxParam = std::max(Rx, infot.MaxParam);
					}
					else if (sf.length() > 6 && std::string(sf.begin(), sf.begin() + 5) == "R#PAR<")
					{
						infot.Expr.at(i) = std::string(sf.begin() + 1, sf.end());
						infot.Replace.at(i) = 0; 
					}
					else
					{
						infot.Expr.at(i) = sf;
						infot.Replace.at(i) = 0;
					}
				}
				else infot.Expr.at(i) = "";
			}
		}
		return Cursor + 1;
	}
	int SetExprIter(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 3)return Cursor + 1;
		std::string ss = GlobalDecryptString(Param.at(0));
		if (!HasExpr(ss))return Cursor + 1;
		auto& info = ExprMap[ss];
		int Rx = atoi(Param.at(2).c_str());
		if (Param.at(1) == "#CUR")info.ExprIter += Rx;
		if (Param.at(1) == "#BEGIN")info.ExprIter = Rx - 1;
		if (Param.at(1) == "#END")info.ExprIter = info.Expr.size() - Rx;
		return Cursor + 1;
	}
	int Link(int Cursor, const std::vector<std::string>& Param)// cond expr(lim 1) button hint
	{
		if (Param.size() < 8)return Cursor + 1;
		LinkInfo inf{ false,
			GlobalDecryptString(Param.at(2)),
			GlobalDecryptString(Param.at(3)),
			GlobalDecryptString(Param.at(4)),
			GlobalDecryptString(Param.at(5)),
			GlobalDecryptString(Param.at(6)),
			GlobalDecryptString(Param.at(7)) };

		if (GlobalDecryptString(Param.at(1)) == "#ON")inf.On = true;
		else if (GlobalDecryptString(Param.at(1)) == "#OFF")inf.On = false;
		Links.push_back(inf);
		LinkName[GlobalDecryptString(Param.at(1))] = Links.size() - 1;
		return Cursor + 1;
	}
	void thrLoadGlobal(const std::string& File)//Force UTF8
	{
		HasOutput = false;
		
		GlobalFileStr = LoadFileStr(File);
		GlobalCommands = GetValidLines(GlobalFileStr);
		RealLBelongFile.push_back(0);

		FileName.push_back(File);
		FileCmdLOffset.push_back(0);
		FileRealLOffset.push_back(1);
		CmdLBelongFile.insert(CmdLBelongFile.end(), GlobalCommands.size(), FileName.size() - 1);
		RealLBelongFile.insert(RealLBelongFile.end(), GlobalFileStr.size(), FileName.size() - 1);

		GlobalCursor = FindLabelPos({ Begin,{"main"},0 });
		RetStack.push(GlobalCursor);
		CallStack.push({ "main",{} });
		LocalStack.push({});
		//cout<<"Cursor:"<<Cursor<<" AT "<<GlobalCommands[Cursor].RealLineNo<<endl;
		while (GlobalCursor >= 0 && GlobalCursor < (int)GlobalCommands.size())
		{
			GlobalCursor = GlobalCommands[GlobalCursor].Func(GlobalCursor, GlobalCommands[GlobalCursor].Param);
			//printf("Cursor=%d LineNo=%d\n",Cursor,GlobalCommands[Cursor].RealLineNo);
			//cout<<"Environ:";
			//PrintUM(GlobalEnviron);
			//cout<<endl;
			if (CallStack.size() <= 1)break;
		}

		if (HasOutput)std::this_thread::sleep_for(std::chrono::milliseconds(FnInfoEndDelay));
	}

	struct LinkResult
	{
		std::string JumpINI, JumpSec , Button, Hint;
        std::unordered_set<std::string> Attrib;
	};
	/*
		const std::string& ININame,
		const std::string& INIActive,
		const std::string& CurSec,
		const std::string& TgKey,
		const std::string& TgValue
		*/
	std::vector<LinkResult> ProcessLink(std::vector<std::string>& Params)
	{
		std::vector<LinkResult> Result;
		for (auto& lnk : Links)
		{
			if (lnk.On)
			{
				ExprMap[lnk.From].ExprIter = 0;
				Params[1] = lnk.From;
				std::string froms = CallCalculateExpr(Params);
				Params[1] = froms;
				ExprMap[froms].ExprIter = 0;
				if (ExprMap[froms].Expr.empty())continue;
				while (ExprMap[froms].ExprIter != ExprMap[froms].Expr.size())
				{
					std::string spls = CallCalculateExpr(Params);
					Params[1] = lnk.Req;
					ExprMap[lnk.Req].ExprIter = 0;
					if (Params.size() < 8)Params.resize(9);
					Params[7] = spls;
					if (lnk.Req != "#IGNORE")
					{
						std::string reqs = CallCalculateExpr(Params);
						if (!IsTrueString(reqs))
						{
							Params[1] = froms;
							Params[7] = "";
							continue;
						}
					}
					Params[1] = lnk.ToINI;
					ExprMap[lnk.ToINI].ExprIter = 0;
					std::string toi = CallCalculateExpr(Params);
					Params[1] = lnk.ToSec;
					ExprMap[lnk.ToSec].ExprIter = 0;
					std::string tos = CallCalculateExpr(Params);
					Params[1] = lnk.Button;
					ExprMap[lnk.Button].ExprIter = 0;
					std::string bns = CallCalculateExpr(Params);
					Params[1] = lnk.Hint;
					ExprMap[lnk.Hint].ExprIter = 0;
					std::string his = CallCalculateExpr(Params);
                    Params[1] = his;
                    ExprMap[his].ExprIter = 0;
                    std::unordered_set<std::string> _Attr;
                    std::string hir = CallCalculateExpr(Params);
                    while (ExprMap[his].ExprIter != ExprMap[his].Expr.size())
                    {
                        _Attr.insert(CallCalculateExpr(Params));
                    }
					//ImGui::TextWrapped(u8"to=%s btn=%s hint=%s", tos.c_str(), bns.c_str(), his.c_str());
                    ExprMap.erase(his);
					Result.push_back({ toi,tos,bns,hir,_Attr });
					Params[1] = froms;
					Params[7] = "";
				}
                ExprMap.erase(froms);
			}
		}
		return Result;
	}

	/*
	int main()
	{
		//rnd = std::default_random_engine{ (unsigned)time(NULL) };
		//srand(time(NULL));
		//thrLoadGlobal("load.in");
		system("pause");
		return 0;
	}
	*/
}















