using UnrealBuildTool;
using System.Collections.Generic;

public class codex_ue_sample_cppEditorTarget : TargetRules
{
    public codex_ue_sample_cppEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("ScreenFlow");
    }
}
