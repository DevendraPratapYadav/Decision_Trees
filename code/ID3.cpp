#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <algorithm>
#include <map>
#include <stack>
#include <queue> 
#include <deque>
#include <list>
#include <utility>
#include <fstream>
#include <time.h> 
#include <cstdlib>
#include <string.h>
#define lli long long int  

using namespace std;

lli n, d, splindex ; 

lli data[6000][100], test[6000][100], all[6000][100] ; 

double early, info_gain_threshold, global_error ; 

lli max_depth ;

map<lli, lli> atbt_list ;

vector<lli> test_split ;

char *filename, *exptno ;

lli attrCount[100];

typedef struct objects
{
	int atbt, plus, minus, current, label ; 
	
	struct objects *parent ;
	
	vector<int> set ;  
	
	map<lli, struct objects*> child ; 
	
	objects()  
	{
		child.clear() ;  
		plus = 0 ;
		minus = 0 ;  
		atbt = -1 ;  
		label = -1 ;  
	}
} node ; 


node *head ;


int read_data()
{

	freopen (filename,"r",stdin) ; 

	cin >> n >> d ;
	
	lli i, j ;
	
	for(i=0;i<n;i++)
	{
		for(j=0;j<=d;j++)
		{
			cin >> data[i][j] ; 
		}
	}
	
	
	fclose(stdin) ;
}


lli best_atbt(vector<int> a, double root_entropy)
{ 
	double maximum = 0, info_gain, loss, temp ;

	lli final_atbt, atbt, total = a.size() ;
	
	lli i, j ; 
	
	map<pair<lli, lli> , lli> cfy ;  // classify
	
	final_atbt = -1 ; 
 	
	for(atbt=0;atbt<d;atbt++) 
	{
		if(atbt_list[atbt] > 0)
			continue ;  
	
		cfy.clear() ;
		
		loss = 0 ;
	
		for(i=0;i<total;i++)
		{
			cfy[make_pair(data[a[i]][atbt],data[a[i]][d])]++ ; 
		}
		
		map<pair<lli, lli>, lli>::iterator it, next ;
		
		for(it=cfy.begin(); it!=cfy.end();)
    	{
    		
    		double entropy, pplus, pminus, pall ;
    		
    		next = ++it ;
    		it-- ;
    		
    		if(next == cfy.end())
    		{
    		//	cout << it->first.first << " " << it->first.second << endl ;
  
    			entropy = 0 ; 
    			pall = it->second ; 
    			it++ ;
    		}
    		else
    		{
    			if(it->first.first == next->first.first)
    			{
    			//	cout << it->first.first << " " << it->first.second << endl ;
    			//	cout << next->first.first << " " << next->first.second << endl ;
    			
					entropy = 0 ;
					
					pminus = it->second ;
					pplus = (next)->second ; 
					
					pall = pplus + pminus ; 
					
					pplus = pplus / pall ;
					pminus = pminus / pall ; 
					
					entropy = -(pplus * log2(pplus) + pminus * log2(pminus)) ;
					
					it++ ;
					it++ ; 
    			}
    			else
    			{
    				//cout << it->first.first << " " << it->first.second << endl ;
    			
    				entropy = 0 ;
    				pall = it->second ;  
    				it++ ;
    			}
    		}
    	
    		//cout << endl ;
    		
    		temp = pall / (double)total ;
    		
    		//cout << pall << " " << total << endl ;
    		//cout << temp << endl << endl  ;
    		
    		temp = temp * entropy ;
    		loss = loss + temp ;
    	}
    	
    	info_gain = root_entropy - loss ;
    	
    	//cout << atbt << " " << loss << endl << endl ;           
    	
    	if(info_gain >= maximum)
    	{
    		maximum = info_gain ;
    		final_atbt = atbt ;  
    	} 
	}
	
	if(maximum < info_gain_threshold)
		final_atbt = -100 ; 
		
	if(final_atbt == -1)
		final_atbt = -200 ;
		
	//cout << "INFO GAIN : " << maximum << endl ;
	
	return final_atbt ; 
	
}


int split(node *curr, int depth)
{

	double root_entropy, pplus, pminus ;

	if(max_depth < depth)
		max_depth = depth ;


	lli i, total = curr->set.size() ;
	
	for(i=0;i<total;i++)
	{
		if(data[curr->set[i]][d] == 1)
			curr->plus++ ;
		else
			curr->minus++ ; 
	}
	
	// Calculating entropy
	
	pplus = (double) curr->plus / (double) total ;
	pminus = (double) curr->minus / (double) total ;
	
	root_entropy = - (pplus * log2(pplus)) - (pminus * log2(pminus)) ; 
	
	// Done
	
	
	// Early Stopping Condition 
	
	double purity ;
	
	purity = curr->plus ;
	purity = purity / (double) total ;  
	
	if(purity >= early) 
	{
		curr->label = 1 ;
		
		//cout << "Added plus : All pure values\n\n" ;
		
		return 0 ;
	}
	
	purity = curr->minus ;
	purity = purity / (double) total ;  
	
	if(purity >= early) 
	{
		curr->label = 0 ;
		
		//cout << "Added minus : All pure values\n\n" ;
		
		return 0 ;
	}
	
	// End of early stopping condition  
	
	
	
	lli atbt = best_atbt(curr->set, root_entropy) ; //  Returns best attribute to pick
	
	
	
	if(atbt == -100)					// 
	{
		curr->label = data[curr->set[0]][d] ;
		
		//cout << "Added label : Zero Info Gain\n\n" ;
		
		return 0 ;  
	}
	
	if(atbt == -200)
	{
		if(curr->plus > curr->minus)
			curr->label = 1 ;
		else
			curr->label = 0 ;
			
		//cout << "Added label : All attributes over\n\n" ;
			
		return 0 ;
	}
	
	
	curr->atbt = atbt ;
	
	
	atbt_list[atbt]++ ;    // You used up that attribute !!!
	
	
	for(i=0;i<total;i++)
	{
		lli element = curr->set[i] ; 
		lli key = data[element][atbt] ;  
	
		if(curr->child[key] == NULL)
		{
			curr->child[key] = new node() ; 
			
			curr->child[key]->current = key ; 
			
			curr->child[key]->parent = curr ;            
			
			curr->child[key]->set.push_back(element) ; 
		}
		else
		{
			curr->child[key]->set.push_back(element) ; 
		}
	}
	
	// Details 
	
	/*
	
	cout << "Depth : " << depth << endl ;
	cout << "Attribute : " << atbt << endl ;
	cout << "No. of children : " << curr->child.size() << endl ;
	cout << "Plus : " << curr->plus << endl ;
	cout << "Minus : " << curr->minus << endl ;
	cout << "Set elements : " << curr->set.size() << endl ;
	
	for(i=0;i<total;i++)
		cout << curr->set[i] << " " ;
		
	cout << endl << endl ;
	*/
	
	// Please add something for the poor empty nodes 
	
	// Please add something so that same attributes never show up again 
	
	
	map<lli, node*>::iterator it ; 
		
	for(it=curr->child.begin(); it!=curr->child.end(); it++)
    {
   		split(it->second, depth+1) ;  	
    }
	
	
	atbt_list[atbt]-- ;    // You freed that attribute !!!
	
	return 0 ; 

}


int evaluate(int index, node *curr)
{
	
	if(curr->label != -1)
	{
		return curr->label ; 
	}
	
	lli atbt = curr->atbt ; 
	
	if(curr->child[data[index][atbt]] != NULL)
	{
		return evaluate(index, curr->child[data[index][atbt]]) ;
	}
	else
	{
		if(curr->plus > curr->minus)
			return 1 ;
		else
			return 0 ; 
	}
	
}










vector <node*> getChild(map<lli,node*> m)
{

   
	vector <node*> ch;
	
	map<lli,node*>::iterator it;
 
	for (it=m.begin(); it!=m.end();it++)
	{
	    if ( (it->second) !=NULL)
		ch.push_back(it->second);
	}
 
	return ch;
 
 
} // end getChild

 
lli countNodes(node* root)
{
	lli c = 0, i;
 	
 	deque< node* > q;
     
    if (root==NULL)
   		return 0 ;
   		
  	q.push_front(root);
 
	while(!q.empty())
	{
		  node* rt=q.front();
		  
		  attrCount[ rt->atbt]++;
		  
		  q.pop_front();
		  
		  c++;
		  
		  vector < node* > nb = getChild(rt->child);
		  
		  for (i=0;i<nb.size();i++)
		  { 
			 if ((nb[i]) != NULL)
			 	q.push_front(nb[i]) ; 
	 
		  }
	}// end while
 
 
return c;
} // end countNOdes


void addNoise(double p)
{
	p/=100;
	srand(time(NULL)) ;
	//cout<<n*p;
	for (int i=0;i<n*p;i++)
	{
	   
		int a=rand()%n;// ,b=rand()%n;
		int temp=data[a][d];
		data[a][d]=1-data[a][d];//data[b][d];
		//data[b][d]=1-data[b][d];//;temp; 
		//cout<<"swap "<<a<<", "<<b<<"\n"<<data[a][d]<<", "<<data[b][d]<<endl;
	}
 

}




int initialise()
{
	early = 1 ;
	max_depth = 0 ;
	info_gain_threshold = 0.0000001 ;
	global_error = 1000 ;  

	test_split.clear() ;
	atbt_list.clear() ;
	read_data() ; 
}


int split_data(lli value)
{
	splindex = value ; 

	srand(time(NULL)) ;
	
	lli i, j ;
	
	for(i=0;i<n;i++)
	{
		test_split.push_back(i) ;
	}
	
	random_shuffle(test_split.begin(), test_split.end()) ;
}


int init_head(node* head)
{
	lli i ; 
	
	max_depth = 0 ;

	for(i=0;i<splindex;i++)
	{
		head->set.push_back(test_split[i]) ; 
	}
}


double run()
{
	lli result = 0, i ;

	double error = 0 ;

	for(i=splindex;i<n;i++)
	{
		result = evaluate(test_split[i], head) ;
		
		if(result != data[test_split[i]][d])
			error++ ;
	}
	
	error = error / (double) (n - splindex) ;
		
	//cout << "Depth : " << max_depth << endl << "Error : " << error << endl  << "Nodes : " << countNodes(head) << endl ; 
	
	return error ; 
}


double pruntest()
{
	lli result = 0, i ;

	double error = 0 ;

	for(i=splindex;i<splindex+1000;i++)
	{
		result = evaluate(test_split[i], head) ;
		
		if(result != data[test_split[i]][d])
			error++ ;
	}
	
	error = error / (double) (1000) ;
		
	//cout << "Depth : " << max_depth << endl << "Error : " << error << endl  << "Nodes : " << countNodes(head) << endl ; 
	
	return error ; 
}


int pruning(node *curr)
{	
	if(curr == NULL)
		return 0 ;

	lli i, total = curr->child.size() ;
	
	node *temp ; 
	
	double local_error ;
	
	if(total == 0)
		return 0 ; 
	
	map<lli,node*>::iterator it;
 
	for (it=curr->child.begin(); it!=curr->child.end();it++)
	{
	    pruning(it->second) ;
	}
	
	for (it=curr->child.begin(); it!=curr->child.end();it++)
	{
	    temp = it->second ; 
	    
	    it->second = NULL ; 
	    
	    local_error = pruntest() ;
	    
	    if(local_error < global_error)
	    {
	    	global_error = local_error ; 
	    	cout << setw(8) << (100 - run()*100) << setw(16) << countNodes(head) << endl ;
	    }
	    else
	    {
	    	it->second = temp ;	
	  	} 
	}
	
}


int ensemble(lli num)
{
	node *forest[num] ; 
	
	lli i, j ;
	
	vector <lli> atbt_shuffle ;
	
	for(i=0;i<d;i++)
	{
		atbt_shuffle.push_back(i) ;
	}
		
	lli k ;
	
	lli plus_count, minus_count ;
	
	for(k=0;k<num;k++)
	{
		atbt_list.clear() ; 
	
		forest[k] = new node() ;	
		forest[k]->parent = NULL ;
		
		random_shuffle(atbt_shuffle.begin(), atbt_shuffle.end()) ;
		
		for(i=9;i<d;i++)
		{
			atbt_list[atbt_shuffle[i]] = 100 ; 
		}
		
		init_head(forest[k]) ; 
		
		split(forest[k], 0) ;
		
		//cout << "Depth : " << max_depth << " Nodes : " << countNodes(forest[k]) << endl ; 
	}
	
	lli result = -1 ;

	double test_error = 0, train_error = 0 ;

	for(i=splindex;i<n;i++)
	{
		plus_count = minus_count = 0 ;
		
		for(k=0;k<num;k++)
		{
			result = evaluate(test_split[i], forest[k]) ;
		
			if(result == 1)
				plus_count++ ;
			else
				minus_count++ ;
		}
		
		if(plus_count > minus_count)
			result = 1 ;
		else
			result = 0 ;
		
		if(result != data[test_split[i]][d])
			test_error++ ;
	}
	
	for(i=0;i<splindex;i++)
	{
		plus_count = minus_count = 0 ;
		
		for(k=0;k<num;k++)
		{
			result = evaluate(test_split[i], forest[k]) ;
		
			if(result == 1)
				plus_count++ ;
			else
				minus_count++ ;
		}
		
		if(plus_count > minus_count)
			result = 1 ;
		else
			result = 0 ;
		
		if(result != data[test_split[i]][d])
			train_error++ ;
	}
	
	test_error = test_error / (double) (n - splindex) ;
	test_error = 100 * (1 - test_error) ;
	
	train_error = train_error / (double) splindex ;
	train_error = 100 * (1 - train_error) ;
		
	cout << setw(20) << train_error << setw(20) << test_error << endl ;  
	
}


int main(int argc, char *argv[])
{
	
	filename = argv[1] ;
	
	exptno = argv[2] ;
	
	initialise() ;
	
	lli i, j ;
	
	if(exptno[0] == '1') 
	{
		cout << setw(30) << "Experiment no. - 1\n\n" ;
	
		double iter ;
		
		int runs = 10, nnodes=0 ; 
		
		cout << "Threshold %" << setw(16) << "Test accuracy" << setw(16) << "No. of nodes" << endl << endl ;

		for(iter=0.94;iter<=1;iter=iter+0.01)
		{
		
			for (int i=0;i<=d;i++)
	   	     {
		 	   	attrCount[i]=0;
	  	     }
   	 

			double res[3] = {0.0} ;

			for (int rr=0;rr<runs;rr++)
			{
			
				early = iter ;
				
				head = new node() ;	
				head->parent = NULL ; 
	
				split_data(1000) ;
	
				init_head(head) ; 
	
				split(head, 0) ;
				
				nnodes = countNodes(head) ;
				
				if(nnodes==1)
				{
					rr--;
					continue;
				}
			
				res[0]+=run() ;  
				res[1]+=nnodes ;
				
			}
		
			res[0]/=runs;
			res[1]/=runs;
	
			cout << setw(4) << (iter*100) << setw(20) << (100-res[0]*100) << setw(16) << (int)res[1] << endl ; 
			
			
		}
		
		cout << "\nNumber of times an attribute is used as the splitting function : " << endl << endl ;
		
		vector < pair<int,int> > afreq;
	
		for (i=0;i<=d;i++)
		{
		 	attrCount[i]/=runs;
		   	afreq.push_back(make_pair(attrCount[i],i));
		}
	   	 
	   	sort(afreq.begin(),afreq.end());
	   	 
	   	for (i=afreq.size()-1;i>=0;i--)
	   	{
	   	 	if (afreq[i].first>0)
	 	   	cout << setw(4) <<  afreq[i].second<<" -> "<<afreq[i].first<<endl;
		}
	
	}
	else if(exptno[0] == '2')
	{
		cout << setw(30) << "Experiment no. - 2\n\n" ;
		
		int iter = 4 ;
		
		double noise[] = {0.5, 1, 5, 10} ;

		int runs = 10, nnodes=0 ; 
		
		cout << "Noise %" << setw(16) << "Test accuracy" << setw(16) << "No. of nodes" << endl << endl ;

		for(iter=0;iter<4;iter++)
		{
			
			read_data() ;
			addNoise(noise[iter]) ;
		

			double res[3] = {0.0} ;

			for (int rr=0;rr<runs;rr++)
			{
			
				head = new node() ;	
				head->parent = NULL ; 
	
				split_data(1000) ;
	
				init_head(head) ; 
	
				split(head, 0) ;
				
				nnodes = countNodes(head) ;
				
				if(nnodes==1)
				{
					rr--;
					continue;
				}
			
				res[0]+=run() ;  
				res[1]+=nnodes ;
				
			}
		
			res[0]/=runs;
			res[1]/=runs;
	
			cout << setw(4) << noise[iter] << setw(16) << (100-res[0]*100) << setw(16) << (int)res[1] << endl ; 
			
		}
		
	}
	else if(exptno[0] == '3') 
	{
		cout << setw(24) << "Experiment no. - 3\n\n" ;
	
		cout << "Test accuracy" << setw(16) << "No. of nodes" << endl << endl ;
	
		head = new node() ;	
		head->parent = NULL ; 
	
		split_data(1000) ;
	
		init_head(head) ; 
	
		split(head, 0) ;
	
		global_error = pruntest() ;
	
		pruning(head) ;
	
		cout << endl ; 
	}
	else if(exptno[0] == '4')
	{
		cout << setw(34) << "Experiment no. - 4\n\n" ;
		
		cout <<"No. of trees" << setw(16) << "   Training accuracy" << setw(16) << "Test accuracy" << endl << endl ;
	
		for(i=1;i<=25;i++)
		{
			split_data(1000) ;
			cout << setw(4) << i ;
			ensemble(i) ;
		}
	}
	
}









