// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class TriForgeEditorTarget : TargetRules
{
	public TriForgeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		bUseAdaptiveUnityBuild = false;
        ExtraModuleNames.AddRange(new string[] { "TriForge", "DedicatedServers" });
    }
}
