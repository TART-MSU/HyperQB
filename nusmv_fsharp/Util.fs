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

module Core.Util 

open System

/// A global source of randomness to ensure deterministic computations
let rnd = new Random(0)

/// Trace quantifiers in a HyperLTL formula
type TraceQuantifierType = 
    | FORALL 
    | EXISTS

    member this.Flip = 
        match this with 
            | FORALL -> EXISTS
            | EXISTS -> FORALL

/// A mutable counter
type Counter(init : int) =
    let mutable a = init

    new () = Counter(0)

    member this.Reset() =   
        a <- 0

    member this.Get = a

    member this.Inc() =
        a <- a + 1

    member this.Inc(x) =
        a <- a + x
    
    member this.Dec() =
        a <- a - 1

    member this.Dec(x) =
        a <- a - x

/// Given a number n, computes all lists of booleans of length n 
let rec computeBooleanPowerSet n =
    if n = 0 then
        Seq.singleton []
    else
        let r = computeBooleanPowerSet (n-1)
        Seq.append (Seq.map (fun x -> true::x) r) (Seq.map (fun x -> false::x) r)

/// Compute the cartesian product of a list of sets
let rec cartesianProduct (LL: list<seq<'a>>) =
    match LL with
    | [] -> Seq.singleton []
    | L :: Ls ->
        seq {
            for x in L do
                for xs in cartesianProduct Ls -> x :: xs
        }

/// Compute the powerset of a given set
let powerset (s : Set<'a>) =
    let asList = Set.toList s 

    let rec computeFiniteChoices (A : list<'a>) =
        match A with
            | [] -> Seq.singleton Set.empty
            | x::xs ->
                let r = computeFiniteChoices xs
                Seq.append r (Seq.map (fun y -> Set.add x y) r)

    computeFiniteChoices asList

/// Given a map A -> set<B> compute all possible maps A -> B that are obtained by picking some element from that set for each key in A
let cartesianProductMap (m : Map<'A, Set<'B>>) =
    let keysAsList = Seq.toList m.Keys

    keysAsList
    |> Seq.toList
    |> List.map (fun x -> m.[x] |> seq)
    |> cartesianProduct
    |> Seq.map (fun x -> 
        List.zip keysAsList x
        |> Map
        )

/// Compute the ordinal of a number of fancy printing
let ordinal n =
    match if n > 20 then n % 10 else n % 20 with
    | 1 -> string n + "st"
    | 2 -> string n + "nd"
    | 3 -> string n + "rd"
    | _ -> string n + "th"


/// Parser for variables used in HyperLTL specifications
module ParserUtil = 
    open FParsec

    /// Parser that parses everything between two '"'
    let escapedStringParser : Parser<string, unit> = 
        skipChar '\"' >>. manyChars (satisfy (fun c -> c <> '\"')) .>> skipChar '\"'    

    // A parser for the LTL atoms
    let relVarParser : Parser<string * int, unit> =
        pipe3
            (many1Chars letter)
            (skipChar '_')
            pint32
            (fun x _ y -> x, y)

    // A parser for the LTL atoms in programs
    let relVarParserBit : Parser<(String * int), unit>= 
        pstring "{" >>. 
            pipe3
                (spaces >>. many1Chars letter)
                (pchar '_')
                (pint32 .>> pstring "}")
                (fun x _ y  -> (x, y))

    let pipe6 a b c d e f fu = 
        pipe5
            (a .>>. b)
            c 
            d 
            e 
            f 
            (fun (a, b) c d e f -> fu a b c d e f)

/// A custom type for the return of a system Call
type SystemCallResult = 
    | SystemCallOutcome of String
    | SystemCallError of String
    | SystemCallTimeout

/// Performs a system call
let systemCall cmd arg timeout =
    let p = new System.Diagnostics.Process();
    p.StartInfo.RedirectStandardOutput <- true
    p.StartInfo.RedirectStandardError <- true
    p.StartInfo.UseShellExecute <- false
    p.StartInfo.FileName <- cmd
    p.StartInfo.Arguments <- arg
    p.Start() |> ignore 

    // Add a timeout if needed
    let a = 
        match timeout with 
            | Option.None -> 
                true
            | Some t -> 
                p.WaitForExit(t :> int)

    if a then 
        let err = p.StandardError.ReadToEnd() 

        if err <> "" then 
            // An error in the systemCall. Report this error to the caller
            SystemCallError err
        else 
            // No error output
            let res = p.StandardOutput.ReadToEnd()

            p.Kill true

            SystemCallOutcome res
    else 
        // Kill the process
        p.Kill true
        SystemCallTimeout
        