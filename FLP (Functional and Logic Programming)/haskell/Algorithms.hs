-- FLP - Decision tree
-- Michal Ľaš (xlasmi00)
-- 09.03.2025
-- Algorithms: Implementation of the functions for classification of values with decision tree (classify) and training new decision tree with labeled data


module Algorithms(
    classify,
    train
) where


import Data.List (nub, group, sort, sortBy, maximumBy, partition)
import Data.Ord (comparing)

import Types


-- ////////////////////////////////////////////////////////////////////////////////////////// --
-- //////////////////////////// Classify data with Decision Tree //////////////////////////// --
-- ////////////////////////////////////////////////////////////////////////////////////////// --


-- Classify feature data based on given decision tree
classify :: DTree -> FeatureValues -> Label
classify (Leaf class_name) _ = class_name
classify (Node index threshold left right) classify_data = classify (if getNth classify_data index <= threshold then left else right) classify_data


-- ////////////////////////////////////////////////////////////////////////////////////////// --
-- ////////////////////////////// Train Decision Tree with data ///////////////////////////// --
-- ////////////////////////////////////////////////////////////////////////////////////////// --

-- Types aliases for better reading
type Impurity = Double
type InformationGain = Double

-- Train new decision tree based on given labeled features
train :: TrainData -> DTree
train [] = error "There are no data to perform training!"
train train_data
    | allLabelsEqual train_data = Leaf (getTrainLineLabel $ head train_data)
    | otherwise = Node idx threshold (train left_train_data) (train right_train_data)
    where
        (idx, threshold, (left_train_data, right_train_data)) = chooseLeadingFeature train_data


-- Choose the best feature with highest information gain (index), the best threshold selection for this feature (threshold) and data split based on the threshold
-- TrainData: labeled features data
-- Returns: index, threshold and tuple (left data split, right data split)
chooseLeadingFeature :: TrainData -> (Index, Threshold, (TrainData, TrainData))
chooseLeadingFeature [] = error "There are no data from which leading feature can be choosen!"
chooseLeadingFeature train_data = (best_idx, best_threshold, best_data_split)
    where
        feature_count = getFeatureCount $ head train_data
        -- Count information gain before spliting
        base_info_gain = countGiniImpurity $ map getTrainLineLabel train_data
        -- Generate list of tuples: (index, information gain, threshold, data split)
        candidates = [(idx, info_gain, threshold, data_split)
                     | idx <- [0 .. feature_count - 1], let (info_gain, threshold, data_split) = chooseBestFeatureSplit idx train_data base_info_gain]
        -- Pick feature with maximal information gain
        (best_idx, _, best_threshold, best_data_split) = maximumBy (comparing snd4) candidates
        snd4 (_, b, _, _) = b


-- Choose the best information gain from all possible splits of one feature
-- Index: index of feature (column)
-- TrainData: labeled features data
-- Returns: information gain of the best split, its threshold and tuple (left data split, right data split)
chooseBestFeatureSplit :: Index -> TrainData -> InformationGain -> (InformationGain, Threshold, (TrainData, TrainData))
chooseBestFeatureSplit idx train_data base_info_gain = 
    if length unique_feature_values <= 1 then
        -- If there is only one unique value of the feature, but multiple labels there is 0 information gain and split is made by the labels
        (0, head unique_feature_values, splitTrainDataByFirstLabel sorted_data)
    else
        (best_info_gain, best_threshold, best_data_split)
    where
        sorted_data = sortBy (comparing (`getFeatureValue` idx)) train_data
        -- Count possible thresholds
        unique_feature_values = nub $ map (`getFeatureValue` idx) sorted_data
        thresholds = zipWith (\a b -> (a + b) / 2.0) unique_feature_values (tail unique_feature_values)
        -- Generate list of tuples: (information gain, threshold, data split) for all thresholds
        candidates = [(base_info_gain - impurity, threshold, data_split)
                     | threshold <- thresholds, let (impurity, data_split) = countSplitGiniImpurity idx threshold sorted_data]

        -- Pick tuple with maximal information gain
        (best_info_gain, best_threshold, best_data_split) = maximumBy (comparing fst3) candidates --foldl (\acc next -> if fst3 next > fst3 acc then next else acc) (-1,-1,([],[])) candidates
        fst3 (a, _, _) = a


-- Count gini impurity for a split by index of feature in train data and threshold value
countSplitGiniImpurity :: Index -> Threshold -> TrainData -> (Impurity, (TrainData, TrainData))
countSplitGiniImpurity idx threshold train_data = (impurity, (left, right))
    where
        (left, right) = splitTrainData idx threshold train_data
        left_labels = map getTrainLineLabel left
        right_labels = map getTrainLineLabel right
        len_left = fromIntegral $ length left
        len_right = fromIntegral $ length right
        total_len = len_left + len_right
        -- veight impurity between left and right splits
        impurity = (countGiniImpurity left_labels * len_left + countGiniImpurity right_labels * len_right) / total_len


-- Split train data into two groups based on feature index and threshold
-- Data has to be sorted from min to max on choosen index !!!
splitTrainData :: Index -> Threshold -> TrainData -> (TrainData, TrainData)
splitTrainData _ _ [] = ([], [])
splitTrainData idx threshold train_data@(line:rest)
    | getFeatureValue line idx <= threshold = (line:line', rest')
    | otherwise = ([], train_data)
    where
        (line', rest') = splitTrainData idx threshold rest


-- Split train data into two groups based on the first label
-- The first group fill have only the same labels the second group can be mixed
splitTrainDataByFirstLabel :: TrainData -> (TrainData, TrainData)
splitTrainDataByFirstLabel [] = ([], [])
splitTrainDataByFirstLabel train_data@(line:_) = partition (\a -> getTrainLineLabel a == getTrainLineLabel line) train_data


-- Count gini impurity for list of labels
countGiniImpurity :: [Label] -> Threshold
countGiniImpurity labels = 1.0 - sum_p
    where
        count = fromIntegral $ length labels
        lable_counts = map (fromIntegral . length) (group $ sort labels)
        class_scores = map (/ count) lable_counts
        sum_p = sum $ map (**2) class_scores
        

-- ////////////////////////////////////// END OF FILE /////////////////////////////////////// --