-- FLP - Decision tree
-- Michal Ľaš (xlasmi00)
-- 09.03.2025
-- Main.hs: main of the flp-fun project

import System.Environment(getArgs)
import Control.Exception(catch, IOException)
import Data.List (delete)

import Types(DTree, FeatureValues, TrainData)
import Algorithms


-- Program help message
helpMessage :: String
helpMessage = "The valid program usage is:\nflp-fun -1 <path_to_file_with_tree> <path_to_file_with_new_data>\nflp-fun -2 <path_to_file_with_train_data>"

-- Read file error handler
handleReadError :: IOException -> IO String
handleReadError _ = error helpMessage


-- Load tree, features values and perform classification
actionClassify :: [String] -> IO()
actionClassify files = do
        tree_content <- catch (readFile $ files !! 0) handleReadError
        data_content <- catch (readFile $ files !! 1) handleReadError
        let dtree = read tree_content :: DTree
        let cdata = map read (lines data_content) :: [FeatureValues]
        mapM_ (putStrLn . classify dtree) cdata
        

-- Train decision tree on given labeled data
actionTrain :: [String] -> IO()
actionTrain file = do
        data_content <- catch (readFile $ head file) handleReadError
        let train_data = map read (lines data_content) :: TrainData  
        let tree = train train_data
        print tree
        

-- Choose action base on given program arguments
chooseAction :: [String] -> IO()
chooseAction [] = error helpMessage
chooseAction args
        | length args > 3 = error helpMessage
        | "-1" `elem` args = actionClassify $ delete "-1" args
        | "-2" `elem` args = actionTrain $ delete "-2" args
        | otherwise = error helpMessage



main :: IO()
main = do
        args <- getArgs
        chooseAction args


-- ////////////////////////////////////// END OF FILE /////////////////////////////////////// --