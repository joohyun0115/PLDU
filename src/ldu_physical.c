
void synchronize_ldu(struct obj_root *root)
{
	//atomic swap due to remove all logs
	entry = SWAP(&head->first, NULL);
	//iteration all logs
	for_each_all_logs(log, entry, llist) {
		//get log's arguments
		//atomic swap due to update-side removing
		if (SWAP(&log->mark, false))
			ldu_physical_update(log->op_num, arg, log->root);
		CLEAR_BIT(log->op_num, &obj->ldu.used);
		// one more check due to reuse garbage log
		if (SWAP(&log->mark, false))
			ldu_physical_update(log->op_num, arg, log->root);
	}
}
