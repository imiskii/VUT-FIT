-- FLP - Decision tree
-- Michal Ľaš (xlasmi00)
-- 09.03.2025
-- Types.hs: Definition of different types used in FLP - Decision Tree project


module Types(
    Index, Threshold, Label,
    DTree(..),
    FeatureValues(..),
    getNth,
    TrainLine(..), TrainData,
    getFeatureValues,
    getFeatureValue,
    getTrainLineLabel,
    getFeatureCount,
    allLabelsEqual
) where


import Data.Array

import Helpers


-- Alias type representing index of a feature
type Index = Int
-- Alias type representing threshold of a decision
type Threshold = Double
-- Alias type representing name of a leaf (label)
type Label = String


-- ////////////////////////////////////////////////////////////////////////////////////////// --
-- ////////////////////////////////////// Decision Tree ///////////////////////////////////// --
-- ////////////////////////////////////////////////////////////////////////////////////////// --

-- Program help message
malformedDTreeError :: String
malformedDTreeError = "Mlaformed decision tree on input!"


-- Data type representing decision tree. Leaf has just string identifier called Label, Node has index, threshold, right and left sub tree
data DTree = Leaf Label | Node Index Threshold DTree DTree


-- Derived show function
instance Show DTree where
--  show :: DTree -> String
    show = showTree 0
      where
        showTree :: Int -> DTree -> String
        showTree indent (Leaf label) = replicate indent ' ' ++ "Leaf: " ++ label
        showTree indent (Node idx threshold left right) =
            replicate indent ' ' ++ "Node: " ++ show idx ++ ", " ++ show threshold ++ "\n"
            ++ showTree (indent + 2) left ++ "\n"
            ++ showTree (indent + 2) right


-- Derived read function
instance Read DTree where
--  readsPrec :: Int -> ReadS DTree
    readsPrec _ raw = uncurry (parseDTree . splitTreeLine) (takeLine raw)


-- Function to parse Decision Tree based on splited lines
-- [String]: parsed part 
-- String: rest of the tree
parseDTree :: [String] -> String -> [(DTree, String)]
parseDTree ("Node:":node_rest) tree_rest = parseNode node_rest tree_rest
parseDTree ("Leaf:":leaf_rest) tree_rest = parseLeaf leaf_rest tree_rest
parseDTree _ _ = error malformedDTreeError

-- Parse leaf of a tree
-- [String]: parsed part 
-- String: rest of the tree
parseLeaf :: [String] -> String -> [(DTree, String)]
parseLeaf (label:_) tree_rest = [(Leaf label, tree_rest)]
parseLeaf _ _ = error malformedDTreeError

-- Parse node of a tree
-- [String]: parsed part 
-- String: rest of the tree
parseNode :: [String] -> String -> [(DTree, String)]
parseNode (idx:threshold:_) tree_rest
    | [(i, "")] <- reads idx :: [(Index, String)]
    , [(t, "")] <- reads threshold :: [(Threshold, String)]
    , [(leftDTree, restAfterLeft)] <- uncurry (parseDTree . splitTreeLine) (takeLine tree_rest) :: [(DTree, String)]
    , [(rightDTree, restAfterRight)] <- uncurry (parseDTree . splitTreeLine) (takeLine restAfterLeft) :: [(DTree, String)]
    = [(Node i t leftDTree rightDTree, restAfterRight)]
parseNode _ _ = error malformedDTreeError


-- Parse string into elements of a three (marg: ["Node:"/"Leaf:"], in case of leaf its label, in case of node its ["index"], ["threshold"])
splitTreeLine :: String -> [String]
splitTreeLine [] = []
splitTreeLine line
    | t == "Node:" = t : splitOn ',' rest
    | t == "Leaf:" = [t, rest]
    | otherwise = error malformedDTreeError
    where
        (t, rest) = splitOnFirst ' ' (trim line)

-- ////////////////////////////////////////////////////////////////////////////////////////// --
-- ////////////////////////////////////// Classify data ///////////////////////////////////// --
-- ////////////////////////////////////////////////////////////////////////////////////////// --


-- Data type representing feature values (either for classification or training)
-- Uses Array type for faster access to indexes
newtype FeatureValues = FeatureValues (Array Index Double)
    deriving(Show)


-- Derived read function for FeatureValues
instance Read FeatureValues where
--    readsPrec :: Int -> ReadS FeatureValues
    readsPrec _ raw_line = 
        let parsed_data = (\a -> listArray (0, length a - 1) a) (map read (splitOn ',' raw_line) :: [Double])
        in [(FeatureValues parsed_data, "")]


-- Get n-th item from FeatureValues (just like "!!" function in Prelude)
getNth :: FeatureValues -> Index -> Double
getNth (FeatureValues xs) idx = xs ! idx

-- Get number of feature values
getLength :: FeatureValues -> Int
getLength (FeatureValues feature_vals) = snd (bounds feature_vals) + 1

-- ////////////////////////////////////////////////////////////////////////////////////////// --
-- /////////////////////////////////////// Train data /////////////////////////////////////// --
-- ////////////////////////////////////////////////////////////////////////////////////////// --

-- Data type representing training value - labeled features
data TrainLine = TrainLine FeatureValues Label
    deriving(Show)

-- Alias for list of labeled features
type TrainData = [TrainLine]


-- Derived read function for TrainLine
instance Read TrainLine where
--  readsPrec :: Int -> ReadS TrainLine
    readsPrec _ raw_line = 
        let (feature_vals, lbl) = (read line_rest, label) :: (FeatureValues, Label)
        in [(TrainLine feature_vals lbl, "")]
        where
            (line_rest, label) = splitOnLast ',' raw_line



getFeatureValues :: TrainLine -> FeatureValues
getFeatureValues (TrainLine feature_vals _) = feature_vals

getFeatureValue :: TrainLine -> Index -> Double
getFeatureValue (TrainLine feature_vals _) = getNth feature_vals

getTrainLineLabel :: TrainLine -> Label
getTrainLineLabel (TrainLine _ lable) = lable

getFeatureCount :: TrainLine -> Int
getFeatureCount (TrainLine feature_vals _) = getLength feature_vals

allLabelsEqual :: TrainData -> Bool
allLabelsEqual train_data = all (== label) labels
    where
        label = getTrainLineLabel $ head train_data
        labels = map getTrainLineLabel train_data


-- ////////////////////////////////////// END OF FILE /////////////////////////////////////// --