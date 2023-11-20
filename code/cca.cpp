#include "basis.h"
#include "cca.h"
#include "cw.h"
#include "preprocessor.h"

#include <sys/times.h> //these two h files are for linux
#include <unistd.h>

//pick a var to be flip
int pick_var()
{
	int         i,k,c,v;
	int         best_var;
	lit*		clause_c;
	
	/**Greedy Mode**/
	/*CCD (configuration changed decreasing) mode, the level with configuation chekcing*/
	if(goodvar_stack_fill_pointer>0)
	{
		best_var = goodvar_stack[0];
		
		for(i=1; i<goodvar_stack_fill_pointer; ++i)
		{
			v=goodvar_stack[i];
			if(score[v]>score[best_var]) best_var = v;
			else if(score[v]==score[best_var] && time_stamp[v]<time_stamp[best_var]) best_var = v;
		}
		
		return best_var;
	}
	
	/*SD (significant decreasing) mode, the level with aspiration*/
	best_var = 0;
	for(i=0; i<unsatvar_stack_fill_pointer; ++i)
	{
		if(score[unsatvar_stack[i]]>sigscore) 
		{
			best_var = unsatvar_stack[i];
			break;
		}
	}

	for(++i; i<unsatvar_stack_fill_pointer; ++i)
	{
		v=unsatvar_stack[i];
		if(score[v]>score[best_var]) best_var = v;
		else if(score[v]==score[best_var] && time_stamp[v]<time_stamp[best_var]) best_var = v;
	}
		
	if(best_var!=0) return best_var;
		
	/**Diversification Mode**/

	update_clause_weights();
	
	/*focused random walk*/

	c = unsat_stack[rand()%unsat_stack_fill_pointer];
	clause_c = clause_lit[c];
	best_var = clause_c[0].var_num;
	for(k=1; k<clause_lit_count[c]; ++k)
	{
		v=clause_c[k].var_num;
		//if(time_stamp[v]<time_stamp[best_var]) best_var = v;
		if(score[v]>score[best_var]) best_var = v;
		else if(score[v]==score[best_var]&&time_stamp[v]<time_stamp[best_var]) best_var = v;
	}
	
	return best_var;
}


//set functions in the algorithm
void settings()
{
	set_clause_weighting();
}

void local_search(int max_flips)
{
	int flipvar;
     
	for (step = 0; step<max_flips; step++)
	{
		//find a solution
		if(unsat_stack_fill_pointer==0) return;

		flipvar = pick_var();

		flip(flipvar);

		time_stamp[flipvar] = step;
	}
}


int main(int argc, char* argv[])
{
	int     seed,i;
	int		satisfy_flag=0;
	struct 	tms start, stop;
    
    cout<<"c This is CCAnr [Version: 2013.4.18] [Author: Shaowei Cai]."<<endl;	
	
	times(&start);

	if (build_instance(argv[1])==0)
	{
		cout<<"Invalid filename: "<< argv[1]<<endl;
		return -1;
	}

    sscanf(argv[2],"%d",&seed);
    
    srand(seed);
    
    if(unitclause_queue_end_pointer>0) preprocess();
    
    build_neighbor_relation();
    
	cout<<"c Instance: Number of variables = "<<num_vars<<endl;
	cout<<"c Instance: Number of clauses = "<<num_clauses<<endl;
	cout<<"c Instance: Ratio = "<<ratio<<endl;
	cout<<"c Instance: Formula length = "<<formula_len<<endl;
	cout<<"c Instance: Avg (Min,Max) clause length = "<<avg_clause_len<<" ("<<min_clause_len<<","<<max_clause_len<<")"<<endl;
	cout<<"c Algorithmic: Random seed = "<<seed<<endl;
    
	for (tries = 0; tries <= max_tries; tries++) 
	{
		 settings();
		 
		 init();
	 
		 local_search(max_flips);

		 if (unsat_stack_fill_pointer==0) 
		 {
		 	if(verify_sol()==1) {satisfy_flag = 1; break;}
		    else cout<<"c Sorry, something is wrong."<<endl;/////
		 }
	}

	times(&stop);
	double comp_time = double(stop.tms_utime - start.tms_utime +stop.tms_stime - start.tms_stime) / sysconf(_SC_CLK_TCK);

    if(satisfy_flag==1)
    {
    	cout<<"s SATISFIABLE"<<endl;
		print_solution();
    }
    else  cout<<"s UNKNOWN"<<endl;
    
    cout<<"c solveSteps = "<<tries<<" tries + "<<step<<" steps (each try has "<<max_flips<<" steps)."<<endl;
    cout<<"c solveTime = "<<comp_time<<endl;
	 
    free_memory();

    return 0;
}
