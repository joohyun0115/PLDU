
void synchronize_ldu(struct obj_root *root)
{
	...

	//atomic remove first, lock-less list
	entry = xchg(&head->first, NULL);

	//iterate all logs
	llist_for_each_entry(dnode, entry, ll_node) {
		//get log's arguments
		...
		//atomic swap due to update-side removing
		if (xchg(&dnode->mark, 0))
			ldu_physical_update(dnode->op_num, arg,
					ACCESS_ONCE(dnode->root));
		clear_bit(dnode->op_num, &vma->dnode.used);
		// one more check due to reuse garbage log
		if (xchg(&dnode->mark, 0))
			ldu_physical_update(dnode->op_num, arg,
					ACCESS_ONCE(dnode->root));
	}
}
