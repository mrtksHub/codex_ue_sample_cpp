using UnrealBuildTool;
using System.Collections.Generic;

public class codex_ue_sample_cppTarget : TargetRules
{
    public codex_ue_sample_cppTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("ScreenFlow");
    }
}
