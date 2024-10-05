// Copyright (C) 2019 Nomango

#pragma once


#include"Browser.h"
#include "ORImage.h"
#include "ORFile.h"
#include "ORComponent.h"
#include "ORBackType.h"
#include "MusicMCI.h"

ORResourcePool<ORImage> Pool1;
ORDescManager Mg1;
MusicMCI Ms1;

const char* AppName= u8"INI浏览器";


int ExitCloseAllFile();

int WindowSizeAdjustX = 15, WindowSizeAdjustY = 5;

bool Await{ true };

void ControlPanel()
{
    ///*
    ImGui::Begin(AppName, nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground| ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos({ 0,-(float)(FontHeight + WindowSizeAdjustY) });
    RECT wp;
    GetWindowRect(MainWindowHandle, &wp);
    ImGui::SetWindowSize({ (float)(wp.right - wp.left - WindowSizeAdjustX),(float)(wp.bottom - wp.top) });

    Mg1.Resize(WindowRelPos({ 0.2F,0.65F }), WindowRelPos({ 0.6F,0.3F }));
    ImGui::Text(u8"等待填入内容");
    if (ImGui::IsItemHovered())Mg1.SetDesc(u8"等待填入内容");
    ImGui::Text(u8"平均FPS %.1f", ImGui::GetIO().Framerate);
    if (ImGui::IsItemHovered())Mg1.SetDesc(u8"FUCK");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)(ImColor(0x7F7F7F)));
    Mg1.DrawUI();
    ImGui::PopStyleColor();
    Mg1.Clear();
    ImGui::End();//*/

    //TEST 23/03/18
    ORTest::Loop();

    /*
    static bool pp = true;
    ImGui::OpenPopup(u8"TestUI");
    if (ImGui::BeginPopupModal(u8"TestUI", &pp))
        //if (ImGui::BeginPopup(u8"TestUI"))
    {
        static int iiii = 0;
        if (ImGui::Button(u8"cnm"))++iiii;
        ImGui::Text(u8"woc %d", iiii);
        ImGui::EndPopup();
    }
    */
}


int ExitUninit()
{
	if (EnableLog)
	{
		GlobalLog.AddLog_CurTime(false);
		GlobalLog.AddLog("程序正在结束运行。");
	}

    ORTest::CleanUp();
	//PreLink::CleanUp();

	if (EnableLog)
	{
		GlobalLog.AddLog_CurTime(false);
		GlobalLog.AddLog("程序已经结束运行。");
	}

	return 0;
}


