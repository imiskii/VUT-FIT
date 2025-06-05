-- FLP - Decision tree
-- Michal Ľaš (xlasmi00)
-- 09.03.2025
-- Helpers.hs: general help function for the main functionality


module Helpers(
    splitOn,
    splitOnFirst,
    splitOnLast,
    trim,
    takeLine
)where


-- Split list of chars by choosen delimeter
splitOn :: Char -> String -> [String]
splitOn delimeter line = (\a -> if null $ head a then tail a else a) $ foldr glue [[]] line
    where
        glue _ [] = []
        glue c acc@(x:xs)
            | c == delimeter = if null x then acc else [] : acc
            | otherwise = (c : x) : xs


-- Split on the last occurence of a character
splitOnLast :: Char -> String -> (String, String)
splitOnLast delimeter = foldr glue ("", "")
    where
        glue c (right, left)
            | c == delimeter && null left = ([], right)
            | otherwise = (c : right, left)


-- Split on the first occurence of a character
splitOnFirst :: Char -> String -> (String, String)
splitOnFirst _ [] = ([],[])
splitOnFirst delimeter (c:rest)
    | c == delimeter = ("", rest)
    | otherwise = (\a -> (c : fst a, snd a)) $ splitOnFirst delimeter rest


-- Remove leading spaces
trim :: String -> String
trim [] = []
trim str@(x:xs)
    | x == ' ' = trim xs
    | otherwise = str


-- Take one line (separated by '\n') from a string and ruturns tuple (line:rest of the string)
takeLine :: String -> (String, String)
takeLine [] = ([],[])
takeLine (c:rest)
    | c == '\n' = ([], rest)
    | otherwise = (\a -> (c : fst a, snd a)) $ takeLine rest


-- ////////////////////////////////////// END OF FILE /////////////////////////////////////// --