
void synchronize_gldu(struct address_space *mapping)
{
	struct llist_node *entry;
	struct ldu_node *dnode, *next;
	struct ldu_head *lduh = &mapping->lduh;

	entry = llist_del_all(&lduh->ll_head);
	llist_for_each_entry_safe(dnode,
			next, entry, ll_node) {
		struct vm_area_struct *vma =
				ACCESS_ONCE(dnode->key);
		if (atomic_cmpxchg(&dnode->mark,
				1, 0) == 1) {
			ldu_physical_update(dnode->op_num,
					vma,
					ACCESS_ONCE(dnode->root));
		}
		clear_bit(dnode->op_num, &vma->dnode.used);
		if (atomic_cmpxchg(&dnode->mark, 1, 0) == 1) {
			ldu_physical_update(dnode->op_num, vma,
					ACCESS_ONCE(dnode->root));
		}
	}
}
