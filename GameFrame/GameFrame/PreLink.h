#pragma once

#define IDI_ICON1 101

#include "MainStage.h"




namespace PreLink
{
    static void glfw_error_callback(int error, const char* description)
    {
        static BufString gle;
        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            sprintf(gle, "GLFW���󣺴����룺%d ������Ϣ��%s", error, description);
            GlobalLog.AddLog(gle);
        }
    }
    
    int PreLoop1()
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        window = glfwCreateWindow(ScrX, ScrY, u8"INI�����", NULL, NULL);
        MainWindowHandle = glfwGetWin32Window(window);
        //glfwHideWindow(window);
        SetClassLong(MainWindowHandle, GCL_HICON, (LONG)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)));
        //ShowWindow(MainWindowHandle, SW_HIDE);
        
        if (window == NULL)
            return 1;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog("�ɹ��򿪴��ڡ�");
        }

        if (Config.Load(".\\Resources\\config.ini"))
        {
            auto Sec = Config.GetSection("Setting");
            if (Sec != Config.NotFound())
            {
                auto Lin = Sec->second.GetLine("Font.Size");
                if (Lin != Sec->second.NotFound())
                {
                    int Tsz;
                    try { Tsz = std::stoi(Lin->second.Value.str); }
                    catch (std::exception& e) { (void)e; }
                    FontHeight = std::max(Tsz, 12);
                    FontHeight = std::min(Tsz, 28);
                }
                Lin = Sec->second.GetLine("Font.Name");
                if (Lin != Sec->second.NotFound())
                {
                    FontPath += Lin->second.Value.str;
                }
                else
                {
                    FontPath += "msyh.ttf";
                }
                Lin = Sec->second.GetLine("Render.UIPositionDeltaX");
                if (Lin != Sec->second.NotFound())
                {
                    try { WindowSizeAdjustX = std::stoi(Lin->second.Value.str); }
                    catch (std::exception& e) { (void)e; }
                }
                Lin = Sec->second.GetLine("Render.UIPositionDeltaY");
                if (Lin != Sec->second.NotFound())
                {
                    try { WindowSizeAdjustY = std::stoi(Lin->second.Value.str); }
                    catch (std::exception& e) { (void)e; }
                }
                Lin = Sec->second.GetLine("Render.FPSLimit");
                if (Lin != Sec->second.NotFound())
                {
                    try { FPSLimit = std::stoi(Lin->second.Value.str); }
                    catch (std::exception& e) { (void)e; }
                }
               
            }
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("�ɹ�����.\\Resources\\config.ini");
            }
        }
        else
        {
            FontHeight = 16;
            FontPath += "msyh.ttf";
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("δ������.\\Resources\\config.ini");
            }
        }

        onexit(ExitUninit);
        return 0;
    }

    void PreLoop2()
    {
        std::string EncodingStr;//MBCS Unicode UTF8

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        ImGuiIO& io = ImGui::GetIO();

        ExtFileClass GetHint;
        if (GetHint.Open(".\\Resources\\load.txt", "rb"))
        {
            static ImVector<ImWchar> myRange;
            ImFontGlyphRangesBuilder myGlyph;
            
            ImWchar base_ranges[]=
            {
                0x0020, 0x00FF, // Basic Latin + Latin Supplement
                0x2000, 0x206F, // General Punctuation
                0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
                0x31F0, 0x31FF, // Katakana Phonetic Extensions
                0xFF00, 0xFFEF, // Half-width characters
                0xFFFD, 0xFFFD, // Invalid
                0
            };
            unsigned char* charBuf;
            GetHint.Seek(0, SEEK_END);
            int Len=GetHint.Position();
            charBuf = new unsigned char[Len+1]();
            GetHint.Seek(0, SEEK_SET);
            GetHint.Read(charBuf, 1, Len);
            GetHint.Close();
            charBuf[Len] = 0;
            myGlyph.AddRanges(base_ranges);
            myGlyph.AddText((const char*)charBuf);
            myGlyph.BuildRanges(&myRange);
            delete[]charBuf;
            //Sleep(12000);
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("�ɹ�����.\\Resources\\load.txt");
            }

            //0x4e00, 0x9FAF, // CJK Ideograms
            //font = io.Fonts->AddFontFromFileTTF(FontPath.c_str(), (float)FontHeight, NULL, LoadRange.data());
            font = io.Fonts->AddFontFromFileTTF(FontPath.c_str(), (float)FontHeight, NULL, myRange.Data);
            
        }
        else
        {
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("δ������.\\Resources\\load.txt ʹ��Ĭ���������");
            }

            font = io.Fonts->AddFontFromFileTTF(FontPath.c_str(), (float)FontHeight, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
            //font = io.Fonts->AddFontFromFileTTF(FontPath.c_str(), (float)FontHeight, NULL, io.Fonts->GetGlyphRangesChineseFull());
        }

        if (font == NULL)
        {
            MessageBoxA(nullptr, "font == NULL", "��������ʧ�ܣ�", MB_ICONERROR);
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL2_Init();

        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog(("�ɹ��������壺" + FontPath).c_str());
        }
        FontLoaded.store(true);

        Pool1.Emplace("001", true, ".\\Resources\\Image\\001.png");
        Mg1.BackGround = Pool1.GetResource("001");
        ORTest::Init();
    }
    
    void CleanUp()
    {
        ORTest::CleanUp();
        // Cleanup
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void ShellLoop()
    {
        try
        {
            // Main loop
            InMainLoop.store(true);
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("��ѭ����ʼ������");
            }
            uint64_t TimeWait = GetSysTimeMicros();
            while (!glfwWindowShouldClose(window))
            {
                if (FPSLimit != -1)
                {
                    while (GetSysTimeMicros() < TimeWait)Sleep(0);
                    TimeWait += 1000000 / FPSLimit;
                }
                // Poll and handle events (inputs, window resize, etc.)
                // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
                // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
                // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
                // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
                glfwPollEvents();

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL2_NewFrame();
                ImGui_ImplGlfw_NewFrame();

                if (font->ContainerAtlas == NULL)
                {
                    if (EnableLog)
                    {
                        GlobalLog.AddLog_CurTime(false);
                        GlobalLog.AddLog("font->ContainerAtlas == NULL");
                        GlobalLog.AddLog_CurTime(false);
                        GlobalLog.AddLog("��������ʧ�ܣ��쳣�˳���");
                        GlobalLog.AddLog_CurTime(false);
                        GlobalLog.AddLog("��������������������Σ���л���֧�֡�");
                        GlobalLog.AddLog_CurTime(false);
                        GlobalLog.AddLog("�����Ȼ����������ϵ���ߡ�");
                    }
                    MessageBoxA(nullptr, "font->ContainerAtlas == NULL", "��������ʧ�ܣ�", MB_ICONERROR);
                    MessageBoxA(nullptr, "���������������������", "��������ʧ�ܣ�", MB_ICONERROR);
                    CleanUp();
                    exit(0);
                }

                ImGui::NewFrame();

                ControlPanel();


                // Rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);

                // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
                // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
                //GLint last_program;
                //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
                //glUseProgram(0);
                ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
                //glUseProgram(last_program);

                glfwMakeContextCurrent(window);
                glfwSwapBuffers(window);
            }
        }
        catch (std::exception& e)
        {
            MessageBoxA(nullptr, e.what(), "�׳��쳣��", MB_ICONERROR);
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("��ѭ���쳣���쳣��Ϣ��");
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog(e.what());
            }
        }
        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog("��ѭ��ֹͣ������");
        }
        
    }

}

