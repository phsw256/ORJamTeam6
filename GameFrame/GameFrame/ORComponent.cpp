#include "ORComponent.h"
#include "ORFile.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

ImVec2 WindowRelPos(ImVec2 Rel)
{
    return { ImGui::GetWindowWidth() * Rel.x,ImGui::GetWindowHeight() * Rel.y };
}
ImVec2 FontRelPos(ImVec2 Rel)
{
    return { ImGui::GetFontSize() * Rel.x,ImGui::GetFontSize() * Rel.y };
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
        if (Stages[i]->DrawButton())
        {
            CurrentStage = i;
            Stages[i]->OnSwitched();
            /*if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("ÇÐ»»ÁËÖ÷²Ëµ¥");
            }*/
        }
        ImGui::SameLine();
    }
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
