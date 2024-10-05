#include "ORComponent.h"
#include "ORFile.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "Global.h"

ImVec2 WindowRelPos(ImVec2 Rel)
{
    return { ImGui::GetWindowWidth() * Rel.x,ImGui::GetWindowHeight() * Rel.y };
}
ImVec2 FontRelPos(ImVec2 Rel)
{
    return { ImGui::GetFontSize() * Rel.x,ImGui::GetFontSize() * Rel.y };
}
void InvisibleArrow(const char* Label)
{
    ImGui::PushStyleColor(ImGuiCol_Text, { 0,0,0,0 });
    ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
    ImGui::ArrowButton(Label, ImGuiDir_Left);
    ImGui::PopStyleColor(4);
}

ORComponentFrame::ORComponentFrame(ImVec2 position, ImVec2 size) : ORComponent()
{
    Position = position;
    Size = size;
}
void ORComponentFrame::Resize(ImVec2 position, ImVec2 size)
{
    Position = position;
    Size = size;
}


ORDescManager::ORDescManager(ImVec2 position, ImVec2 size) : ORComponentFrame(position, size) {}
void ORDescManager::DrawUI()
{
    auto Pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(Position);
    ImGui::BeginChildFrame(ThisImGuiID, Size, flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    if (BackGround)
    {
        auto Pos2 = ImGui::GetCursorPos();
        ImGui::SetCursorPos({ 0.0f,0.0f });
        BackGround->Draw();
        ImGui::SetCursorPos(Pos2);
    }
    ImGui::TextWrapped(Desc.c_str());
    ImGui::EndChildFrame();
    ImGui::SetCursorPos(Pos);
}

void ORDescManager::Clear()
{
    Desc.clear();
}
void ORDescManager::SetDesc(const _UTF8 std::string_view desc)
{
    Desc = desc;
}

bool ORStage::DrawButton()
{
    bool Ret = false;
    ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
    if (Enabled)Ret = ImGui::SmallButton(Name.c_str());
    else ImGui::TextDisabled(Name.c_str());
    ImGui::PopStyleColor(1);
    return Ret;
}
ORStage::ORStage(const _UTF8 std::string_view StageName) :ORComponent(), Name(StageName)
{

}

void ORTopBar::DrawUI()
{
    for (size_t i = 0; i < Stages.size(); i++)
    {
        if (!Stages[i])continue;
        if (!ShowDisabledButtons && !Stages[i]->IsEnabled())continue;
        if (DrawUIExtFilter && !DrawUIExtFilter->IsSelected(*Stages[i]))continue;
        if (Stages[i]->DrawButton())
        {
            CurrentStage = i;
            Stages[i]->OnSwitched();
            /*if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("切换了主菜单");
            }*/
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();
}
void ORTopBar::AddStage(std::unique_ptr<ORStage>&& pStage)
{
    Stages.push_back(std::move(pStage));
}
ORStage* ORTopBar::GetCurrentStage() const
{
    if (Stages.empty())return nullptr;
    auto& p = Stages[CurrentStage];
    if (p && p->IsEnabled())return p.get();
    return nullptr;
}
bool ORTopBar::ForceChangeStage(int StageID)
{
    if (StageID == CurrentStage || StageID < 0 || StageID >= (int)Stages.size())return false;
    CurrentStage = StageID;
    Stages[StageID]->OnSwitched();
    return true;
}
bool ORTopBar::ForceChangeStage(const _UTF8 std::string_view Name)
{
    auto ps = ORTopBar::GetCurrentStage();
    if (ps && ps->GetName() == Name)return false;
    for (size_t i = 0; i < Stages.size(); i++)
    {
        if (i == CurrentStage)continue;
        if (Stages[i] && Stages[i]->GetName() == Name)
        {
            CurrentStage = i;
            Stages[i]->OnSwitched();
            return true;
        }
    }
    return false;
}


void ORHintManager::Clear()
{
    HasSet = HasCustom = IsCountDown = false;
}
bool ORHintManager::LoadHintFile(const std::string_view FileName)
{
    auto st = ORExtFileStraw(FileName);
    if (!st.Available())return false;
    Hint = std::move(GetLines(st));
    return true;
}
bool ORHintManager::LoadHintFile(ORStaticStraw& Source)
{
    if (!Source.Available())return false;
    Hint = std::move(GetLines(Source));
    return true;
}
void ORHintManager::DrawUI()
{
    ImDrawList* DList = ImGui::GetForegroundDrawList();
    if (!Hint.empty())
    {
        if (HintTimer.GetMilli() > HintChangeMillis)
        {
            UseHint = rand() % Hint.size();
            HintTimer.Set();
        }
    }
    if (HasSet && IsCountDown && CountDownTimer.GetMilli() > CountDownMillis)Clear();
    if (HasSet && HasCustom)
        if (!CustomFn(CustomHint))Clear();

    DList->AddRectFilled(Position, Position + Size, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));
    DList->AddImage(BackGround->GetID(), Position, Position + Size);
    if (FrameOnBottom)DList->AddLine(Position, Position + ImVec2{ Size.x, 0.0F }, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), BorderThickness);
    else DList->AddRect(Position, Position + Size, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), BorderRounding, ImDrawFlags_None, BorderThickness);

    if (!Hint.empty() || HasSet)
    {
        const std::string& HintStr = GetHint();
        DList->AddText(Position + FontRelPos({ 0.8F,0.25F }), 
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
            HintStr.c_str(), HintStr.c_str() + HintStr.size());
    }
}
void ORHintManager::SetHint(const _UTF8 std::string& Str, int TimeLimitMillis)
{
    HasCustom = false;
    HasSet = true;
    CustomHint = Str;
    if (TimeLimitMillis != -1)
    {
        IsCountDown = true;
        CountDownMillis = TimeLimitMillis;
        CountDownTimer.Set();
    }
    else IsCountDown = false;

}
void ORHintManager::SetHintCustom(const std::function<bool(_UTF8 std::string&)>& Fn)
{
    HasCustom = true;
    HasSet = true;
    CustomHint = "";
    IsCountDown = false;
    CustomFn = Fn;
}
const std::string& ORHintManager::GetHint() const
{
    return HasSet ? CustomHint : Hint[UseHint];
}

ORPopUp& ORPopUp::Create(const _UTF8 std::string& title)
{
    Title = title;
    CanClose = false;
    Show = []() {};
    Flag = ImGuiWindowFlags_AlwaysAutoResize;
    return *this;
}
ORPopUp& ORPopUp::CreateModal(const _UTF8 std::string& title, bool canclose, StdMessage close)
{
    Title = title;
    Modal = true;
    CanClose = canclose;
    Close = close;
    Show = []() {};
    Flag = ImGuiWindowFlags_AlwaysAutoResize;
    return *this;
}
ORPopUp& ORPopUp::SetTitle(const _UTF8 std::string& title)
{
    Title = title;
    return *this;
}
ORPopUp& ORPopUp::SetFlag(ImGuiWindowFlags flag)
{
    Flag |= flag;
    return *this;
}
ORPopUp& ORPopUp::UnsetFlag(ImGuiWindowFlags flag)
{
    Flag &= ~flag;
    return *this;
}
ORPopUp& ORPopUp::ClearFlag()
{
    Flag = 0;
    return *this;
}
ORPopUp& ORPopUp::PushTextPrev(const _UTF8 std::string& Text)//TODO:优化，试图砍掉prev
{
    StdMessage ShowPrev{ std::move(Show) };
    Show = [=]() {ImGui::TextWrapped(Text.c_str()); ShowPrev(); };
    return *this;
}
ORPopUp& ORPopUp::PushTextBack(const _UTF8 std::string& Text)
{
    StdMessage ShowPrev{ std::move(Show) };
    Show = [=]() {ShowPrev(); ImGui::TextWrapped(Text.c_str()); };
    return *this;
}
ORPopUp& ORPopUp::PushMsgPrev(StdMessage Msg)
{
    StdMessage ShowPrev{ std::move(Show) };
    Show = [=]() {Msg(); ShowPrev(); };
    return *this;
}
ORPopUp& ORPopUp::PushMsgBack(StdMessage Msg)
{
    StdMessage ShowPrev{ std::move(Show) };
    Show = [=]() {ShowPrev(); Msg(); };
    return *this;
}
ORPopUp& ORPopUp::SetSize(ImVec2 NewSize)
{
    Size = NewSize;
    return *this;
}


ORPopUp::~ORPopUp()
{
    Close();
}
void ORPopUp::DrawUI()
{
    Show();
}

std::unique_ptr<ORPopUp> ORPopUpManager::BasicPopUp()
{
    return std::move(std::make_unique<ORPopUp>());
}

std::unique_ptr<ORPopUp> ORPopUpManager::SingleText(const _UTF8 std::string& StrId, const _UTF8 std::string& Text, bool Modal)
{
    auto pPopUp = std::make_unique<ORPopUp>();
    if (Modal)pPopUp->CreateModal(StrId, false);
    else pPopUp->Create(StrId).SetFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize).PushTextBack(Text);
    return std::move(pPopUp);
}

void ORPopUpManager::DrawUI()
{
    if (HasPopup && CurrentPopup)
    {
        ImGui::OpenPopup(CurrentPopup->Title.c_str());
        bool HPPrev = HasPopup;
        if (CurrentPopup->Size.x >= 1.0F && CurrentPopup->Size.y >= 1.0F)ImGui::SetNextWindowSize(CurrentPopup->Size);
        ImGui::SetNextWindowPos(CurrentPopup->Position);
        if (CurrentPopup->Modal)
        {
            if (ImGui::BeginPopupModal(CurrentPopup->Title.c_str(), CurrentPopup->CanClose ? (&HasPopup) : nullptr), CurrentPopup->Flag)
            {
                CurrentPopup->DrawUI();
                ImGui::EndPopup();
            }
        }
        else
        {
            if (ImGui::BeginPopup(CurrentPopup->Title.c_str(), CurrentPopup->Flag))
            {
                CurrentPopup->DrawUI();
                ImGui::EndPopup();
            }
        }
        if (CurrentPopup->CanClose && HPPrev && (!HasPopup)) CurrentPopup.reset();
        ImGui::CloseCurrentPopup();
    }
}


void Browse_ShowList_Impl(const std::string& suffix, int Sz, int* Page, int KeyPerPage)
{
    int RenderF = (*Page) * KeyPerPage;
    int RenderN = (1 + (*Page)) * KeyPerPage;
    bool HasPrev = ((*Page) != 0);
    bool HasNext = (RenderN < Sz);
    int RealRF = std::max(RenderF, 0);
    int RealNF = std::min(RenderN, Sz);
    int PageN = (Sz - 1) / KeyPerPage + 1;
    if (HasPrev || HasNext)
    {
        if (HasPrev)
        {
            if (ImGui::ArrowButton(("prev_" + suffix).c_str(), ImGuiDir_Left))
            {
                (*Page)--;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了上一页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();
            ImGui::Text(u8"上一页");
            ImGui::SameLine();
        }
        else
        {
            InvisibleArrow(("prev_" + suffix).c_str());
            ImGui::SameLine();
        }
        if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 13.0f);
        if ((*Page) + 1 >= 100)ImGui::SetCursorPosX(FontHeight * 12.5f);
        else ImGui::SetCursorPosX(FontHeight * 12.0f);
        if (HasNext)
        {
            ImGui::Text(u8"下一页");
            ImGui::SameLine();
            if (ImGui::ArrowButton(("next_" + suffix).c_str(), ImGuiDir_Right))
            {
                (*Page)++;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了下一页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();

        }
        ImGui::NewLine();

        if (*Page != 0)
        {
            if (ImGui::ArrowButton(("fpg_" + suffix).c_str(), ImGuiDir_Left))
            {
                (*Page) = 0;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了第一页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();
            ImGui::Text(u8"第一页");
            ImGui::SameLine();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, { 0,0,0,0 });
            ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
            ImGui::ArrowButton("OBSOLETE_BUTTON", ImGuiDir_Left);
            ImGui::SameLine();
            ImGui::PopStyleColor(4);
        }
        if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 5.5f);
        else ImGui::SetCursorPosX(FontHeight * 6.0f);
        auto PosYText = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(PosYText - FontHeight * 0.5f);
        ImGui::Text(u8"第（%d/%d）页", (*Page) + 1, PageN);
        ImGui::SetCursorPosY(PosYText);
        ImGui::SameLine();
        if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 13.0f);
        if ((*Page) + 1 >= 100)ImGui::SetCursorPosX(FontHeight * 12.5f);
        else ImGui::SetCursorPosX(FontHeight * 12.0f);
        if ((*Page) + 1 != PageN)
        {
            ImGui::Text(u8"最后页");
            ImGui::SameLine();
            if (ImGui::ArrowButton(("lpg_" + suffix).c_str(), ImGuiDir_Right))
            {
                (*Page) = PageN - 1;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了最后页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
}




bool ORUndoStack::Undo()
{
    if (Cursor == -1)return false;
    Stack[Cursor].UndoAction();
    --Cursor;
    //IBF_Inst_Project.Project.ChangeAfterSave = true;
    return true;
}
bool ORUndoStack::Redo()
{
    if (Cursor == (int)Stack.size() - 1)return false;
    ++Cursor;
    Stack[Cursor].RedoAction();
    //IBF_Inst_Project.Project.ChangeAfterSave = true;
    return true;
}
bool ORUndoStack::CanUndo() const
{
    return Cursor > -1;
}
bool ORUndoStack::CanRedo() const
{
    return Cursor < (int)Stack.size() - 1;
}
void ORUndoStack::Release()
{
    while (Cursor < (int)Stack.size() - 1)
        Stack.pop_back();
}
void ORUndoStack::Push(const _Item& a)
{
    Release();
    Stack.push_back(a);
    ++Cursor;
    //IBF_Inst_Project.Project.ChangeAfterSave = true;
}
void ORUndoStack::Clear()
{
    Stack.clear();
}
ORUndoStack::_Item* ORUndoStack::Top()
{
    if (Cursor <= -1)return nullptr;
    else return &Stack.at(Cursor);
}
