#ifndef _CW_H_
#define _CW_H_

#include "basis.h"

#define sigscore	ave_weight   //significant score needed for aspiration

int		ave_weight=1;
int		delta_total_weight=0;

/**************************************** clause weighting for 3sat **************************************************/

int		threshold;
float	p_scale;//w=w*p+ave_w*q
float	q_scale=0;
int		scale_ave;//scale_ave==ave_weight*q_scale

int 	q_init=0;

void smooth_clause_weights()
{
	int i,j,c,v;
	int new_total_weight=0;

	for (v=1; v<=num_vars; ++v) 
		score[v] = 0;
	
	//smooth clause score and update score of variables
	for (c = 0; c<num_clauses; ++c)
	{
		clause_weight[c] = clause_weight[c]*p_scale+scale_ave;
		if(clause_weight[c]<1) clause_weight[c] = 1;
		
		new_total_weight+=clause_weight[c];
		
		//update score of variables in this clause 
		if (sat_count[c]==0) 
		{
			for(j=0; j<clause_lit_count[c]; ++j)
			{
				score[clause_lit[c][j].var_num] += clause_weight[c];
			}
		}
		else  if(sat_count[c]==1)
			score[sat_var[c]]-=clause_weight[c];
	}
	
	ave_weight=new_total_weight/num_clauses;
}

void update_clause_weights()
{
	int i,v;

	for(i=0; i < unsat_stack_fill_pointer; ++i)
		clause_weight[unsat_stack[i]]++;
	
	for(i=0; i<unsatvar_stack_fill_pointer; ++i)
	{
		v = unsatvar_stack[i];
		score[v] += unsat_app_count[v];
		if(score[v]>0 &&  conf_change[v]==1 && already_in_goodvar_stack[v] ==0)
		{
			push(v,goodvar_stack);
			already_in_goodvar_stack[v] =1;
		}
	}
	
	delta_total_weight+=unsat_stack_fill_pointer;
	if(delta_total_weight>=num_clauses)
	{
		ave_weight+=1;
		delta_total_weight -= num_clauses;
		
		//smooth weights
		if(ave_weight>threshold)
			smooth_clause_weights();
	}
}


void set_clause_weighting()
{	
	threshold=300;
	p_scale=0.3;
	if(q_init==0)
	{
		if(ratio<=15) q_scale=0;
		else q_scale=0.7;
	}
	else 
	{
		if(q_scale<0.5)  //0
			q_scale = 0.7;
		else
			q_scale = 0;
	}
	
	scale_ave=(threshold+1)*q_scale;
	q_init = 1;
}

#endif
