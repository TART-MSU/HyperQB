(*    
    Copyright (C) 2022 Raven Beutner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*)

module Program

open System 
open System.IO 

open Core.RunConfiguration
open Core.ModelChecking

open CommandLineParser
open ModelCheckingEntryPoint

[<EntryPoint>]
let main args =

    let swtotal = System.Diagnostics.Stopwatch()
    swtotal.Start()

    let args = 
        if args.Length = 0 then 
            // In case no command line arguments are given, we assume the user wants the help message
            [|"--help"|]
        else 
            args

    // Parse the command line args
    let cmdArgs =
        match CommandLineParser.parseCommandLineArguments (Array.toList args) with
            | Result.Ok x -> x
            | Result.Error e ->
                   printfn $"%s{e}"
                   exit 0
                   
    // By convention the paths.conf file is located in the same directory as the HyPA executable
    // Get the path of the current binary
    let configPath = 
        System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + "/paths.conf"
                   
    // Check if the path to the config file is valid , i.e., the file exists
    if System.IO.FileInfo(configPath).Exists |> not then 
        printfn "The paths.conf file does not exist in the same directory as the executable"
        exit 1              
    
    // Parse the config File
    let configContent = 
        try
            File.ReadAllText configPath
        with 
            | _ -> 
                printfn "Could not open paths.conf file"
                exit 1

    // Only log events that match the verbosity
    let logger vb s = 
        if List.contains cmdArgs.Verbosity vb then 
            printf $"%s{s}" 
    
    let solverConfig = 
        match CommandLineParser.parseConfigFile configContent with 
            | Result.Ok x -> 
                let solverConfig = 
                    {
                        SolverConfiguration.MainPath = "."
                        OSpotPath = x.SpotPath
                        ORabitJarPath = x.RabitJarPath
                        OBaitJarPath = x.BaitJarPath
                        OForkliftJarPath = x.ForkliftJarPath
                    }
                
                logger [TWO; THREE; FOUR] (sprintf "AutoHyper has been given access to the follwoing solver: ")

                if solverConfig.OSpotPath.IsSome then 
                    logger [TWO; THREE; FOUR] (sprintf "spot ")
                    if solverConfig.OSpotPath.Value <> "" && (System.IO.DirectoryInfo(solverConfig.OSpotPath.Value).Exists |> not) then 
                        printfn "The path to the spot directory is incorrect"
                        exit 1
                    
                if solverConfig.ORabitJarPath.IsSome then 
                    logger [TWO; THREE; FOUR] (sprintf "RABIT ")
                    if System.IO.FileInfo(solverConfig.ORabitJarPath.Value).Exists |> not then 
                        printfn "The path to the RABIT jar is incorrect"
                        exit 1
                    
                if solverConfig.OBaitJarPath.IsSome then 
                    logger [TWO; THREE; FOUR] (sprintf "BAIT ")
                    if System.IO.FileInfo(solverConfig.OBaitJarPath.Value).Exists |> not then 
                        printfn "The path to the BAIT jar is incorrect"
                        exit 1

                if solverConfig.OForkliftJarPath.IsSome then 
                    logger [TWO; THREE; FOUR] (sprintf "FORKLIFT ")
                    if System.IO.FileInfo(solverConfig.OForkliftJarPath.Value).Exists |> not then 
                        printfn "The path to the FORKLIFT jar is incorrect"
                        exit 1

                logger [TWO; THREE; FOUR] (sprintf "\n")
                
                solverConfig

            | Result.Error r -> 
                printfn $"%s{r}"
                exit 1
    
    let config = 
        {
            Configuration.SolverConfig = solverConfig;
            Logger = logger
        }
   
    // Check which method should be used, i.e., verify a program, verify a transition system
    match cmdArgs.ExecMode with 
        | ExplictSystem (systemPaths, propPath) -> 

            let m = Option.defaultValue SPOT cmdArgs.Mode
        
            explictSystemVerification config systemPaths propPath m cmdArgs.Timeout
        | SystemFromProgram (systemPaths, propPath) -> 
            let m = Option.defaultValue SPOT cmdArgs.Mode
        
            programSystemVerification config systemPaths propPath m cmdArgs.Timeout

        | NuSMVSystem (systemPaths, propPath) -> 
            let m = Option.defaultValue SPOT cmdArgs.Mode
        
            nuSMVSystemVerification config systemPaths propPath m cmdArgs.Timeout

        | AigerSystem (systemPaths, propPath) -> 
            let m = Option.defaultValue SPOT cmdArgs.Mode
        
            aigerSystemVerification config systemPaths propPath m cmdArgs.Timeout
        
        | INVALID -> 
            printfn "Invalid Command Line Arguments."
            exit 1

    swtotal.Stop()
    config.Logger [TWO; THREE; FOUR] $"Time %i{swtotal.ElapsedMilliseconds}ms (~=%.2f{double(swtotal.ElapsedMilliseconds) / 1000.0}s)\n"

    0
