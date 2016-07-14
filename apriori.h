#ifndef _INC_APRIORI
#define _INC_APRIORI

namespace TestCode{

    class Apriori
    {
        public:
        
        /* Class Methods */
        void runApriori(std::string filename, double minsup);
        void loadData();
        void candidateGenerator(int length);
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
        void setFile(std::string fname) { this->filename = fname; };
        void setNumTrans(int ntrans) { this->ntrans = ntrans; };
        void setNumItems(int nitems) { this->nitems = nitems; };
        void setMinSup(double ms) { this->minsup = ms; };
        void setThreshold() { this->threshold = ceil(minsup * ntrans); };
        /* Get */ 
        std::string getFile() { return this->filename; };
        int getNumTrans() { return this->ntrans; };
        int getNumItems() { return this->nitems; };
        double getMinSup() { return this->minsup; };
        double getThreshold() { return this->threshold; };
        int getCandItemSetSize() { return this->candItemSets.size(); };
        int getFreqItemSetSize() { return this->freqItemSets.size(); };
        int getCandSupSize() { return this->candSup.size(); };
        
        /* cleaning vector, matrix */
        void clearCandSupports() { this->candSup.clear(); };
        void clearCandItemSets() { this->candItemSets.clear(); };
        void clearFreqItemSets() { this->freqItemSets.clear(); };


        private:
        std::string filename;
        int ntrans, nitems;
        double minsup, threshold; 
        bool calcCacheFlag = true;
        std::vector<int> candSup;
        std::vector<std::vector<int>> candItemSets;
        std::vector<std::vector<int>> freqItemSets;
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

} //namespace TestCode

#endif
