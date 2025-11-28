// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class ShotDemo : ModuleRules
{
	public ShotDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        var IDbgDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../third_party/IDbg"));
        var IDbgIncludeDir = Path.Combine(IDbgDir, "include");
        var IDbgLibDir = Path.Combine(IDbgDir, "lib");

        PublicIncludePaths.Add(IDbgIncludeDir);
        PublicAdditionalLibraries.Add(Path.Combine(IDbgLibDir, "IDbg.lib"));
        PublicDelayLoadDLLs.Add("IDbg.dll");
        
        // Copy DLL to output directory
        string IDbgDllPath = Path.Combine(IDbgLibDir, "IDbg.dll");
        string ProjectBinariesDir = Path.Combine(Target.ProjectFile.Directory.FullName, "Binaries", "Win64");

        System.Console.WriteLine("========================================");
        System.Console.WriteLine("IDbg DLL Path: " + IDbgDllPath);
        System.Console.WriteLine("$(BinaryOutputDir): " + "$(BinaryOutputDir)");
        System.Console.WriteLine("ProjectBinariesDir: " + ProjectBinariesDir);
        System.Console.WriteLine("========================================");
        // Use BinaryOutputDir which is the standard variable for binary output
        RuntimeDependencies.Add("$(BinaryOutputDir)/IDbg.dll", IDbgDllPath);
        
        // Also explicitly add to project Binaries directory as fallback
        RuntimeDependencies.Add(Path.Combine(ProjectBinariesDir, "IDbg.dll"), IDbgDllPath);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
