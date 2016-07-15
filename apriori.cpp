#include <iostream> 
#include <iomanip> 
#include <sstream> 
#include <fstream>
#include <string> 
#include <vector> 
#include <unordered_map>
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


//======================== Class Util ===================================

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


//======================== / Class Util ===================================



/* ======================== Class Apriori =============================*/

void TestCode::Apriori::loadData()
{
    std::string fname = this->getFile();
    //std::printf(" import data from file: %s\n", fname.c_str());
    
    TestCode::Util util;
	std::ifstream ifs(fname);
    
    // file name exceptions
	if (ifs.fail())
    {
		std::cerr << " Error: No such a file found: " << fname << std::endl;
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
    int length = cis[0].size();
    std::vector<int> supAry; //supports array
    int cntN = 0;
    int ntrans = this->getNumTrans();
    TestCode::Util util;


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
        }
        supAry.push_back(sum);
    }
    this->candSup = supAry;

    double caches = cis.size() * ntrans;
    std::printf("    #cached %.0f :-> %d (%.1f %%)\n", caches, cntN, (caches-cntN)*100/caches);

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

                            // check all subsets of new pattern are frequent
                            subsets = this->getSubsets(new_pattern);
                            isAllFrequent = this->checkFreqOfSubsets(subsets, this->candItemSets);
                            //count # contributions of freqitemsets[i] 
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
            std::printf("  Estimate-candidate status: (%d / %d item) %3.3lf%%\n", i, n_cand, ((double)i/n_cand)*100.0);
		} // end for i itemsets
//         cout << "#cntVec: " << cntVec.size() << ", #cis: " << cis.size() << endl;
        return cntVec;
	}
}



/* generate Candidate Item sets */
void TestCode::Apriori::candidateGenerator(int pattern_length){
    // confirmation for the correct itemsets' pattern length
    this->clearCandItemSets();
    int n_freq = this->getFreqItemSetSize();
	std::cerr << "size of freqent item sets : " << n_freq 
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

                            // generate new pattern
                            new_pattern = this->freqItemSets[i];
                            new_pattern.push_back(jth_each_id);

                            // ########## DEBUG ############
                            //std::printf("new_pattern \n");
                            //TestCode::Util util;
                            //util.printVec(new_pattern);
                            // ########## DEBUG ############

                            // check all subsets of new pattern are frequent
                            subsets = this->getSubsets(new_pattern);

                            // ########## DEBUG ############
                            //std::printf("subsets\n");
                            //TestCode::Util util;
                            //util.printMat(subsets);
                            // ########## DEBUG ############

                            isAllFrequent = this->checkFreqOfSubsets(subsets, this->freqItemSets);
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
            std::printf("  Generate-candidate status: (%d / %d item) %3.3lf%%\n", i, n_freq, ((double)i/n_freq)*100.0);
		} // end for i itemsets
	}
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
        if(this->candSup[i] >= this->getThreshold()) {
			this->freqItemSets.push_back(this->candItemSets[i]);
		}
	}
	std::cerr << std::endl; 
}



void TestCode::Apriori::runApriori(std::string filename, double minsup)
{
    this->setFile(filename);
    this->loadData();
    this->setMinSup(minsup);
    double ms = this->getMinSup();
    this->setThreshold();
    double threshold = this->getThreshold();
    std::printf("minimum support: %2.0f %%, threshold: %2.0f \n", ms*100, threshold);

    TestCode::Util util;

    std::vector<int> n_cache;
    std::vector<std::vector<int>> n_eachCaches;

    int i_roop = 1;

    while(true){
        std::cerr << " ---------- Generate Candidate ---------- " << std::endl;
        this->candidateGenerator(i_roop);
        std::cerr << "======= Item length: [ " 
            << i_roop 
            << " ] ====== " << std::endl;
        std::cerr << "Size of candidate itemsets: " 
            << this->getCandItemSetSize() << "\n" << std::endl;
        if ( this->getCandItemSetSize() == 0) break;
//         util.printMat(this->candItemSets);
        std::cerr << " ... Done. \n" << std::endl;
        
        //calc total sum for each items
        this->clearCandSupports();
        std::cerr << " ---------- Estimate Patterns ---------- " << std::endl;
        std::vector<int> cntVec = this->estimatePatterns(i_roop);  //estimation of next calculation
        //util.printVec(cntVec);
        std::cerr << " ... Done. \n" << std::endl;

        std::cerr << " ---------- Count Support ---------- " << std::endl;
        n_cache = this->countSupport(cntVec); //returns supports array
        n_eachCaches.push_back(n_cache);
//         std::cerr << "Each candidate's value:" << std::endl; 
//         util.printVec(this->candSup);
        std::cerr << " ... Done. \n" << std::endl;
        this->clearFreqItemSets();

        std::cerr << " ---------- Check Support ---------- " << std::endl;
        /* get new freqItemSets, each elem is over threshold*/
        this->checkSupport();
//         std::cerr << "Freqent item sets:" << std::endl;
//         util.printMat(this->freqItemSets);
        std::cerr << " ... Done. \n" << std::endl;

        /* stored for result */
        resultFreqItemSets.push_back(freqItemSets);
        i_roop++; 

        /* condition for finishing apriori */
        if(i_roop > this->getNumItems() || this->getFreqItemSetSize() < 2) break;
    }

    std::cout << "\n ...Done\n";

    /* calc average for num. of caches*/
    double sum1 = 0.0;
    double sum2 = 0.0;
    for (int i = 0; i < n_eachCaches.size(); i++){
        sum1 += n_eachCaches[i][0]; // in naive way
        sum2 += n_eachCaches[i][1]; // by next-candidate estimation
    }
    printf("\n Num. of caches\n");
    printf(" In naive way: [ %.0f ] ---> In estimation way: [ %.0f ]\n", sum1, sum2);
    printf("                     %.1f %% saved!\n", (sum1-sum2)*100/sum1);

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
}

//======================== / Class Apriori ===================================


/* ====================== exec ===========================================*/
int main(int argc, char **argv)
{
    // instance generation
    TestCode::Apriori apriori;
//     std::string data = "./data/input/T10I6N50D100L1k";
    std::string data = "./data/input/T100I10N1kD100L1k_tmp";
    double minsup = 0.1;
    apriori.runApriori(data, minsup);

    return 0;
}
