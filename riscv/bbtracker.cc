#include <stdlib.h>
#include <malloc.h>
#include "bbtracker.h"

bb_tracker_t::bb_tracker_t ()
{

  bb_id = 0;

  first_interval = 1;
  interval_sum=0; 
  
  dyn_inst=0;
  total_inst= 0;
  total_calls= 0;

}



void bb_tracker_t::init_bb_tracker (char* dir_name, char* out_name, uint64_t m_interval_size)
{
  int64_t i;

  outdir = dir_name;
  outfile = out_name;
  interval_size = m_interval_size;

  /* initialize hash ptr table */
  for (i=0; i<bb_size; i++)
    bb_hash[i] = NULL;

}


bb_node_ptr bb_tracker_t::create_bb_node (uint64_t pc, uint64_t num_inst)
{
  bb_node_ptr temp;

  temp = (bb_node_ptr) malloc(sizeof(bb_node));
  
  if (temp == NULL) {
    fprintf(stderr,"OUT OF MEMORY\n");
    exit(1);
  }

  temp->pc = pc;
  temp->bb_id = bb_id++;
  temp->count = num_inst;
  temp->next = NULL;

  return temp;
}


void bb_tracker_t::append_bb_node (bb_node_ptr m_bb_node, bb_node_ptr head)
{
  /* Append assumes head is non null. */

  while (head->next != NULL) 
    head = head->next;                                              

  head->next = m_bb_node;
}


/* Search for bb_node with pc, if found return 1, otherwise 
   return 0 */
int bb_tracker_t::find_bb_node (bb_node_ptr head, uint64_t pc, uint64_t num_inst)
{

  if (head != NULL) {
    while ((head->next != NULL) && (head->pc!=pc)) {
      head = head->next;
    }
    
    if ((head != NULL) && (head->pc == pc)) {
      head->count += num_inst;
      return 1;
    }
  }
  return 0;
} 

void bb_tracker_t::print_list (bb_node_ptr head, uint64_t array[]) 
{
  do {
    array[head->bb_id] = head->count;

    /* clear stats */
    head->count = 0;
    head = head->next;
  } while(head != NULL);
}

void bb_tracker_t::print_bb_hash (bb_node_ptr hash[])
{
  uint64_t i;
  uint64_t bb_array[bb_id];

  /* initialize array for sorting bb according to bb_id */
  for(i=0; i<bb_id; i++) {
    bb_array[i] = 0;
  }

  for(i=0; i<bb_size; i++) {
    if (hash[i] != NULL)
      print_list(hash[i], bb_array);
  }

  if (first_interval) {
    first_interval = 0;
    sprintf( finalname, "gzip -c > %s/%s.bb.gz", outdir, outfile );
    bbtrace = popen(finalname,"w");
  }

  fprintf(bbtrace,"T");
 
  for(i=0; i<bb_id; i++) { 
    if(bb_array[i] > 0) {
      fprintf( bbtrace, ":%" PRIu64 ":%" PRIu64 "   ", i+1, bb_array[i]);
    }
  }

  fprintf( bbtrace, "\n");
  fflush( bbtrace );
}


void bb_tracker_t::bb_tracker(uint64_t pc, uint64_t num_inst)
{
  /* key into bb-hash based on pc of last inst in bb*/
  uint64_t bb_key = (pc>>2)%bb_size;

  /* Increment bb with the number of instructions it contains */
  if (!find_bb_node(bb_hash[bb_key], pc, num_inst)) {

    /* new bb, need to create node for it */
    bb_node_ptr temp = create_bb_node(pc, num_inst);

    if (bb_hash[bb_key] == NULL) {                                           
      bb_hash[bb_key] = temp;                                                
    } else {
      append_bb_node(temp, bb_hash[bb_key]);                                     
    } 
  }

  dyn_inst += num_inst;
  total_inst += num_inst;

  /* if reached end of interval, dump stats and decrement counter */
  if (dyn_inst > interval_size) {
    dyn_inst -= interval_size;
    print_bb_hash(bb_hash);
  }
}
