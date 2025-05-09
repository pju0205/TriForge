// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class TriForgeServerTarget : TargetRules
{
	public TriForgeServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		bUseAdaptiveUnityBuild = false;

        ExtraModuleNames.AddRange(new string[] { "TriForge", "DedicatedServers" });
    }
}
