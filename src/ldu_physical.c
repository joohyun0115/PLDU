
void synchronize_ldu(struct obj_root *root)
{
	//atomic swap due to remove all logs
	entry = xchg(&head->first, NULL);
	//iteration all logs
	llist_for_each_entry(log, entry, llist) {
		//get log's arguments
		//atomic swap due to update-side removing
		if (xchg(&log->mark, false))
			ldu_physical_update(log->op_num, arg, log->root);
		clear_bit(log->op_num, &obj->ldu.used);
		// one more check due to reuse garbage log
		if (xchg(&log->mark, false))
			ldu_physical_update(log->op_num, arg, log->root);
	}
}
