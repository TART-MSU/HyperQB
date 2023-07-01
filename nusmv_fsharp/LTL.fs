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

/// Module containing the definition of a LTL and HyperLTL formula and functions used to manipulate them. 
module Core.LTL 

open System
open System.IO
open FParsec

open Util
open RunConfiguration

/// Type of an LTL formula with atoms of type 'T
type LTL<'T when 'T: comparison> = 
    | Atom of 'T
    | True
    | False 
    | And of LTL<'T> * LTL<'T>
    | Or of LTL<'T> * LTL<'T>
    | Implies of LTL<'T> * LTL<'T>
    | Equiv of LTL<'T> * LTL<'T>
    | Xor of LTL<'T> * LTL<'T>
    | Not of LTL<'T>
    | X of LTL<'T>
    | F of LTL<'T>
    | G of LTL<'T>
    | U of LTL<'T> * LTL<'T>
    | W of LTL<'T> * LTL<'T>
    | M of LTL<'T> * LTL<'T>
    | R of LTL<'T> * LTL<'T>

    /// Print this LTl formula as a string as supported by spot
    member this.Print (varNames : 'T -> String) =
        match this with
            | Atom x -> varNames x
            | True -> "1"
            | False -> "0"
            | And(e1, e2) -> "(" + e1.Print varNames + " & " + e2.Print varNames + ")"
            | Or(e1, e2) -> "(" + e1.Print varNames + " | " + e2.Print varNames + ")"
            | Implies(e1, e2) -> "(" + e1.Print varNames + " -> " + e2.Print varNames + ")"
            | Equiv(e1, e2) -> "(" + e1.Print varNames + " <-> " + e2.Print varNames + ")"
            | Xor(e1, e2) -> "(" + e1.Print varNames + " xor " + e2.Print varNames + ")"
            | Not e -> "(! " + e.Print varNames + ")"
            | X e -> "(X " + e.Print varNames + ")"
            | F e -> "(F " + e.Print varNames + ")"
            | G e -> "(G " + e.Print varNames + ")"
            | U(e1, e2) -> "(" + e1.Print varNames + " U " + e2.Print varNames + ")"
            | W(e1, e2) -> "(" + e1.Print varNames + " W " + e2.Print varNames + ")"
            | M(e1, e2) -> "(" + e1.Print varNames + " M " + e2.Print varNames + ")"
            | R(e1, e2) -> "(" + e1.Print varNames + " R " + e2.Print varNames + ")"

    /// Map a function over this formula
    member this.Map (f : 'T -> 'U) = 
        match this with 
            | Atom x -> Atom (f x)
            | True -> True 
            | False -> False 
            | And(e1, e2) -> And(e1.Map f, e2.Map f)
            | Implies(e1, e2) -> Implies(e1.Map f, e2.Map f)
            | Equiv(e1, e2) -> Equiv(e1.Map f, e2.Map f)
            | Xor(e1, e2) -> Xor(e1.Map f, e2.Map f)
            | Or(e1, e2) -> Or(e1.Map f, e2.Map f)
            | U(e1, e2) -> U(e1.Map f, e2.Map f)
            | W(e1, e2) -> W(e1.Map f, e2.Map f)
            | M(e1, e2) -> M(e1.Map f, e2.Map f)
            | R(e1, e2) -> R(e1.Map f, e2.Map f)
            | F e -> F(e.Map f)
            | G e -> G(e.Map f)
            | X e -> X(e.Map f)
            | Not e -> Not(e.Map f)

    /// Bind a function over this formula, i.e., replace atoms with arbitrary LTL formulas
    member this.Bind (f : 'T -> LTL<'U>) = 
        match this with 
            | Atom x -> f x
            | True -> True 
            | False -> False 
            | And(e1, e2) -> And(e1.Bind f, e2.Bind f)
            | Implies(e1, e2) -> Implies(e1.Bind f, e2.Bind f)
            | Equiv(e1, e2) -> Equiv(e1.Bind f, e2.Bind f)
            | Xor(e1, e2) -> Xor(e1.Bind f, e2.Bind f)
            | Or(e1, e2) -> Or(e1.Bind f, e2.Bind f)
            | U(e1, e2) -> U(e1.Bind f, e2.Bind f)
            | W(e1, e2) -> W(e1.Bind f, e2.Bind f)
            | M(e1, e2) -> M(e1.Bind f, e2.Bind f)
            | R(e1, e2) -> R(e1.Bind f, e2.Bind f)
            | F e -> F(e.Bind f)
            | G e -> G(e.Bind f)
            | X e -> X(e.Bind f)
            | Not e -> Not(e.Bind f)

    /// Compute all atoms in this formula
    member this.AllAtoms = 
        match this with 
            | Atom x -> Set.singleton x
            | True | False -> Set.empty 
            | And(e1, e2) | Implies(e1, e2) | Equiv(e1, e2) | Xor(e1, e2) | Or(e1, e2) | U(e1, e2) | W(e1, e2) | M(e1, e2) | R(e1, e2) -> Set.union e1.AllAtoms e2.AllAtoms
            | F e | G e | X e | Not e -> e.AllAtoms


/// A HyperLTL formula consists of the LTL matrix and the quantifier prefix.
/// The LTL-body consists of atoms over 'T * int where the index refers to the quantifiers in the prefix (beginning by 0 for the outermost quantifier)
type HyperLTL<'T when 'T: comparison> = 
    {
        QuantifierPrefix : list<TraceQuantifierType>
        LTLMatrix : LTL<'T * int>
    }
    
    member this.IsConsistent () =
            this.LTLMatrix.AllAtoms
            |> Set.forall (fun (_, i) -> i <= this.QuantifierPrefix.Length)
            
    /// Print this HyperLTL formula as a String
    member this.Print (varNames : 'T -> String) =
        let strWriter = new StringWriter()
        for t in this.QuantifierPrefix do
            match t with 
                | FORALL -> strWriter.Write("forall ")
                | EXISTS -> strWriter.Write("exists ")

        let varStringer (x, i) = "\"" + varNames x + "\"_" + string(i) 

        strWriter.Write(this.LTLMatrix.Print varStringer)

        strWriter.ToString()

/// HyperLTL where the quantifier are bound to explicit names.
/// The prefix consists of pairs (Q, n) where n gives a name to the quantified trace.
/// The LTL-body consist of atoms over T * String where the string index matches that in the prefix
type NamedHyperLTL<'T when 'T: comparison> = 
    {
        QuantifierPrefix : list<TraceQuantifierType * String>
        LTLMatrix : LTL<'T * String>
    }

    /// Convert to a (non-named) HyperLTL formula
    member this.ToHyperLTL() = 
        let names = this.QuantifierPrefix |> List.map snd

        let nameMap = 
            names 
            |> List.mapi (fun i n -> n, i)
            |> Map.ofList

        {
            HyperLTL.QuantifierPrefix = this.QuantifierPrefix |> List.map fst 
            LTLMatrix = this.LTLMatrix.Map (fun (x, n) -> (x, nameMap.[n]))
        }

    /// Print the formula as a String
    member this.Print (varNames : 'T -> String)  =
        let strWriter = new StringWriter()
        for t in this.QuantifierPrefix do
            match t with 
                | FORALL, x -> strWriter.Write("forall " + x + ".")
                | EXISTS, x -> strWriter.Write("exists " + x + ".")

        let varStringer (x, i) = "\"" + varNames x + "\"_" + i

        strWriter.Write(this.LTLMatrix.Print varStringer)

        strWriter.ToString()

/// Given the quantifier prefix of a HyperLTL formula, extract the prefix in the more succinct block format.
/// Consecutive quantifiers of the same type are summarized by their number.
/// For example, the block-representation of "FORALL FORALL EXISTS FORALL" would be [2, 1, 1].
let extractBlocks (qf : list<TraceQuantifierType>) = 
    let rec helper t count q = 
        match q with 
            | [] -> [count]
            | x::xs -> 
                if x = t then 
                    helper t (count + 1) xs
                else 
                    count::helper x 1 xs

    helper qf.[0] 0 qf

module Parser =
    /// Parser for an LTL formula when given a parser for its atoms
    let ltlParser (atomParser : Parser<'T, unit>) = 
        let ltlParser, ltlParserRef = createParserForwardedToRef()

        let trueParser = 
            stringReturn "1" True

        let falseParser = 
            stringReturn "0" False 

        let variableParser = 
            atomParser
            |>> Atom

        let parParser = 
            skipChar '(' >>. spaces >>. ltlParser .>> spaces .>> skipChar ')'

        let basicParser = 
            spaces >>. choice [ 
                trueParser
                falseParser
                variableParser
                parParser
            ] .>> spaces

        let oppLtl = new OperatorPrecedenceParser<LTL<'T>, unit, unit>()

        let addInfixOperator string precedence associativity f =
            oppLtl.AddOperator(
                InfixOperator(string, spaces, precedence, associativity, f)
            )

        let addPrefixOperator string precedence associativity f =
            oppLtl.AddOperator(
                PrefixOperator(string, spaces, precedence, associativity, f)
            )

        do
            oppLtl.TermParser <- basicParser

            addInfixOperator "&" 5 Associativity.Left (fun x y -> And(x, y))
            addInfixOperator "|" 4 Associativity.Left (fun x y -> Or(x, y))
            addInfixOperator "->" 3 Associativity.Left (fun x y -> Implies(x, y))
            addInfixOperator "<->" 2 Associativity.None (fun x y -> Equiv(x, y))
            addInfixOperator "xor" 2 Associativity.None (fun x y -> Xor(x, y))
            addInfixOperator "U" 10 Associativity.Left (fun x y -> U(x, y))
            addInfixOperator "W" 10 Associativity.Left (fun x y -> W(x, y))
            addInfixOperator "M" 10 Associativity.Left (fun x y -> M(x, y))
            addInfixOperator "R" 10 Associativity.Left (fun x y -> R(x, y))

            addPrefixOperator "F" 20 true (fun x -> F x)
            addPrefixOperator "G" 20 false (fun x -> G x)
            addPrefixOperator "X" 30 true (fun x -> X x)
            addPrefixOperator "!" 40 true (fun x -> Not x)

        do 
            ltlParserRef := oppLtl.ExpressionParser

        ltlParser

    let private prefixParser = 
        let eParser = stringReturn "exists" EXISTS
        let uParser = stringReturn "forall" FORALL
        
        spaces >>.
        many1 ((eParser <|> uParser) .>> spaces)

    let namedPrefixParser = 
        let eParser = 
            skipString "exists " >>. spaces >>. (many1Chars letter) .>> spaces .>> pchar '.'
            |>> fun x -> EXISTS, x

        let uParser = 
            skipString "forall " >>. spaces >>. (many1Chars letter) .>> spaces .>> pchar '.'
            |>> fun x -> FORALL, x
        
        spaces >>.
        many1 ((eParser <|> uParser) .>> spaces)

    let private hyperltlParser (atomParser : Parser<'T, unit>) = 
        let ap : Parser<'T * int, unit>= 
            atomParser .>> pchar '_' .>>. pint32

        pipe2 
            prefixParser
            (ltlParser ap)
            (fun x y -> {HyperLTL.QuantifierPrefix = x; LTLMatrix = y})

    let private namedHyperltlParser (atomParser : Parser<'T, unit>) = 
        let ap : Parser<'T * String, unit>= 
            atomParser .>> pchar '_' .>>. (many1Chars letter)

        pipe2 
            namedPrefixParser
            (ltlParser ap)
            (fun x y -> {NamedHyperLTL.QuantifierPrefix = x; LTLMatrix = y})

    /// Parses a LTL formula from a String
    let parseLTL (atomParser : Parser<'T, unit>) s =    
        let full = ltlParser atomParser .>> spaces .>> eof
        let res = run full s
        match res with
        | Success (res, _, _) -> Result.Ok res
        | Failure (err, _, _) -> Result.Error err

    /// Parses a HyperLTL formula from a String
    let parseHyperLTL (atomParser : Parser<'T, unit>) s =    
        let full = hyperltlParser atomParser .>> spaces .>> eof
        let res = run full s
        match res with
        | Success (res, _, _) -> Result.Ok res
        | Failure (err, _, _) -> Result.Error err

    /// Parses a Named HyperLTL formula from a String
    let parseNamedHyperLTL (atomParser : Parser<'T, unit>) s =    
        let full = namedHyperltlParser atomParser .>> spaces .>> eof
        let res = run full s
        match res with
        | Success (res, _, _) -> Result.Ok res
        | Failure (err, _, _) -> Result.Error err

/// Module for the random generation of HyperLTL formulas using spot's randltl
module Random = 

    /// Given a seed, size bounds, a list of aps and a number of samples, call spot's randltl to generate random LTL formulas
    let private invokeRandLtl config (seed : int) (sizel : int, sizeu : int)  (aps : list<string>) (numberOfSamples : int) = 
    
        let args = "--seed=" + string(seed) + " --tree-size=" + string(sizel) + ".." + string(sizeu) + " --ltl-priorities \"xor=0,M=0\"" + " -p -n" + string(numberOfSamples) + (List.fold (fun s x -> s + " " + x) "" aps)
        let res = 
            match Util.systemCall (config.SolverConfig.SpotPath + "randltl") args Option.None with 
                | SystemCallOutcome x -> Result.Ok x 
                | SystemCallError msg -> 
                    Result.Error ("Unexpected error with spot during random formula generation: " + msg)
                | SystemCallTimeout -> 
                    // Unreachable as no timeout was set
                    exit 0 

        res 
        |> Result.bind (fun res -> 
            let lines = 
                res.Split('\n')
                |> Array.toList
                |> List.filter (fun x -> x <> "")

            let asLtL = 
                lines 
                |> List.map (fun x -> Parser.parseLTL Util.ParserUtil.relVarParser x)

            if List.exists Result.isError asLtL then 
                Result.Error "Some of the LTL formulas returned by spot could not be parsed."
            else 
                asLtL
                |> List.map (fun x -> Result.defaultWith (fun _ -> failwith "") x)
                |> Result.Ok
            )

    /// Generates random HyperLTL formulas with a given quantifier prefix
    let getRandomHyperLTL config (prefix : list<TraceQuantifierType>) (sizel, sizeu) (aps : list<String>) numberOfSamples = 
        let indexed_aps = 
            List.allPairs aps [0..prefix.Length-1]
            |> List.map (fun (x, i) -> x + "_" + string(i))
            
        let seed = Util.rnd.Next() 

        let ltlFormulas = invokeRandLtl config seed (sizel, sizeu) indexed_aps numberOfSamples

        let hyperltlFormulas = 
            ltlFormulas
            |> Result.map (fun ltlFormulas -> 
                ltlFormulas
                |> List.map (fun x -> {HyperLTL.QuantifierPrefix = prefix; LTLMatrix = x})
                )

        hyperltlFormulas

    /// Generates random HyperLTL formulas with a given quantifier prefix but calls spot once for each sample. Multiple formulas can thus be identical.
    let getRandomHyperLTLOneByOne config (prefix : list<TraceQuantifierType>) (sizel, sizeu) (aps : list<String>) numberOfSamples = 
        let indexed_aps = 
            List.allPairs aps [0..prefix.Length-1]
            |> List.map (fun (x, i) -> x + "_" + string(i))

        let ltlFormulas = 
            List.init numberOfSamples 
                (fun _ -> 
                    let seed = Util.rnd.Next() 
                    let formulas = invokeRandLtl config seed (sizel, sizeu) indexed_aps 10

                    formulas
                    |> Result.map (fun formulas -> formulas.[Util.rnd.Next(10)])
                )

        match List.tryFind Result.isError ltlFormulas with 
            | Some (Result.Error x) -> 
                Result.Error ("Could not obtain randltl formulas: " + x)
            | _ -> 
                ltlFormulas
                |> List.map (fun x -> Result.defaultWith (fun _ -> failwith "") x)
                |> List.map (fun x -> {HyperLTL.QuantifierPrefix = prefix; LTLMatrix = x})
                |> Result.Ok
            