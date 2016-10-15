
void synchronize_ldu(struct obj_root *root)
{
	entry = SWAP(&head->first, NULL);
	//iteration all logs
	for_each_all_logs(log, entry, llist) {
		//... : get log's arguments
		//atomic swap due to update-side removing
		if (SWAP(&log->mark, false))
			ldu_apply_log(log->op_num, arg, log->root);
		CLEAR_BIT(log->op_num, &obj->ldu.used);
		// once again check due to reusing garbage logs
		if (SWAP(&log->mark, false))
			ldu_apply_log(log->op_num, arg, log->root);
	}
}
