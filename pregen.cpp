#include "apriori.h"

//using namespace TestCode;
/*** How APRIORI Works ***
 * Find all frequent itemsets:
 * Get frequent items:
 *   Items whose occurrence in database is greater than or equal to the min.support threshold.
 * Get frequent itemsets:
 *   Generate candidates from frequent items.
 *   Prune the results to find the frequent itemsets.
 * Generate strong association rules from frequent itemsets
 * Rules which satisfy the min.support and min.confidence threshold.
 */

// TODO 1: func. candidateGenerator, estimatePatterns が似ているから一つの関数にできる。
// TODO 2: dummy set を加えるfunc.をつくる
// TODO 3: improve aprioriを実装したい。
// TODO 4: estimateが非常に重たい計算になる。高速化したい。


/* ========================  Class Util ===================================*/

std::vector<int> TestCode::Util::split(std::string &str, char delim) 
{
    std::istringstream iss(str);
    std::string tmp;
    std::vector<int> res;
    while(getline(iss, tmp, delim))
    {
        res.push_back(std::stoi(tmp));
    }
    return res;
}


std::string TestCode::Util::vectorStringJoin(std::vector<int> v, int size)
{
    std::stringstream ss;
    if (size < 0){
        size = v.size();
    }
    for (int i = 0; i < size; ++i){
        if (i != 0) ss << ",";
        ss << v[i];
    }
    return ss.str();
}


/*--- PRINT ----*/
// 1 dimension
template <typename T>
void TestCode::Util::printVec(std::vector<T> vec){
    for (int i = 0; i < vec.size(); i++){
        std::cout << vec[i] << ","; 
    }
    std::cout << "\n";
}

// 2 dimension
void TestCode::Util::printMat(std::vector<std::vector<int>> mat)
{
    for (int i = 0; i < mat.size(); i++){
        this->printVec(mat[i]);
    }
    std::cout << "\n";
}

// 3 dimension
void TestCode::Util::print3d(std::vector<std::vector<std::vector<int>>> vec){
    std::cerr << std::endl; 
    for (int i = 0; i < vec.size(); i++){
        this->printMat(vec[i]);
    }
    std::cerr << std::endl; 
}


/* ======================== / Class Util ===================================*/

/* ========================  Class Log ===================================*/

void TestCode::Log::saveLog(std::string str) 
{
    std::fstream ofs(this->fnameLog.c_str(), std::ios::app);
    ofs << str << std::endl;
    ofs.close();
}

std::string TestCode::Log::setParam(std::string fnameData, std::string fnameLog, 
        int ntrans, int nitems,
        double minsup, double threshold, double alpha, 
        bool calcCacheFlag, bool hasPruneStep, bool hasDummySet)
{
    this->fnameData = fnameData;
    this->fnameLog = fnameLog;
    this->ntrans = ntrans;
    this->nitems = nitems;
    this->minsup = minsup;
    this->threshold = threshold;
    this->alpha = alpha;
    this->calcCacheFlag = calcCacheFlag;
    this->hasPruneStep = hasPruneStep;
    this->hasDummySet = hasDummySet;

    std::ostringstream oss;
    json v;

    v["fname_data"] = fnameData;
    v["fname_log"] = fnameLog;
    v["ntrans"] = ntrans; 
    v["nitems"] = nitems;
    v["minsup"] = minsup;  
    v["threshold"] = threshold;
    v["alpha"] = alpha;
    v["flag_cache"] = calcCacheFlag;
    v["flag_prune"] = hasPruneStep;
    v["flag_dummy"] = hasDummySet;

    /*save all in one place */
    this->result["params"] = v;

    /*so that can return indipendently*/
    oss << v << std::endl;
    return oss.str();
}

std::string TestCode::Log::setEvalResult(int i_roop, 
        int n_cand_itemset_wo_dummy, int n_dummy_itemset, 
        int n_cand_itemset_w_dummy, int n_freq_itemset,  
        int n_cache_wo_prune, int n_cache_w_prune)
{

    double saved = (n_cache_wo_prune - n_cache_w_prune)*100/n_cache_wo_prune;

    std::ostringstream oss;
    json v;

    v["ith_roop"] = i_roop;
    v["n_cand_itemset_wo_dummy"] = n_cand_itemset_wo_dummy;
    v["n_dummy_itemset"] = n_dummy_itemset; 
    v["n_cand_itemset_w_dummy"] = n_cand_itemset_w_dummy;
    v["n_freq_itemset"] = n_freq_itemset;  
    v["n_cache_wo_prune"] = n_cache_wo_prune;
    v["n_cache_w_prune"] = n_cache_w_prune;
    v["percent_saved"] = saved;

    /*save all in one place */
    std::stringstream ss;
    ss << i_roop;
    std::string str_num = ss.str();
    this->result["roop"].set(str_num, v);

    /*so that can return indipendently*/
    oss << v << std::endl;
    return oss.str();
}


std::string TestCode::Log::setTotalResult(double sum1, double sum2, double saved)
{
    std::ostringstream oss;
    json v;

//     saved << fixed << setprecision(1) << saved;
    v["N_cache_from"] = (int) sum1;
    v["N_cache_to"] = (int) sum2;
    v["percent_saved"] = saved; 


    /*save all in one place */
    this->result["total"] = v;

    /*so that can return indipendently*/
    oss << v << std::endl;
    return oss.str();
}

bool TestCode::Log::saveLogToJsonFile(std::string str) 
{
    //std::fstream::truncは上書きしてしまう
    std::fstream ofs(this->fnameLog.c_str(), std::fstream::out | std::fstream::app);
    if (ofs){
        ofs << str;
        ofs.close();
        return true;
    }
    else{
        return false;
    }
}


/* ========================  /Class Log ===================================*/

/* ======================== Class Apriori =============================*/

void TestCode::Apriori::loadData()
{
    std::string fnameData = this->getDataFile();
    //std::printf(" import data from file: %s\n", fname.c_str());
    
    TestCode::Util util;
	std::ifstream ifs(fnameData);
    
    // file name exceptions
	if (ifs.fail())
    {
		std::cerr << " Error: No such a file found: " << fnameData << std::endl;
		exit(1);
	}

	std::cerr << "\n Loading data...\n" << std::endl; 

	for (std::string line; getline(ifs, line);) 
    {
        std::vector<int> val_binary = util.split(line);
		plainData.push_back(val_binary);
	}

    int ntrans = plainData.size();
    int nitems = plainData[0].size();

    if (ntrans < 1 || nitems < 1)
    {
        std::cerr << "Error: invalid data formats!" << std::endl;
        std::cerr << "ntrans: " << ntrans << std::endl;
        std::cerr << "nitems: " << nitems << std::endl;
        exit(1);
    } 
    else
    {
        this->setNumTrans(ntrans);
        this->setNumItems(nitems);
        std::cout << "   INPUT DATASET: " 
            << " #trans: " << ntrans 
            << ",  #items: " << nitems 
            << std::endl;
        std::cout << "\n...Done. \n" << std::endl;
    }
}


// calc each dotproduct value & push each value
std::vector<int> TestCode::Apriori::countSupport(std::vector<int> cntVec)
{
    std::vector<std::vector<int>> cis = this->candItemSets;

    int res = 0;
    int n_cand = cis.size();
    int length = cis[0].size();
    std::vector<int> supAry; //supports array
    int cntN = 0;
    int ntrans = this->getNumTrans();
    TestCode::Util util;

    int x = n_cand/100;

    for (int i = 0; i < cis.size(); i++)
    {
        int sum = 0;
        for (int k = 0; k < ntrans; k++)
        {
            std::string hash = util.vectorStringJoin(cis[i], length-1);
            hash += ":" + std::to_string(k); //hash key
            int calcCacheInt = 1; 
            int start = 1;
            int end = length;

            if (this->calcCacheFlag && this->calcCache.count(hash) != 0)
            {
//                 std::printf("Using Cache\n");
                calcCacheInt = this->calcCache.at(hash);
//                 std::cout << "key: " << hash << ", val: " << calcCacheInt << std::endl;
                start = end - 1;
            }
            else
            {   // choose first 'a' from itemset [a,b,...]
                calcCacheInt = this->plainData[k][cis[i][0]]; 
                /*
                if (calcCacheInt != 0 && calcCacheInt != 1)
                {
                std::cout << "no cache" << calcCacheInt << std::endl;
                }
                */
            }
            int cacheCopy = calcCacheInt;

            for (int j = start; j < end; j++)
            {
                cacheCopy *= this->plainData[k][cis[i][j]];
                /*
                if (cacheCopy != 0 && cacheCopy != 1)
                {
                std::cout << "cache copy " << cacheCopy << std::endl;
                }
                */
            }


            if (this->calcCacheFlag && end >= 1)
            {
                //TODO:ここにキャッシュフィルタがほしい
                //検討事項：キャッシュする際に行うのか、すべてキャッシュしたあとに不要キャッシュを削除するのか
                if (cntVec[i] != 0)
                {
                    std::string hash = util.vectorStringJoin(cis[i], length);
                    hash += ":" + std::to_string(k);
                    this->calcCache.insert(make_pair(hash, cacheCopy));
                    cntN++; // #cached
                }
            }

            sum += cacheCopy; //sum up
        } // end k
        supAry.push_back(sum);
        // loging progress per 10%
        if ( x != 0 && i % (x*10) == 0 )
            std::printf("  Support-Count status: (%d / %d item) %3.3lf%%\n", i, n_cand, ((double)i/n_cand)*100.0);
    } // end i
    this->candSup = supAry;

    double caches = cis.size() * ntrans;
    std::printf("\n    #cached %.0f :-> %d (%.1f %%)\n", caches, cntN, (caches-cntN)*100/caches);

    std::vector<int> cache;
    cache.push_back(caches);
    cache.push_back(cntN);
    return cache;
}


std::vector<int> TestCode::Apriori::estimatePatterns(int pattern_length)
{
    //copy canditemsets
    std::vector<int> cntVec;
    std::vector<int> new_pattern;
    std::vector<std::vector<int>> subsets;
    int n_cand = this->getCandItemSetSize();
    int pre_patLen = pattern_length - 1;

	if(pre_patLen == 0)
    {
		for(int i = 0; i < n_cand; i++)
            cntVec.push_back(1);
        return cntVec;
	}
    else
    {
        std::vector<int> history;
        int cnt = 0;
		bool hasSamePattern = false;
		bool isAllFrequent = false;
        // for loging progress per 1 %
        int x = n_cand/100;

		for (int i = 0; i < n_cand-1; i++) { // i itemset
			for (int j = i + 1; j < n_cand; j++) { // i + 1 itemset
				for (int k = 0; k < pre_patLen; k++) { // k in i + 1 itemset

                    int ith_last_id = this->candItemSets[i][pre_patLen-1];
                    int jth_each_id = this->candItemSets[j][k];

                    if(jth_each_id > ith_last_id)
                    {
						if (!history.empty()) {
							for (int l = 0; l < history.size(); l++) {
								if(history[l] == jth_each_id)
                                {
									hasSamePattern = true;
                                    break;
								}
							}
						}
                        // count new possible candidate
						if(!hasSamePattern){
							history.push_back(jth_each_id);
                            // generate new pattern
                            new_pattern = this->candItemSets[i];
                            new_pattern.push_back(jth_each_id);

                            /* check all subsets of new pattern are frequent */
                            if (this->hasPruneStep)
                            {   // perform prune
                                //std::printf("pruning: (i,j,k)=(%d,%d,%d) for i in %d\n", i,j,k, n_cand);
                                subsets = this->getSubsets(new_pattern);
                                isAllFrequent = this->checkFreqOfSubsets(subsets, this->candItemSets);
                            }
                            else
                            {   // no prune
                                //std::printf("w/o prune\n");
                                isAllFrequent = true;
                            }

                            /* count # contributions of freqitemsets[i] */
                            if (isAllFrequent) cnt++;
						}
					} // end for generation of new candidate
                    // reset & prep for next new candidate pattern
					hasSamePattern = false;
                    isAllFrequent = false;
					new_pattern.clear();
				} // end for k in i + 1 itemset
			} // end for i + 1 itemsets
			history.clear();
            // cnt == 0 means notithing generated from freqItemSets[i]
            cntVec.push_back(cnt); 
            cnt = 0;

            // loging progress per 10%
            if ( x != 0 && i % (x*10) == 0 )
                std::printf("  Estimate-candidate status: (%d / %d item) %3.3lf%%\n", i, n_cand, ((double)i/n_cand)*100.0);
		} // end for i itemsets
//         cout << "#cntVec: " << cntVec.size() << ", #cis: " << cis.size() << endl;
        return cntVec;
	}
}



/* generate Candidate Item sets */
void TestCode::Apriori::candidateGenerator(int pattern_length){
    // confirmation for the correct itemsets' pattern length
    int n_freq = this->getFreqItemSetSize();
	std::cerr << "  Size of freqent item sets : " << n_freq 
        << "\n" << std::endl; 	

    int pre_patLen = pattern_length - 1; // pattern length in the previous loop
    
	if(pre_patLen == 0)
    {   // in first set, it is like [[0],[1],[2],...,[nitems]]
        std::vector<int> ids;
        int nitems = this->getNumItems();
		for(int i = 0; i < nitems; i++){
			ids.clear();
			ids.push_back(i);
			this->candItemSets.push_back(ids);
		}
	}
    else
    {   // patLen >= 2, i.e., pre_patLen >= 1
        std::vector<int> history;
        std::vector<int> new_pattern;
        std::vector<std::vector<int>> subsets;
		bool hasSamePattern = false;
        bool isAllFrequent = false; 
        // for loging progress per 1 %
        int x = n_freq/100;

		for(int i = 0; i < n_freq-1; i++){                   // ith pattern 
			for(int j = i + 1; j < n_freq; j++) {            // next (i+1)th-pattern
				for(int k = 0; k < pre_patLen; k++){         // k in next pattern
                    // check elements in the (i+1)th-pattern is over last element of ith-pattern
                    int ith_last_id = this->freqItemSets[i][pre_patLen-1];
                    int jth_each_id = this->freqItemSets[j][k];
                    //std::printf("check: %d > %d\n", jth_each_id, ith_last_id);
                    if(jth_each_id > ith_last_id)
                    {
                        // search history to figure out the pattern had already added to new candidate set 
                        if (!history.empty()) 
                        {
                            for(int l = 0; l < history.size(); l++) {
                                if(history[l] == jth_each_id){
                                    hasSamePattern = true;
                                    break;
                                }
                            }
                        }
                        // add new candidateset
                        if(!hasSamePattern)
                        {
                            history.push_back(jth_each_id);

                            /* generate new pattern */
                            new_pattern = this->freqItemSets[i];
                            new_pattern.push_back(jth_each_id);

                            /* ########## DEBUG ############ */
                            //std::printf("new_pattern \n");
                            //TestCode::Util util;
                            //util.printVec(new_pattern);
                            /* ########## DEBUG ############ */

                            /* check all subsets of new pattern are frequent */
                            if (this->hasPruneStep)
                            {   //perform prune 
                                //printf("pruning\n");
                                subsets = this->getSubsets(new_pattern);

                                /* ########## DEBUG ############ */
                                //std::printf("subsets\n");
                                //TestCode::Util util;
                                //util.printMat(subsets);
                                /* ########## DEBUG ############ */

                                isAllFrequent = this->checkFreqOfSubsets(subsets, this->freqItemSets);
                            }
                            else
                            {   // no prune step
                                //printf("w/o prune\n");
                                isAllFrequent = true;
                            }

                            if (isAllFrequent)
                                this->candItemSets.push_back(new_pattern);
                        }
                    }
                    // reset & prep for next new candidate pattern
					hasSamePattern = false;
                    isAllFrequent = false;
					new_pattern.clear();
				} // end for k in i + 1 itemset
			} // end for i + 1 itemsets
			history.clear();

            // loging progress per 10%
            if ( x != 0 && i % (x*10) == 0 )
                std::printf("  Generate-candidate status: (%d / %d item) %3.3lf%%\n", i, n_freq, ((double)i/n_freq)*100.0);
		} // end for i itemsets
	}
}

//	candItemSetsにダミーデータセットを追加する
void TestCode::Apriori::dummyGenerator(int pattern_length)
{
    
    this->clearDummyItemSets();
    int pre_patLen = pattern_length - 1; // pattern length in the previous loop

    if (pre_patLen == 0) return;
    if (this->infreqItemSets.size() > 0 && this->infreqItemSets[0].size() < pre_patLen)
    {
        std::cerr << "Error: invalid length itemsets, confirm the iteration counter." << std::endl;
        exit(1);
    }

    int n_ifis = this->getInfreqItemSetSize();
    std::vector<int> history;
    std::vector<int> new_pattern;
    std::vector<std::vector<int>> subsets;
	bool hasSamePattern = false;
	bool isAllFrequent = false;
    int x = n_ifis/100;

	for (int i = 0; i < n_ifis-1; i++) {
        for (int j = i + 1; j < n_ifis; j++) {
			for (int k = 0; k < pre_patLen; k++) {
                int ith_last_id = this->infreqItemSets[i][pre_patLen-1];
                int jth_each_id = this->infreqItemSets[j][k];
                //std::printf("check: %d > %d\n", jth_each_id, ith_last_id);
                if(jth_each_id > ith_last_id)
                {
                    // search history to figure out the pattern had already added to new candidate set 
                    if (!history.empty()) 
                    {
                        for(int l = 0; l < history.size(); l++) {
                            if(history[l] == jth_each_id){
                                hasSamePattern = true;
                                break;
                            }
                        }
                    }
                    // add new candidateset
                    if(!hasSamePattern)
                    {
                        history.push_back(jth_each_id);

                        /* generate new pattern */
                        new_pattern = this->infreqItemSets[i];
                        new_pattern.push_back(jth_each_id);

                        /* ########## DEBUG ############ */
                        //std::printf("new_pattern \n");
                        //TestCode::Util util;
                        //util.printVec(new_pattern);
                        /* ########## DEBUG ############ */

                        /* check all subsets of new pattern are frequent */
                        if (this->hasPruneStep)
                        {   //perform prune 
                            //printf("pruning\n");
                            subsets = this->getSubsets(new_pattern);

                            /* ########## DEBUG ############ */
                            //std::printf("subsets\n");
                            //TestCode::Util util;
                            //util.printMat(subsets);
                            /* ########## DEBUG ############ */

                            isAllFrequent = this->checkFreqOfSubsets(subsets, this->infreqItemSets);
                        }
                        else
                        {   // no prune step
                            //printf("w/o prune\n");
                            isAllFrequent = true;
                        }

                        if (isAllFrequent)
                            this->dummyItemSets.push_back(new_pattern);
                    }
                } // end for generation of new candidate
                // reset & prep for next new candidate pattern
                hasSamePattern = false;
                isAllFrequent = false;
                new_pattern.clear();
            } // end for k in i + 1 itemset
        } // end for i + 1 itemsets
        history.clear();
        // loging progress per 10%
        if ( x != 0 && i % (x*10) == 0 )
            std::printf("  Generate-dummy status: (%d / %d item) %3.3lf%%\n", i, n_ifis, ((double)i/n_ifis)*100.0);
    } // end for i itemsets

	/* -------------- add dummy itemsets to candItemSets ----------------------*/
    int dummySize = ceil((1.00 / this->getAlpha() - 1.00) * this->getCandItemSetSize());
    int n_cis = this->getCandItemSetSize();

	if (dummySize >= this->getDummyItemSetSize())
    {   // added all generated dumyItemSets to candItemSets
		for (std::vector<int> dis : this->dummyItemSets)
            this->candItemSets.push_back(dis);

        printf("\n   #CandidateItemset: %d :-> %d\n", n_cis, this->getCandItemSetSize());
	}
    else 
    {   // if dummyItemSets has lots of itemsets, choose randomly which to add to candItemSets
		int nrand = 0;
		int idx;
		std::vector<int> idxs;
		while (nrand < dummySize) {
			idx = genrand_int32() % this->getDummyItemSetSize();
			if (idxs.size() != 0) 
            {
				for (int i : idxs)
                    if (i == idx) break;
			}
			idxs.push_back(idx);
			nrand++;
		}

		// add random-selected dummyItemSets to candItemSets
		for (int i : idxs) 
			this->candItemSets.push_back(this->dummyItemSets[i]);

        printf("\n   #CandidateItemset: %d :-> %d\n", n_cis, this->getCandItemSetSize());
	}
    this->clearInfreqItemSets();
}




std::vector<std::vector<int>> TestCode::Apriori::getSubsets(std::vector<int> pattern)
{
    std::vector<int> set;
    std::vector<std::vector<int>> subset;
    for (int i = 0; i < pattern.size(); i++){
        for (int j = 0; j < pattern.size(); j++){
            if (j != i)
                set.push_back(pattern[j]);
        }
        subset.push_back(set);
        set.clear();
    }
    return subset;
}

bool TestCode::Apriori::isSameSet(
        std::vector<int> set1, std::vector<int> set2) 
{
    bool isEqual = true;

    // error check
    if (set1.size() != set2.size())
    {
        std::cerr 
            << "Error in size between sets to be compared.\n" 
            << "[ Size ]  set1: " << set1.size()
            << ",  set2: " << set2.size()
            << std::endl;
        exit(1);
    }

    for (int i = 0; i < set1.size(); i++){
        if (set1[i] != set2[i]) 
        {
            isEqual = false;
            break;
        }
    }
    return isEqual;
}

bool TestCode::Apriori::checkSetInSets(
        std::vector<int> keyset, 
        std::vector<std::vector<int>> sets)
{
    bool isEqual = false;
    for ( std::vector<int> set : sets ){
        isEqual = this->isSameSet(keyset, set);
        // if find the same set, end func.
        if (isEqual) return true;
    }
    // if all set are not equal to keyset
    return false;
}

bool TestCode::Apriori::checkFreqOfSubsets(
        std::vector<std::vector<int>> subsets,
        std::vector<std::vector<int>> searchFrom)
{
    bool isAllFrequent = true;
    bool isSetInSets = true;
    for ( std::vector<int> subset : subsets ){

        // ########## DEBUG ############
        //printf("subset in checkFreqOfSubsets\n");
        //TestCode::Util util;
        //util.printVec(subset);
        // ########## DEBUG ############

        isSetInSets = this->checkSetInSets(subset, searchFrom);
        if (!isSetInSets) 
        {
            isAllFrequent = false;
            break;
        }
    }
    return isAllFrequent;
}


void TestCode::Apriori::checkSupport(){
    int n_cis = this->getCandItemSetSize();
    int n_css = this->getCandSupSize();
    if(n_cis != n_css){
        std::printf(" Error: Server returned different num. of candidate sets");
        std::cerr 
            << " # of server's : "<< n_cis 
            << " # of client's: "<< n_css 
            << std::endl; 
        exit(1); 
    }

	for(int i = 0; i < n_cis; i++){
        if(this->candSup[i] >= this->getThreshold()) 
			this->freqItemSets.push_back(this->candItemSets[i]);
        else
            this->infreqItemSets.push_back(this->candItemSets[i]);
	}
	std::cerr << std::endl; 
}



void TestCode::Apriori::runApriori(std::string fnameData, std::string fnameLog, double minsup, double alpha, 
        bool calcCacheFlag, bool hasPruneStep, bool hasDummySet)
{
    this->setDataFile(fnameData);
    this->setLogFile(fnameLog);
    this->setCacheFlag(calcCacheFlag);
    this->setPruneFlag(hasPruneStep);
    this->setDummyFlag(hasDummySet);
    this->setMinSup(minsup);
    this->setAlpha(alpha);
    // start loading data
    this->loadData();
    double ms = this->getMinSup();
    this->setThreshold();
    double threshold = this->getThreshold();
    double alp = this->getAlpha();
    std::printf("minimum support: %2.0f %%, threshold: %2.0f, alpha: %2.0f %%\n", ms*100, threshold, alp*100);

    TestCode::Util util;

    /* store each variable to json format*/
    TestCode::Log log;
    log.initResult();

    std::string params = log.setParam(fnameData, fnameLog, 
            this->getNumTrans(), this->getNumItems(), 
            ms, threshold, alp, calcCacheFlag, hasPruneStep, hasDummySet);
    //bool ret;
    //ret = log.saveLogToJsonFile(params);
    //if (!ret)
        //std::cerr << "Apriori result log write error : " << this->fnameLog << std::endl;



    std::vector<int> n_cache;
    std::vector<std::vector<int>> n_eachCaches;

    int i_roop = 1;
    std::string jsonFmt;

    while(true){
        std::cerr << "\n======= Item length: [ " 
            << i_roop 
            << " ] ====== " << std::endl;

        std::cerr << "\n ---------- Generate Candidate ---------- \n" << std::endl;
        this->clearCandItemSets();
        this->candidateGenerator(i_roop);
        int n_cand_wo_dummy = this->getCandItemSetSize();
        std::cerr << "  Size of candidate itemsets: " 
            << n_cand_wo_dummy << "\n" << std::endl;
        if ( n_cand_wo_dummy == 0) break;
//         util.printMat(this->candItemSets);
        std::cerr << "\n ... Done. \n" << std::endl;
        
        if (this->hasDummySet)
        {
            std::cerr << "\n ---------- Generate Dummy ---------- \n" << std::endl;
            this->dummyGenerator(i_roop);
            std::cerr << "\n  Size of dummy itemsets: " 
                << this->getDummyItemSetSize() << "\n" << std::endl;
            std::cerr << "\n ... Done. \n" << std::endl;
        }

        this->clearCandSupports();
 
        std::cerr << "\n ---------- Estimate Patterns ---------- \n" << std::endl;
        std::vector<int> cntVec = this->estimatePatterns(i_roop);  //estimation of next calculation
//         std::cerr << "\n ### DEBUG MODE ###\n" << std::endl;
//         std::vector<int> cntVec(this->getCandItemSetSize(), 1); 
//         util.printVec(cntVec);
        std::cerr << "\n ... Done. \n" << std::endl;


        std::cerr << "\n ---------- Count Support ---------- \n" << std::endl;
        n_cache = this->countSupport(cntVec); //returns supports array
        n_eachCaches.push_back(n_cache);
//         std::cerr << "Each candidate's value:" << std::endl; 
//         util.printVec(this->candSup);
        std::cerr << "\n ... Done. \n" << std::endl;

        std::cerr << "\n ---------- Check Support ---------- \n" << std::endl;
        this->clearFreqItemSets();
        /* get new freqItemSets, each elem is over threshold*/
        this->checkSupport();
//         std::cerr << "Freqent item sets:" << std::endl;
//         util.printMat(this->freqItemSets);
        std::cerr << "\n ... Done. \n" << std::endl;

        /* stored for result */
        resultFreqItemSets.push_back(freqItemSets);

        /* log to json file*/
        jsonFmt = log.setEvalResult(
                i_roop,
                n_cand_wo_dummy,
                this->getDummyItemSetSize(), 
                this->getCandItemSetSize(), 
                this->getFreqItemSetSize(),
                n_cache[0],
                n_cache[1]
                );

//         ret = log.saveLogToJsonFile(jsonFmt);
//         if (!ret)
//             std::cerr << "Apriori result log write error : " << this->fnameLog << std::endl;

        /* condition for finishing apriori */
        if(i_roop >= this->getNumItems() || this->getFreqItemSetSize() < 2) break;
        i_roop++; 
    }

    std::cout << "\n ...Done\n";
    
    /* calc average for num. of caches*/
    double sum1 = 0.0;
    double sum2 = 0.0;
    for (int i = 0; i < n_eachCaches.size(); i++){
        sum1 += n_eachCaches[i][0]; // in naive way
        sum2 += n_eachCaches[i][1]; // by next-candidate estimation
    }
    double saved = (sum1-sum2)*100/sum1;
    printf("\n Num. of caches\n");
    printf(" In naive way: [ %.0f ] ---> In estimation way: [ %.0f ]\n", sum1, sum2);
    printf("                     %.1f %% saved!\n", saved);

    /* log to json file*/
    jsonFmt = log.setTotalResult(sum1, sum2, saved);

    sum1 = 0.0;
    sum2 = 0.0;
    std::vector<double> size_ct{0.70, 1.00, 0.95, 0.85, 0.70}; // MB
    for (int i = 0; i < n_eachCaches.size(); i++){
        sum1 += n_eachCaches[i][0]*size_ct[i]; // in naive way
        sum2 += n_eachCaches[i][1]*size_ct[i]; // by next-candidate estimation
    }
    printf("\n Ciphertext size on memory\n");
    printf(" In naive way: [ %.0f MB ] ---> In estimation way: [ %.0f MB ]\n", sum1, sum2);
    printf("                     %.1f %% saved!\n", (sum1-sum2)*100/sum1);

    //std::cerr << "result: frequent item sets" << std::endl;
    //util.print3d(this->resultFreqItemSets);

    /* write down all results to file in json fmt */
    std::ostringstream oss;
    oss << log.result << std::endl;
    log.saveLogToJsonFile(oss.str());
}

//======================== / Class Apriori ===================================


/* ====================== exec ===========================================*/
int main(int argc, char **argv)
{
    // instance generation
    TestCode::Apriori apriori;
//     std::string data = "./data/input/BEST_T40I60N500D1kL1k";
    std::string data = "./data/input/TEST";
    std::string fnameLog= "./apriori.log.json";
    double minsup = 0.1;
    double alpha = 0.5;
    bool calcCacheFlag = true;
    bool hasPruneStep = true;
    bool hasDummySet = true;
    apriori.runApriori(data, fnameLog, minsup, alpha, calcCacheFlag, hasPruneStep, hasDummySet);

    return 0;
}
