#include "basis.h"
//preprocess
void unit_propagation()
{
    lit uc_lit;
    int uc_clause;
    int uc_var;
    bool uc_sense;
    
    int c,v;
    int i,j;
    lit cur, cur_c;
    
    
    //while (unitclause_queue_beg_pointer < unitclause_queue_end_pointer) 
    for(unitclause_queue_beg_pointer=0; unitclause_queue_beg_pointer < unitclause_queue_end_pointer; unitclause_queue_beg_pointer++)
    {
        uc_lit = unitclause_queue[unitclause_queue_beg_pointer];
        
        uc_var = uc_lit.var_num;
        uc_sense = uc_lit.sense;
        
        if(fix[uc_var]==1) {if(uc_sense!=cur_soln[uc_var])cout<<"c wants to fix a variable twice, forbid."<<endl; continue;}
     
        cur_soln[uc_var] = uc_sense;//fix the variable in unit clause
        fix[uc_var] = 1;
        
        for(i = 0; i<var_lit_count[uc_var]; ++i)
        {
            cur = var_lit[uc_var][i];
            c = cur.clause_num;
            
            if(clause_delete[c]==1) continue;
            
            if(cur.sense == uc_sense)//then remove the clause from var's var_lit[] array
            {
                clause_delete[c]=1;
            }
            else
            {
                if(clause_lit_count[c]==2)
                {
                    if(clause_lit[c][0].var_num == uc_var)
                    {
                        unitclause_queue[unitclause_queue_end_pointer++] = clause_lit[c][1];
                    }
                    else
                    {
                        unitclause_queue[unitclause_queue_end_pointer++] = clause_lit[c][0];
                    }
                    
                    clause_delete[c]=1;
                }
                else
                {
                    for(j=0; j<clause_lit_count[c]; ++j)
                    {
                        if(clause_lit[c][j].var_num == uc_var)
                        {
                            clause_lit[c][j]=clause_lit[c][clause_lit_count[c]-1];
    
                            clause_lit_count[c]--;
                            
                            break;
                        }
                    }//for
                }
            }
            
        }//for 
        
        
        
    }//begpointer to endpointer for
    
}


void preprocess()
{
    int c,v,i;
    int delete_clause_count=0;
    int fix_var_count=0;
    
    unit_propagation();
    
    //rescan all clauses to build up var literal arrays
    for (v=1; v<=num_vars; ++v) 
        var_lit_count[v] = 0;
    
    max_clause_len = 0;
	min_clause_len = num_vars;
    int    formula_len=0;
    
    for (c = 0; c < num_clauses; ++c) 
    {
        if(clause_delete[c]==1) {
            delete_clause_count++;
            continue;
        }
        
        for(i=0; i<clause_lit_count[c]; ++i)
        {
            v = clause_lit[c][i].var_num;
            var_lit[v][var_lit_count[v]] = clause_lit[c][i];
            ++var_lit_count[v];
        }
        clause_lit[c][i].var_num=0; //new clause boundary
        clause_lit[c][i].clause_num = -1;
        
        //about clause length
        formula_len += clause_lit_count[c];
        
        if(clause_lit_count[c] > max_clause_len)
            max_clause_len = clause_lit_count[c];
        else if(clause_lit_count[c] < min_clause_len)
            min_clause_len = clause_lit_count[c];
    }
    
    avg_clause_len = (double)formula_len/num_clauses;
    
    for (v=1; v<=num_vars; ++v) 
    {
    	if(fix[v]==1)
    	{
    		fix_var_count++;
    	}
        var_lit[v][var_lit_count[v]].clause_num=-1;//new var_lit boundary
    }

    cout<<"c unit propagation fixes "<<fix_var_count<<" variables, and delets "<<delete_clause_count<<" clauses"<<endl;
    
}
