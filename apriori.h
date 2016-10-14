#ifndef _INC_APRIORI
#define _INC_APRIORI

#include <iostream> 
#include <iomanip> 
#include <sstream> 
#include <fstream>
#include <string> 
#include <vector> 
#include <unordered_map>
#include <math.h>
#include "MT.h"
// #include "jsoncons/json.hpp"
// using jsoncons::json;
// using jsoncons::json_deserializer;

// #include <boost/filesystem.hpp>
// namespace fs = boost::filesystem;
// #include <boost/algorithm/string/predicate.hpp>

namespace TestCode{

    class Apriori
    {
        public:
        
        /* Class Methods */
//         void runApriori(std::string fnameData, std::string fnameLog, double minsup, double alpha, bool cacheflag, bool pruneflag, bool dummyflag);
        void runApriori(std::string fnameData, double minsup, double alpha, bool cacheflag, bool pruneflag, bool dummyflag);
        void loadData();
        void candidateGenerator(int length);
        void dummyGenerator(int length);
        void checkSupport();
        std::vector<int> estimatePatterns(int length);
        std::vector<int> countSupport(std::vector<int> cntVec);
        std::vector<std::vector<int>> getSubsets(std::vector<int> pattern);
        bool isSameSet(std::vector<int> set1, std::vector<int> set2);
        bool checkSetInSets(std::vector<int> keyset, std::vector<std::vector<int>> sets);
        bool checkFreqOfSubsets(std::vector<std::vector<int>> subsets,
                std::vector<std::vector<int>> searchFrom);

        /* access private variables */
        /* Set */ 
        void setDataFile(std::string fname) { this->fnameData = fname; };
        void setLogFile(std::string fname) { this->fnameLog = fname; };
        void setNumTrans(int ntrans) { this->ntrans = ntrans; };
        void setNumItems(int nitems) { this->nitems = nitems; };
        void setMinSup(double ms) { this->minsup = ms; };
        void setThreshold() { this->threshold = ceil(minsup*ntrans); };
        void setAlpha(double alpha) { this->alpha = alpha; };
        void setCacheFlag(bool flag) { this->calcCacheFlag = flag; };
        void setPruneFlag(bool flag) { this->hasPruneStep = flag; };
        void setDummyFlag(bool flag) { this->hasDummySet = flag; };
        /* Get */ 
        std::string getDataFile() { return this->fnameData; };
        std::string getLogFile() { return this->fnameLog; };
        int getNumTrans() { return this->ntrans; };
        int getNumItems() { return this->nitems; };
        double getMinSup() { return this->minsup; };
        double getThreshold() { return this->threshold; };
        double getAlpha() { return this->alpha; };
        bool getCacheFlag() { return this->calcCacheFlag; };
        bool getPruneFlag() { return this->hasPruneStep; };
        bool getDummyFlag() { return this->hasDummySet; };

        int getCandItemSetSize() { return this->candItemSets.size(); };
        int getFreqItemSetSize() { return this->freqItemSets.size(); };
        int getInfreqItemSetSize() { return this->infreqItemSets.size(); };
        int getDummyItemSetSize() { return this->dummyItemSets.size(); };
        int getCandSupSize() { return this->candSup.size(); };
        
        /* cleaning vector, matrix */
        void clearCandSupports() { this->candSup.clear(); };
        void clearCandItemSets() { this->candItemSets.clear(); };
        void clearFreqItemSets() { this->freqItemSets.clear(); };
        void clearInfreqItemSets() { this->infreqItemSets.clear(); };
        void clearDummyItemSets() { this->dummyItemSets.clear(); };


        private:
        std::string fnameData, fnameLog;
        int ntrans, nitems;
        double minsup, threshold, alpha; 
        bool calcCacheFlag, hasPruneStep, hasDummySet;
        std::vector<int> candSup;
        std::vector<std::vector<int>> candItemSets;
        std::vector<std::vector<int>> freqItemSets;
        std::vector<std::vector<int>> infreqItemSets;
        std::vector<std::vector<int>> dummyItemSets;
        std::vector<std::vector<std::vector<int>>> resultFreqItemSets;
        std::vector<std::vector<int>> plainData;
        std::unordered_map<std::string, int> calcCache;

    }; // class Apriori

    class Util
    {
        public:
        std::vector<int> split(std::string &str, char delim=char(32));
        std::string vectorStringJoin(std::vector<int> v, int size);

        template <class T>
        void printVec(std::vector<T> vec); // 1 dim
        void printMat(std::vector<std::vector<int>> mat); // 2 dim
        void print3d(std::vector<std::vector<std::vector<int>>> vec); // 3 dim

    }; // class Util 

//     class Log
//     { 
//         public:
//         std::string setParam(std::string fnameData, std::string fnameLog, 
//                 int ntrans, int nitems,
//                 double mimsup, double threshold, double alp, 
//                 bool calcCacheFlag, bool hasPruneStep, bool hasDummySet);
//         std::string setEvalResult(int i_roop, 
//                 int n_cand_itemset_wo_dummy, int n_dummy_itemset, 
//                 int n_cand_itemset_w_dummy, int n_freq_itemset,  
//                 int n_cache_wo_prune, int n_cache_w_prune);
//         std::string setTotalResult(double sum1, double sum2, double saved);
//
// //         bool saveLogToJsonFile(std::string str);
//         void saveLog(std::string str);
//         void setFile(std::string filename) { this->fnameLog = filename; };
//
//         /* summarization */
// //         json result;
//         void initResult(){ 
//             this->result["roop"] = result;
//         };
//
//         private:
//         std::string fnameData, fnameLog;
//         int ntrans, nitems;
//         double minsup, threshold, alpha; 
//         bool calcCacheFlag, hasPruneStep, hasDummySet;
//
//     }; // class Log

} //namespace TestCode

#endif
